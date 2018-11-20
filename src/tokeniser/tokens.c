#include "tokens.h"

#include "../assert.h"
#include "../malloc.h"

char const * rlc_tok_result_message(
	enum RlcTokResult result)
{
	static char const * messages[] = {
		"tokenising successful",
		"end of source file reached",
		"invalid hexadecimal digit",
		"invalid octal digit",
		"invalid decimal digit",
		"invalid floating type specifier",
		"unclosed caracter/string literal",
		"invalid hexadecimal escape sequence",
		"invalid octal escape sequence",
		"invalid decimal escape sequence",
		"invalid escape sequence",
		"invalid UTF-8 byte",
		"empty character literal",
		"unclosed comment",
		"unexpected symbol"
	};

	static_assert(RLC_COVERS_ENUM(messages, RlcTokResult), "mal-sized table.");

	RLC_DASSERT(RLC_IN_ENUM(result, RlcTokResult));

	return messages[result];
}

char const * rlc_token_type_name(
	enum RlcTokenType type)
{
	static char const * const names[] = {
		"Whitespace",
		"Comment",
		"Identifier",
		"HexNumber",
		"DecimalNumber",
		"OctalNumber",
		"CharNumber",
		"Utf8CharNumber",
		"Utf16CharNumber",
		"Utf32CharNumber",
		"0",
		"String",
		"Utf8String",
		"Utf16leString",
		"Utf16beString",
		"Utf32leString",
		"Utf32beString",

		"Bin16",
		"Bin32",
		"Bin64",
		"Bin128",

		"Backslash",
		"Comma",
		"Dot",
		"DotAsterisk",
		"TripleDot",
		"Plus",
		"PlusEqual",
		"DoublePlus",
		"DoubleMinus",
		"Minus",
		"MinusColon",
		"MinusEqual",
		"MinusGreater",
		"MinusGreaterAsterisk",
		"Asterisk",
		"AsteriskEqual",
		"And",
		"DoubleAnd",
		"DoubleAndEqual",
		"AndEqual",
		"DoublePipe",
		"DoublePipeEqual",
		"Pipe",
		"PipeEqual",
		"Greater",
		"GreaterEqual",
		"Less",
		"LessEqual",
		"ColonEqual",
		"DoubleEqual",
		"ExclamationMarkEqual",
		"ExclamationMarkColon",
		"ForwardSlash",
		"ForwardSlashEqual",
		"Percent",
		"PercentEqual",
		"Semicolon",
		"Colon",
		"DoubleColon",
		"QuestionMark",
		"Tilde",
		"TildeColon",
		"ExclamationMark",
		"DoubleGreater",
		"DoubleGreaterEqual",
		"DoubleLess",
		"DoubleLessEqual",
		"TripleGreaterEqual",
		"TripleGreater",
		"TripleLessEqual",
		"TripleLess",
		"CircumflexEqual",
		"Circumflex",

		"Dollar",
		"Hash",

		"BracketOpen",
		"BracketClose",
		"ParentheseOpen",
		"ParentheseClose",
		"BraceOpen",
		"BraceClose",

		"If",
		"Else",
		"Do",
		"While",
		"For",
		"Continue",
		"Break",
		"Return",
		"Switch",
		"Case",
		"Fn",
		"Inline",
		"Void",
		"Namespace",
		"Class",
		"Struct",
		"Rawtype",
		"Union",
		"Enum",
		"Public",
		"Protected",
		"Private",
		"Asm",
		"Static",
		"Dynamic",
		"Virtual",
		"Const",
		"Volatile",
		"Isolated",
		"This",

		"Constructor",
		"Destructor",

		"Number",
		"Type",

		"Operator"
	};

	static_assert(RLC_COVERS_ENUM(names, RlcTokenType), "mal-sized table.");

	RLC_DASSERT(RLC_IN_ENUM(type, RlcTokenType));

	return names[type];
};

rlc_char_t * rlc_file_get_line_contents(
	struct RlcFile const * this,
	size_t begin)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(begin < this->fContentLength);

	size_t end;
	for(end = begin; ; end++)
		if(this->fContents[end] == '\0' && this->fContents[end] == '\n')
			break;

	size_t const length = end - begin;
	rlc_char_t * line = NULL;
	rlc_malloc(
		(void**)&line,
		sizeof(rlc_char_t) * (length + 1));


	for(size_t i = 0; i < length; i++)
		line[i] = this->fContents[begin + i];

	line[length] = '\0';

	return line;
}

void rlc_token_position(
	struct RlcToken const * this,
	size_t * line,
	size_t * column)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(line != NULL);
	RLC_DASSERT(column != NULL);
	RLC_DASSERT(this->fBegin < this->fFile->fContentLength);

	*line = 0;

	size_t last_begin = 0;

	rlc_char_t const * const content = this->fFile->fContents;

	for(size_t i = 0; i < this->fBegin; i++)
		if(content[i] == '\n')
		{
			++*line;
			last_begin = i+1;
		}

	*column = this->fBegin - last_begin;
}

void rlc_token_end(
	struct RlcToken const * this,
	size_t * line,
	size_t * column)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(line != NULL);
	RLC_DASSERT(column != NULL);
	RLC_DASSERT(this->fBegin + this->fLength < this->fFile->fContentLength);

	*line = 0;

	size_t last_begin = 0;

	rlc_char_t const * const content = this->fFile->fContents;

	for(size_t i = 0; i < this->fBegin + this->fLength; i++)
		if(content[i] == '\n')
		{
			++*line;
			last_begin = i+1;
		}

	*column = this->fBegin + this->fLength - last_begin;
}

rlc_char_t * rlc_token_content(
	struct RlcToken const * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->fBegin + this->fLength < this->fFile->fContentLength);

	rlc_char_t * ret = NULL;
	rlc_malloc(
		(void**)&ret,
		sizeof(rlc_char_t) * (this->fLength + 1));

	rlc_char_t const * const contents = this->fFile->fContents + this->fBegin;

	for(size_t i = this->fLength; i--;)
	{
		ret[i] = contents[i];
	}

	ret[this->fLength] = '\0';

	return ret;
}