#include "tokeniser.h"
#include "../malloc.h"
#include "../macros.h"
#include "../error.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static _Noreturn void tok_error(
	struct RlcTokeniser const * this,
	char const * message);

static void skip(
	struct RlcTokeniser * this);
static int skip_whitespace(
	struct RlcTokeniser * this);
static int skip_comment(
	struct RlcTokeniser * this);

static int identifier(
	struct RlcTokeniser * this);
static int number(
	struct RlcTokeniser * this);
static int op(
	struct RlcTokeniser * this);
static int character(
	struct RlcTokeniser * this);
static int string(
	struct RlcTokeniser * this);

static char look(
	struct RlcTokeniser const * this);
static char ahead(
	struct RlcTokeniser const * this,
	size_t n);
static int eof(
	struct RlcTokeniser const * this);
static int take_str(
	struct RlcTokeniser * this,
	char const * string);
static char take(
	struct RlcTokeniser * this);
static void ignore(
	struct RlcTokeniser * this,
	RlcSrcIndex count);

void rlc_tokeniser_create(
	struct RlcTokeniser * this,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	this->fSource = file;
	this->fIndex = 0;
	this->fStart = 0;

	skip(this);
}

int rlc_tokeniser_read(
	struct RlcTokeniser * this,
	struct RlcToken * token)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(token != NULL);

	token->content.start = this->fStart = this->fIndex;

	if(!identifier(this)
	&& !string(this)
	&& !character(this)
	&& !number(this)
	&& !op(this))
		tok_error(this, "unexpected character");

	token->content.length = this->fIndex - this->fStart;
	token->type = this->fType;

	skip(this);
	return look(this) != '\0';
}

void tok_error(
	struct RlcTokeniser const * this,
	char const * message)
{
	struct RlcSrcPosition pos;
	rlc_src_file_position(this->fSource, &pos, this->fIndex);

	fprintf(stderr, "%s:%u:%u: error: %s\n",
		this->fSource->fName,
		pos.line,
		pos.column,
		message);

	if(this->fStart != this->fIndex)
	{
		rlc_src_file_position(this->fSource, &pos, this->fStart);
		fprintf(stderr, "%s:%u:%u: error: caused here.\n",
			this->fSource->fName,
			pos.line,
			pos.column);
	}

	exit(EXIT_FAILURE);
}

void skip(
	struct RlcTokeniser * this)
{
	RLC_DASSERT(this != NULL);

	while(skip_whitespace(this) || skip_comment(this));
}

int skip_whitespace(
	struct RlcTokeniser * this)
{
	int skipped = 0;
	char c;
	while((c = look(this)))
	{
		static char const k_chars[] = " \t\r\n\v";
		for(size_t i = 0; i < sizeof(k_chars)-1; i++)
		{
			if(k_chars[i] == c)
			{
				skipped = 1;
				ignore(this, 1);
				continue;
			}
		}
		break;
	}
	return skipped;
}

int skip_comment(
	struct RlcTokeniser * this)
{
	if(take_str(this, "//"))
	{
		char c;
		while((c = take(this)))
			if(c == '\n')
				break;
		return 1;
	} else if(take_str(this, "(/"))
	{
		while(!eof(this))
		{
			if(take_str(this, "/)"))
				return 1;
			else
				ignore(this, 1);
		}
		tok_error(this, "unterminated block comment");
	} else
		return 0;
}


struct {
	char const * str;
	enum RlcTokenType kw;
} const s_keywords [] = {
	// keywords.
		{"break", kRlcTokBreak },
		{"case", kRlcTokCase },
		{"constructor", kRlcTokConstructor },
		{"continue", kRlcTokContinue },
		{"default", kRlcTokDefault },
		{"destructor", kRlcTokDestructor },
		{"do", kRlcTokDo },
		{"else", kRlcTokElse },
		{"enum", kRlcTokEnum},
		{"extern", kRlcTokExtern },
		{"for", kRlcTokFor },
		{"if", kRlcTokIf },
		{"include", kRlcTokInclude },
		{"inline", kRlcTokInline },
		{"isolated", kRlcTokIsolated },
		{"namespace", kRlcTokNamespace },
		{"number", kRlcTokNumber },
		{"operator", kRlcTokOperator },
		{"private", kRlcTokPrivate },
		{"protected", kRlcTokProtected },
		{"public", kRlcTokPublic },
		{"return", kRlcTokReturn },
		{"sizeof", kRlcTokSizeof },
		{"static", kRlcTokStatic },
		{"switch", kRlcTokSwitch },
		{"this", kRlcTokThis },
		{"type", kRlcTokType },
		{"union", kRlcTokUnion },
		{"virtual", kRlcTokVirtual },
		{"void", kRlcTokVoid },
		{"while", kRlcTokWhile },
	}, s_operators [] = {
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

		{ "::=", kRlcTokDoubleColonEqual },
		{ ":=", kRlcTokColonEqual },
		{ "::", kRlcTokDoubleColon },
		{ ":", kRlcTokColon },
		{ "@@", kRlcTokDoubleAt },
		{ "@", kRlcTokAt },
		{ "..!", kRlcTokDoubleDotExclamationMark },
		{ "..?", kRlcTokDoubleDotQuestionMark },
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

		{ "$", kRlcTokDollar },
		{ "#", kRlcTokHash },
};

