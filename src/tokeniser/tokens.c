#include "tokens.h"
#include "../assert.h"


char const * rlc_tok_result_message(
	enum RlcTokResult result)
{
	static char const * messages[] = {
		"tokenising successful",
		"end of source file reached",
		"invalid hexadecimal digit",
		"invalid octal digit.",
		"invalid decimal digit.",
		"invalid floating type specifier",
		"unclosed caracter/string literal.",
		"invalid hexadecimal escape sequence.",
		"invalid octal escape sequence.",
		"invalid decimal escape sequence.",
		"invalid escape sequence.",
		"invalid UTF-8 byte.",
		"empty character literal.",
		"unexpected symbol."
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
		"TripleDot",
		"Plus",
		"PlusEqual",
		"DoublePlus",
		"DoubleMinus",
		"Minus",
		"MinusColon",
		"MinusEqual",
		"MinusGreater",
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

		"BracketOpen",
		"BracketClose",
		"ParentheseOpen",
		"ParentheseClose",
		"BraceOpen",
		"BraceClose",

		"If",
		"Else",
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
		"Typedef",
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
		"This"
	};

	static_assert(RLC_COVERS_ENUM(names, RlcTokenType), "mal-sized table.");

	RLC_DASSERT(RLC_IN_ENUM(type, RlcTokenType));

	return names[type];
};