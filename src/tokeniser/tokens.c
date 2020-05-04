#include "tokens.h"

#include "../assert.h"
#include "../malloc.h"

char const * rlc_token_type_name(
	enum RlcTokenType type)
{
	static char const * const names[] = {
		"identifier",
		"number",
		"character",
		"string",

		"'ABSTRACT'",
		"'BREAK'",
		"'CASE'",
		"'CATCH'",
		"'CONSTRUCTOR'",
		"'CONTINUE'",
		"'DEFAULT'",
		"'DESTRUCTOR'",
		"'DO'",
		"'ELSE'",
		"'ENUM'",
		"'EXTERN'",
		"'FINAL'",
		"'FINALLY'",
		"'FOR'",
		"'IF'",
		"'INCLUDE'",
		"'INLINE'",
		"'NOINIT'",
		"'NUMBER'",
		"'OPERATOR'",
		"'OVERRIDE'",
		"'PRIVATE'",
		"'PROTECTED'",
		"'PUBLIC'",
		"'RETURN'",
		"'SIZEOF'",
		"'STATIC'",
		"'SWITCH'",
		"'THIS'",
		"'THROW'",
		"'TRY'",
		"'TYPE'",
		"'UNION'",
		"'VIRTUAL'",
		"'VOID'",
		"'WHILE'",

		"+=",
		"++",
		"+",

		"-=",
		"-:",
		"--",
		"->*",
		"->",
		"-",

		"*=",
		"*",

		"\\",

		"/=",
		"/",

		"%=",
		"%",

		"!=",
		"!:",
		"!",

		"^=",
		"^",

		"~:",
		"~",

		"&&=",
		"&&",
		"&=",
		"&",

		"||=",
		"||",
		"|=",
		"|",

		"?",

		"::=",
		":=",
		"::",
		":",
		"@@",
		"@",
		"...",
		"..!",
		"..?",
		".*",
		".",
		",",
		";",
		"==",

		"[",
		"]",
		"{",
		"}",
		"(",
		")",

		"<<<=",
		"<<<",
		"<<=",
		"<<",
		"<=",
		"<",

		">>>=",
		">>>",
		">>=",
		">>",
		">=",
		">",

		"$",
		"#"
	};

	static_assert(RLC_COVERS_ENUM(names, RlcTokenType), "mal-sized table.");

	RLC_DASSERT(RLC_IN_ENUM(type, RlcTokenType));

	return names[type];
}