static int is_identifier_start(char c)
{
	return (c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| (c == '_');
}
static int is_identifier_end(char c)
{
	return (c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| (c >= '0' && c <= '9')
		|| (c == '_');
}

int identifier(
	struct RlcTokeniser * this)
{
	if(!is_identifier_start(look(this)))
		return 0;

	do ignore(this, 1); while(is_identifier_end(look(this)));

	struct RlcSrcString str;
	str.start = this->fStart;
	str.length = this->fIndex - this->fStart;

	for(size_t i = 0; i < _countof(s_keywords); i++)
		if(0 == rlc_src_string_cmp_cstr(this->fSource, &str, s_keywords[i].str))
		{
			this->fType = s_keywords[i].kw;
			return 1;
		}

	this->fType = kRlcTokIdentifier;
	return 1;
}

inline static int is_decimal(char c)
{
	return (unsigned char)(c -'0') < 10;
}
inline static int is_octal(char c)
{
	return (unsigned char)(c -'0') < 8;
}
inline static int is_binary(char c)
{
	return (unsigned char)(c -'0') < 2;
}
static int is_hexadecimal(char c)
{
	return (unsigned char)(c - 'a') < 6
		|| (unsigned char)(c - 'A') < 6
		|| is_decimal(c);
}

int number(
	struct RlcTokeniser * this)
{
	char c = look(this);
	if(c >= '1' && c <= '9')
	{
		do ignore(this, 1); while(is_decimal(look(this)));
	} else if(c == '0')
	{
		ignore(this, 1);

		switch(look(this))
		{
		case 'x':
		case 'X':
			{
				do ignore(this, 1); while(is_hexadecimal(look(this)));
			} break;
		case 'b':
		case 'B':
			{
				do ignore(this, 1); while(is_binary(look(this)));
			} break;
		default:
			while(is_octal(look(this))) ignore(this, 1);
		}
	} else
		return 0;

	this->fType = kRlcTokNumberLiteral;
	return 1;
}

int op(
	struct RlcTokeniser * this)
{
	for(size_t i = 0; i < _countof(s_operators); i++)
		if(take_str(this, s_operators[i].str))
		{
			this->fType = s_operators[i].kw;
			return 1;
		}
	return 0;
}

int character(
	struct RlcTokeniser * this)
{
	static char const * k_start[] =
	{
		"32L\'", "32B\'", "32\'"
		"16L\'", "16B\'", "16\'",
		"32l\'", "32b\'",
		"16l\'", "16b\'",
		"8\'",
		"\'"
	};

	for(size_t i = 0; i < _countof(k_start); i++)
		if(take_str(this, k_start[i]))
		{
			char c;
			while((c = take(this)))
			{
				if(c == '\'')
				{
					this->fType = kRlcTokCharacterLiteral;
					return 1;
				}
				else if(c == '\\')
					take(this);
				else if(c == '\n')
					tok_error(this, "forbidden newline in character literal");
			}
			tok_error(this, "unterminated character literal");
		}
	return 0;
}

int string(
	struct RlcTokeniser * this)
{
	static char const * k_start[] =
	{
		"32L\"", "32B\"", "32\""
		"16L\"", "16B\"", "16\"",
		"32l\"", "32b\"",
		"16l\"", "16b\"",
		"8\"",
		"\""
	};

	for(size_t i = 0; i < _countof(k_start); i++)
		if(take_str(this, k_start[i]))
		{
			char c;
			while((c = take(this)))
			{
				if(c == '"')
				{
					this->fType = kRlcTokStringLiteral;
					return 1;
				}
				else if(c == '\\')
					ignore(this, 1);
				else if(c == '\n')
					tok_error(this, "forbidden newline in string literal");
			}
			tok_error(this, "unterminated string literal");
		}

	return 0;
}
char ahead(
	struct RlcTokeniser const * this,
	size_t n)
{
	if(this->fIndex + n < this->fSource->fContentLength)
		return this->fSource->fContents[this->fIndex+n];
	else
		return '\0';
}

int eof(
	struct RlcTokeniser const * this)
{
	return this->fIndex >= this->fSource->fContentLength;
}

char look(
	struct RlcTokeniser const * this)
{
	return ahead(this, 0);
}

int take_str(
	struct RlcTokeniser * this,
	char const * string)
{
	size_t i;
	for(i = 0; string[i]; i++)
	{
		if(string[i] != ahead(this, i))
			return 0;
	}

	ignore(this, i);
	return 1;
}

char take(
	struct RlcTokeniser * this)
{
	char c = look(this);
	if(c) ignore(this, 1);
	return c;
}

void ignore(
	struct RlcTokeniser * this,
	RlcSrcIndex n)
{
	this->fIndex += n;
	RLC_DASSERT(this->fIndex <= this->fSource->fContentLength);
}