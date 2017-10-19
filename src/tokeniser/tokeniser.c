#include "tokeniser.h"
#include "../malloc.h"
#include "../macros.h"
#include "../chartype.h"
#include "../error.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>


struct {
	rlc_utf8_t const * str;
	enum RlcTokenType kw;
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
	{"typedef", kRlcTokTypedef},
	{"enum", kRlcTokEnum},
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
	{"type", kRlcTokType },
	{"number", kRlcTokNumber },
	{"operator", kRlcTokOperator }
}, s_operators [] = {
	{ "@", kRlcTokDynamic },
	// operators
	{ "+=", kRlcTokPlusEqual },
	{ "++", kRlcTokDoublePlus },
	{ "+", kRlcTokPlus },

	{ "-=", kRlcTokMinusEqual },
	{ "-:", kRlcTokMinusColon },
	{ "--", kRlcTokDoubleMinus },
	{ "->*", kRlcTokMinusGreaterAsterisk },
	{ "->", kRlcTokMinusGreater },
	{ "-", kRlcTokMinus },

	{ "*=", kRlcTokAsteriskEqual },
	{ "*", kRlcTokAsterisk },

	{ "\\", kRlcTokBackslash },

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

	{ "&&=", kRlcTokDoubleAndEqual },
	{ "&&", kRlcTokDoubleAnd },
	{ "&=", kRlcTokAndEqual },
	{ "&", kRlcTokAnd },

	{ "||=", kRlcTokDoublePipeEqual },
	{ "||", kRlcTokDoublePipe },
	{ "|=", kRlcTokPipeEqual },
	{ "|", kRlcTokPipe },

	{ "?", kRlcTokQuestionMark },

	{ ":=", kRlcTokColonEqual },
	{ "::", kRlcTokDoubleColon },
	{ ":", kRlcTokColon },
	{ "...", kRlcTokTripleDot },
	{ ".*", kRlcTokDotAsterisk },
	{ ".", kRlcTokDot },
	{ ",", kRlcTokComma },
	{ ";", kRlcTokSemicolon },
	{ "==", kRlcTokDoubleEqual },

	{ "[", kRlcTokBracketOpen },
	{ "]", kRlcTokBracketClose },
	{ "{", kRlcTokBraceOpen },
	{ "}", kRlcTokBraceClose },
	{ "(", kRlcTokParentheseOpen },
	{ ")", kRlcTokParentheseClose },

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

	{ "$", kRlcTokVolatile },
	{ "#", kRlcTokConst },
};

int rlc_match_string(
	rlc_char_t const * src,
	rlc_char_t const * to_match)
{
	size_t len = rlc_strlen(to_match);
	return rlc_strncmp(src, to_match, len);
}

enum RlcTokResult rlc_finish_token(
	struct RlcToken * tok,
	size_t begin,
	size_t end,
	enum RlcTokenType type)
{
	tok->fBegin = begin;
	tok->fLength = end - begin;
	tok->fType = type;

	return kRlcTokResultOk;
}

int rlc_floating_type(
	rlc_char_t suffix,
	enum RlcTokenType * out)
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

