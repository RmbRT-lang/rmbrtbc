#include "tokeniser.h"
#include "../malloc.h"
#include "../macros.h"
#include "../chartype.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>


struct {
	rlc_utf8_t const * str;
	RlcTokenType kw;
} const s_keywords [] = {
	// keywords.
	{"if", kRlcTokIf },
	{"else", kRlcTokElse },
	{"while", kRlcTokWhile },
	{"for", kRlcTokFor },
	{"continue", kRlcTokContinue },
	{"break", kRlcTokBreak },
	{"return", kRlcTokReturn },
	{"switch", kRlcTokSwitch },
	{"case", kRlcTokCase },
	{"fn", kRlcTokFn },
	{"inline", kRlcTokInline },
	{"void", kRlcTokVoid },
	{"namespace", kRlcTokNamespace },
	{"class", kRlcTokClass },
	{"struct", kRlcTokStruct },
	{"rawtype", kRlcTokRawtype },
	{"union", kRlcTokUnion },
	{"public", kRlcTokPublic },
	{"protected", kRlcTokProtected },
	{"private", kRlcTokPrivate },
	{"asm", kRlcTokAsm },
	{"static", kRlcTokStatic },
	{"dynamic", kRlcTokDynamic },
	{"virtual", kRlcTokVirtual },
	{"const", kRlcTokConst },
	{"volatile", kRlcTokVolatile },
	{"isolated", kRlcTokIsolated },
	{"this", kRlcTokThis },
}, s_operators [] = {
	// operators
	{ "+=", kRlcTokPlusEqual },
	{ "++", kRlcTokDoublePlus },
	{ "+", kRlcTokPlus },

	{ "-=", kRlcTokMinusEqual },
	{ "-:", kRlcTokMinusColon },
	{ "--", kRlcTokDoubleMinus },
	{ "->", kRlcTokMinusGreater },
	{ "-", kRlcTokMinus },

	{ "*=", kRlcTokAsteriskEqual },
	{ "*", kRlcTokAsterisk },

	{ "/=", kRlcTokForwardSlashEqual },
	{ "/", kRlcTokForwardSlash },

	{ "%=", kRlcTokPercentEqual },
	{ "%", kRlcTokPercent },

	{ "!=", kRlcTokExclamationMarkEqual },
	{ "!:", kRlcTokExclamationMarkColon },
	{ "!", kRlcTokExclamationMark },

	{ "^=", kRlcTokCircumflexEqual },
	{ "^", kRlcTokCircumflex },

	{ "~:", kRlcTokTildeColon },
	{ "~", kRlcTokTilde },

	{ "&&", kRlcTokDoubleAnd },
	{ "&", kRlcTokAnd },

	{ "?", kRlcTokQuestionMark },
	
	{ ":=", kRlcTokColonEqual },
	{ "::", kRlcTokDoubleColon },
	{ ":", kRlcTokColon },
	{ "...", kRlcTokTripleDot },
	{ ".", kRlcTokDot },
	{ ",", kRlcTokComma },
	{ ";", kRlcTokSemicolon },
	{ "==", kRlcTokDoubleEqual },

	{ "[", kRlcTokBracketL },
	{ "]", kRlcTokBracketR },
	{ "{", kRlcTokBraceL },
	{ "}", kRlcTokBraceR },
	{ "(", kRlcTokParentheseL },
	{ ")", kRlcTokParentheseR },

	{ "<<<=", kRlcTokTripleLessEqual },
	{ "<<<", kRlcTokTripleLess },
	{ "<<=", kRlcTokDoubleLessEqual },
	{ "<<", kRlcTokDoubleLess },
	{ "<=", kRlcTokLessEqual },
	{ "<", kRlcTokLess },
	
	{ ">>>=", kRlcTokTripleGreaterEqual },
	{ ">>>", kRlcTokTripleGreater },
	{ ">>=", kRlcTokDoubleGreaterEqual },
	{ ">>", kRlcTokDoubleGreater },
	{ ">=", kRlcTokGreaterEqual },
	{ ">", kRlcTokGreater },
};

int rlc_match_string(rlc_char_t const * src, rlc_char_t const * to_match)
{
	size_t len = rlc_strlen(to_match);
	return rlc_strncmp(src, to_match, len);
}

RlcTokResult rlc_finish_token(
	struct RlcToken * tok,
	size_t begin,
	size_t end,
	RlcTokenType type)
{
	tok->fBegin = begin;
	tok->fLength = end - begin;
	tok->fType = type;

	return kRlcTokResultOk;
}

