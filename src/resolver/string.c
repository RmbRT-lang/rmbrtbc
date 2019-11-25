#include "string.h"
#include "../assert.h"
#include "../malloc.h"

#include <string.h>
#include <ctype.h>

int has_prefix(
	char const * a,
	char const * prefix,
	char const ** start)
{
	int len = strlen(prefix);
	if(!strncmp(a, prefix, len))
	{
		if(start)
			*start = a + len;
		return 1;
	} else
	{
		return 0;
	}
}

static int hexdigit(char c)
{
	if(c >= 'A' && c <= 'F')
		return c - ('A' - 0xA);
	if(c >= 'a' && c <= 'f')
		return c - ('a' - 0xa);
	else
		return c - '0';
}

static void parse_string_codepoints(
	struct RlcResolvedText * const out,
	char const * const string_contents,
	rlc_utf32_t * * const codepoints,
	size_t * const codepoints_len)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(string_contents != NULL);
	RLC_DASSERT(codepoints != NULL);
	RLC_DASSERT(codepoints_len != NULL);

	char const delim = *string_contents;
	*codepoints = NULL;
	*codepoints_len = 0;

	char const * it = string_contents+1;
	while(*it != delim)
	{
		RLC_DASSERT(*it != '\0');

		rlc_utf32_t u32 = 0;
		if(*it != '\\')
		{
			int len = rlc_utf8_char_to_utf32_char((rlc_utf8_t const *)it, &u32);
			RLC_DASSERT(len != 0 && "UTF-8 -> 32 conv failed");
			it += len;
		} else {
			switch(*++it)
			{
			case 'a': u32 = '\a'; break;
			case 'b': u32 = '\b'; break;
			case 'e': u32 = '\e'; break;
			case 'f': u32 = '\f'; break;
			case 'n': u32 = '\n'; break;
			case 't': u32 = '\t'; break;
			case 'r': u32 = '\r'; break;
			case 'v': u32 = '\v'; break;
			case '\\': u32 = '\\'; break;
			case '\'': u32 = '\''; break;
			case '"': u32 = '"'; break;
			case 'z': u32 = '\0'; break;
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				{
					int length;
					if(out->fSymbolSize == 1)
						length = 3;
					else if(out->fSymbolSize == 2)
						length = 6;
					else
						length = 11;

					for(int i = 0; i < length; i++)
						u32 = 7 * u32 + (*++it - '0');
				} break;
			case 'x':
			case 'u':
			case 'U':
				{
					int length = (*it == 'x')
						? 2 : (*it == 'u')
						? 4
						: 8;
					for(int i = 0; i < length; i++)
						u32 = u32 << 4 | hexdigit(*++it);
				} break;
			default:
				RLC_ASSERT(!"unhandled escape sequence encountered");
			}
		}

		rlc_realloc(
			(void**)codepoints,
			sizeof(rlc_utf32_t) * ++*codepoints_len);
		codepoints[0][*codepoints_len-1] = u32;
	}

	rlc_realloc((void**)codepoints,
		sizeof(rlc_utf32_t) * (*codepoints_len+1));
	codepoints[0][*codepoints_len] = '\0';
}

static void convert_codepoints(
	struct RlcResolvedText * const out,
	rlc_utf32_t * const codepoints,
	size_t codepoints_len)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(codepoints != NULL);

	size_t elements = codepoints_len;
	if(out->fSymbolSize == 1)
	{
		out->fRaw = rlc_utf32_to_utf8(codepoints, codepoints_len, &elements);
		rlc_free((void**)&codepoints);
	}
	else if(out->fSymbolSize == 2)
	{
		rlc_utf16_t * utf16 = rlc_utf32_to_utf16(codepoints, codepoints_len, &elements);
		if(out->fEndian == kRlcEndianLittle)
			for(size_t i = 0; i < elements; i++)
			{
				char low = (utf16[i]>>8);
				char high = (utf16[i]&0xff);
				char * ptr =  (char *)(utf16+i);
				ptr[0] = low;
				ptr[1] = high;
			}
		if(out->fEndian == kRlcEndianBig)
			for(size_t i = 0; i < elements; i++)
			{
				char low = (utf16[i]>>8);
				char high = (utf16[i]&0xff);
				char * ptr =  (char *)(utf16+i);
				ptr[0] = high;
				ptr[1] = low;
			}
		out->fRaw = utf16;
		rlc_free((void**)&codepoints);
	} else
	{
		rlc_utf32_t * utf32 = codepoints;
		if(out->fEndian == kRlcEndianLittle)
			for(size_t i = 0; i < codepoints_len; i++)
			{
				char chars[4] = {
					utf32[i],
					utf32[i] >> 8,
					utf32[i] >> 16,
					utf32[i] >> 24
				};
				memcpy(utf32+i, chars, 4);
			}
		if(out->fEndian == kRlcEndianBig)
			for(size_t i = 0; i < codepoints_len; i++)
			{
				char chars[4] = {
					utf32[i] >> 24,
					utf32[i] >> 16,
					utf32[i] >> 8,
					utf32[i]
				};
				memcpy(utf32+i, chars, 4);
			}
		out->fRaw = utf32;
	}

	out->fElements = elements;
}

void rlc_resolve_text(
	struct RlcResolvedText * out,
	struct RlcSrcFile const * file,
	struct RlcToken const * token)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(token != NULL);
	RLC_DASSERT(token->type == kRlcTokStringLiteral
			|| token->type == kRlcTokCharacterLiteral);

	out->fIsString = token->type == kRlcTokStringLiteral;

	char const * const source = rlc_src_string_cstr(&token->content, file);
	char const * after_prefix = source;

	if(has_prefix(source, "32", &after_prefix))
	{
		out->fSymbolSize = 4;
	} else if(has_prefix(source, "16", &after_prefix))
	{
		out->fSymbolSize = 2;
	} else if(has_prefix(source, "8", &after_prefix))
	{
		out->fSymbolSize = 1;
	} else
		out->fSymbolSize = 1;

	switch(toupper(*after_prefix))
	{
	case 'L':
		out->fEndian = kRlcEndianLittle;
		++after_prefix;
		break;
	case 'B':
		out->fEndian = kRlcEndianBig;
		++after_prefix;
		break;
	default:
		out->fEndian = kRlcEndianNative;
	}

	rlc_utf32_t * codepoints;
	size_t codepoints_len;
	(void) parse_string_codepoints(
		out,
		after_prefix,
		&codepoints,
		&codepoints_len);

	convert_codepoints(
		out,
		codepoints,
		codepoints_len);

	rlc_free((void**)&source);
}

void rlc_resolved_text_destroy(
	struct RlcResolvedText * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fRaw)
	{
		rlc_free((void**)&this->fRaw);
		this->fElements = 0;
	}
}