enum RlcTokResult rlc_parse_character(
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

enum RlcTokResult rlc_next_token(
	rlc_char_t const * src,
	size_t offset,
	struct RlcToken * out,
	size_t * error_index)
{
	size_t index = offset;

	if(!src[index])
		return kRlcTokResultEos;


	enum RlcTokenType unicode_modifier;
	int unicode_modifier_found = 0;

	if(src[index] == '/')
	{
		if(src[index+1] == '*')
		{
			index += 2;
			while(src[index])
			{
				if(src[index] == '*'
				&& src[index+1] == '/')
				{
					return rlc_finish_token(out, offset, index + 2, kRlcTokComment);
				}
				else
					++index;
			}
			return (*error_index = offset), kRlcTokResultUnclosedComment;
		} else if(src[index+1] == '/')
		{
			index += 2;

			while(src[index] && src[index] != '\n')
				++index;

			return rlc_finish_token(out, offset, index+1, kRlcTokComment);
		}
	}


	static struct {
		rlc_char_t const match[5];
		enum RlcTokenType tokentype;
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
				enum RlcTokResult result = rlc_parse_character(
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
				enum RlcTokResult result = rlc_parse_character(
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
						if(!rlc_is_hex(src[++index]))
							return (*error_index = index), kRlcTokResultInvalidHexDigit;
						while(rlc_is_hex(src[++index]));

						if(src[index] == '.')
						{
							while(rlc_is_hex(src[++index]));
							enum RlcTokenType type;
							if(src[index == '\''] && ++index
							&& rlc_floating_type(src[index], &type) && ++index)
								if(rlc_is_ident_first_char(src[index]))
									return (*error_index = index), kRlcTokResultInvalidHexDigit;
								else
									return rlc_finish_token(out, offset, index, type);
							else
								return (*error_index = index),
									kRlcTokResultInvalidFloatingSpecifier;
						}

						if(rlc_is_ident_first_char(src[index]))
							return (*error_index = index), kRlcTokResultInvalidHexDigit;
						else
							return rlc_finish_token(out, offset, index, kRlcTokHexNumber);
					} else if(rlc_is_octal(src[index]))
					{
						while(rlc_is_octal(src[++index]));
						if(rlc_is_decimal(src[index]) || rlc_is_ident_first_char(src[index]))
							return (*error_index = index), kRlcTokResultInvalidOctalDigit;
						else if(src[index] == '.')
						{
							while(rlc_is_octal(src[++index]));
							enum RlcTokenType type;
							if(src[index == '\''] && ++index
							&& rlc_floating_type(src[index], &type) && ++index)
								if(rlc_is_ident_first_char(src[index]))
									return (*error_index = index), kRlcTokResultInvalidHexDigit;
								else
									return rlc_finish_token(out, offset, index, type);
							else
								return (*error_index = index),
									kRlcTokResultInvalidFloatingSpecifier;
						}
						else
							return rlc_finish_token(out, offset, index, kRlcTokOctalNumber);
					} else if(rlc_is_ident_first_char(src[index]))
						return (*error_index = index),
							kRlcTokResultInvalidOctalDigit;
					else if(src[index] == '.')
					{
						while(rlc_is_decimal(src[++index]));
						enum RlcTokenType type;
						if(src[index == '\''] && ++index
						&& rlc_floating_type(src[index], &type) && ++index)
							if(rlc_is_ident_first_char(src[index]))
								return (*error_index = index), kRlcTokResultInvalidHexDigit;
							else
								return rlc_finish_token(out, offset, index, type);
						else
							return (*error_index = index),
								kRlcTokResultInvalidFloatingSpecifier;
					}
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
						enum RlcTokenType type;
						if(src[index == '\''] && ++index
						&& rlc_floating_type(src[index], &type) && ++index)
							if(rlc_is_ident_first_char(src[index]))
								return (*error_index = index), kRlcTokResultInvalidHexDigit;
							else
								return rlc_finish_token(out, offset, index, type);
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


static size_t get_line_number(
	rlc_char_t const * src,
	size_t const index,
	size_t * col,
	size_t * line_len)
{
	size_t line = 0;
	size_t lastline = 0;
	for(size_t i = 0; i < index; i++)
	{
		if(src[i] == '\n')
		{
			++line;
			lastline = i+1;
		}
	}
	size_t i;
	for(i = lastline; src[i] && src[i] != '\n'; i++) { ; }

	if(src[i] == '\n')
		--i;

	* col = index - lastline + 1;
	* line_len = i - lastline + 1;
	return line+1;
}

enum RlcTokResult rlc_tokenise(
	rlc_char_t const * src,
	struct RlcFile * out,
	int skip_whitespaces,
	int skip_comments,
	size_t * error_index)
{

	size_t const len = src ? rlc_strlen(src) : 0;

	out->fTokenCount = 0;
	out->fTokens = NULL;

	if(!len)
		return kRlcTokResultOk;

	size_t index = 0;
	struct RlcToken tok;
	enum RlcTokResult result;
	while(index < len)
	{
		tok.fFile = out;
		if((result=rlc_next_token(src, index, &tok, error_index)) == kRlcTokResultOk)
		{
			if(!(tok.fType == kRlcTokWhitespace && skip_whitespaces)
			&& !(tok.fType == kRlcTokComment && skip_comments))
			{
				rlc_realloc((void**)&out->fTokens, ++out->fTokenCount * sizeof(struct RlcToken));
				out->fTokens[out->fTokenCount-1] = tok;
			}
			index += tok.fLength;
		}
		else
		{
			size_t col;
			size_t len;
			size_t line = get_line_number(
				src,
				*error_index,
				&col,
				&len);

			rlc_char_t * content = NULL;
			rlc_malloc(
				(void**) &content,
				sizeof(rlc_char_t) * len + 1);

			memcpy(content, src + (*error_index - col + 1), sizeof(rlc_char_t) * len);
			content[len] = '\0';

			rlc_report_lexical_error(
				out->fName,
				line,
				col,
				content,
				result);

			if(out->fTokens)
				rlc_free((void**)&out->fTokens);
			out->fTokenCount = 0;

			return result;
		}
	}

	return kRlcTokResultOk;
}