int rlc_floating_type(rlc_char_t suffix, RlcTokenType * out)
{
	switch(suffix)
	{
	case 'h':
	case 'H':
		return (*out = kRlcTokBin16), 1;
	case 's':
	case 'S':
		return (*out = kRlcTokBin32), 1;
	case 'd':
	case 'D':
		return (*out = kRlcTokBin64), 1;
	case 'q':
	case 'Q':
		return (*out = kRlcTokBin128), 1;
	default:
		return 0;
	}
}

RlcTokResult rlc_parse_character(
	rlc_char_t const * src,
	size_t index,
	size_t * length)
{
	switch(src[index])
	{
	case '\0':
	case '\n':
		return kRlcTokResultUnclosedLiteral;
	case '\\':
		{
			switch(src[index+1])
			{
			case 'x':
			case 'X':
				{
					if(!rlc_is_hex(src[index+2]) || !rlc_is_hex(src[index+3]))
						return kRlcTokResultInvalidHexEscape;
					else
						return
							(*length = 3),
							kRlcTokResultOk;
				} break;
			case 'o':
			case 'O':
				{
					if(!rlc_is_octal(src[index+2])
					|| !rlc_is_octal(src[index+3])
					|| !rlc_is_octal(src[index+4])
					|| src[index+2] > '3')
						return kRlcTokResultInvalidOctalEscape;
					else
						return
							(*length = 4),
							kRlcTokResultOk;
				} break;
			case 'd':
			case 'D':
				{
					if(!rlc_is_decimal(src[index+2])
					|| !rlc_is_decimal(src[index+3])
					|| !rlc_is_decimal(src[index+4])
					|| src[index+2] > '2'
					|| (src[index+2] == '2' && src[index+3] >'5')
					|| (src[index+2] == '2' && src[index+3] == '5' && src[index+4] > '5'))
						return kRlcTokResultInvalidDecimalEscape;
					else
						return
							(*length = 4),
							kRlcTokResultOk;
				} break;
			case '\\':
			case 'n':
			case 'r':
			case 't':
			case 'f':
			case 'v':
			case 'b':
			case 'a':
			case '"':
			case '\'':
			case '0':
				{
					return
						(*length = 2),
						kRlcTokResultOk;
				} break;
			default:
				{
					return kRlcTokResultInvalidEscape;
				} break;
			}
		} break;
	default:
		{
			return (*length = 1), kRlcTokResultOk;
		} break;
	}
}

RlcTokResult rlc_next_token(
	rlc_char_t const * src,
	size_t offset,
	struct RlcToken * out,
	size_t * error_index)
{
	size_t index = offset;

	if(!src[index])
		return kRlcTokResultEos;


	RlcTokenType unicode_modifier;
	int unicode_modifier_found = 0;


	static struct {
		rlc_char_t const match[5];
		RlcTokenType tokentype;
		int skip;
	} const utf_literals[] = {
		// string literals.
		{{'8','\"',0,0,0}, kRlcTokUtf8String, 1},
		
		{{'1','6','L','\"',0}, kRlcTokUtf16leString, 3},
		{{'1','6','l','\"',0}, kRlcTokUtf16leString, 3},
		{{'1','6','B','\"',0}, kRlcTokUtf16beString, 3},
		{{'1','6','b','\"',0}, kRlcTokUtf16beString, 3},

		{{'3','2','L','\"',0}, kRlcTokUtf32leString, 3},
		{{'3','2','l','\"',0}, kRlcTokUtf32leString, 3},
		{{'3','2','B','\"',0}, kRlcTokUtf32beString, 3},
		{{'3','2','b','\"',0}, kRlcTokUtf32beString, 3},

		// char literals.
		{{'8','\'',0,0,0}, kRlcTokUtf8CharNumber, 1},
		{{'1','6','\'',0,0}, kRlcTokUtf16CharNumber, 2},
		{{'3','2','\'',0,0}, kRlcTokUtf32CharNumber, 2}
	};


	for(int i = 0; i<_countof(utf_literals); i++)
	{
		if(rlc_match_string(src+index, utf_literals[i].match) == 0)
		{
			unicode_modifier = utf_literals[i].tokentype;
			unicode_modifier_found = 1;
			index += utf_literals[i].skip;
			break;
		}
	}

	switch(src[index])
	{
	case '"':
		{
			++index;
			while(src[index] != '"')
			{
				size_t length;
				RlcTokResult result = rlc_parse_character(
					src,
					index,
					&length);

				if(result != kRlcTokResultOk)
					return (*error_index = index), result;
				else
					index += length;
			}
			++index;

			return rlc_finish_token(out, offset, index,
				unicode_modifier_found
					? unicode_modifier
					: kRlcTokString);
		} break;
	case '\'':
		{
			if(src[++index] == '\'')
				return kRlcTokResultEmptyCharLiteral;
			while(src[index] != '\'')
			{
				size_t length;
				RlcTokResult result = rlc_parse_character(
					src,
					index,
					&length);

				if(result != kRlcTokResultOk)
					return (*error_index = index), result;
				else
					index += length;
			}

			++index;

			return rlc_finish_token(out, offset, index,
				unicode_modifier_found
				? unicode_modifier
				: kRlcTokCharNumber);
		} break;
	default:
		{
			if(rlc_is_whitespace(src[index]))
			{
				while(rlc_is_whitespace(src[++index]));
				return rlc_finish_token(out, offset, index, kRlcTokWhitespace);
			} else if(rlc_is_ident_first_char(src[index]))
			{
				while(rlc_is_ident_last_char(src[++index]));

				for(int i = 0; i < _countof(s_keywords); i++)
				{
					int len_to_cmp = strlen(s_keywords[i].str);
					if(index-offset > len_to_cmp)
						len_to_cmp = index-offset;
					
					if(!rlc_strncmp_utf8(src+offset, s_keywords[i].str, len_to_cmp))
					{
						return rlc_finish_token(out, offset, index, s_keywords[i].kw);
					}
				}

				return rlc_finish_token(out, offset, index, kRlcTokIdentifier);
			} else if(rlc_is_decimal(src[index]))
			{
				if(src[index] == '0')
				{
					++index;
					if(src[index] == 'x' || src[index] == 'X')
					{
						while(rlc_is_hex(src[++index]));
						if(rlc_is_ident_first_char(src[index]))
							return (*error_index = index), kRlcTokResultInvalidHexDigit;
						else
							return rlc_finish_token(out, offset, index, kRlcTokHexNumber);
					} else if(rlc_is_decimal(src[index]))
					{
						while(rlc_is_octal(src[++index]));
						if(rlc_is_decimal(src[index]) || rlc_is_ident_first_char(src[index]))
							return (*error_index = index), kRlcTokResultInvalidOctalDigit;
						else
							return rlc_finish_token(out, offset, index, kRlcTokOctalNumber);
					} else if(rlc_is_ident_first_char(src[index]))
						return kRlcTokResultInvalidOctalDigit;
					else
						return rlc_finish_token(out, offset, index, kRlcTok0);
				} else
				{
					while(rlc_is_decimal(src[index]))
						index++;
					if(rlc_is_ident_first_char(src[index]))
						return (*error_index = index), kRlcTokResultInvalidDecimalDigit;
					else if(src[index] == '.')
					{
						while(rlc_is_decimal(src[++index]));
						RlcTokenType type;
						if(rlc_floating_type(src[index], &type))
							return rlc_finish_token(out, offset, ++index, type);
						else
							return (*error_index = index),
								kRlcTokResultInvalidFloatingSpecifier;
					} else
						return rlc_finish_token(out, offset, index, kRlcTokDecimalNumber);
				}
			} else
			{
				for(int i = 0; i < _countof(s_operators); i++)
				{
					int len_to_cmp = strlen(s_operators[i].str);
					
					if(!rlc_strncmp_utf8(src+offset, s_operators[i].str, len_to_cmp))
						return rlc_finish_token(out, offset, index+len_to_cmp, s_operators[i].kw);
				}

				return (*error_index = index), kRlcTokResultUnexpected;
			}
		} break;
	}
}


RlcTokResult rlc_tokenise(
	rlc_char_t const * src,
	struct RlcToken ** out,
	int skip_whitespaces,
	size_t * count,
	size_t * error_index)
{

	size_t const len = src ? rlc_strlen(src) : 0;

	*count = 0;
	*out = 0;

	if(!len)
		return kRlcTokResultOk;

	size_t index = 0;
	struct RlcToken tok;
	RlcTokResult result;
	while(index < len)
	{
		if((result=rlc_next_token(src, index, &tok, error_index)) == kRlcTokResultOk)
		{
			if(!(tok.fType == kRlcTokWhitespace && skip_whitespaces))
			{
				rlc_realloc((void**)out, ++*count * sizeof(struct RlcToken));
				(*out)[*count-1] = tok;
			}
			index += tok.fLength;
		}
		else
		{
			if(*out)
				rlc_free((void**)out);
			*count = 0;

			return result;
		}
	}

	return kRlcTokResultOk;
}