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

		"'abstract'",
		"'break'",
		"'case'",
		"'catch'",
		"'constructor'",
		"'continue'",
		"'default'",
		"'destructor'",
		"'do'",
		"'else'",
		"'enum'",
		"'extern'",
		"'final'",
		"'finally'",
		"'for'",
		"'if'",
		"'include'",
		"'inline'",
		"'namespace'",
		"'noinit'",
		"'number'",
		"'operator'",
		"'override'",
		"'private'",
		"'protected'",
		"'public'",
		"'return'",
		"'sizeof'",
		"'static'",
		"'switch'",
		"'this'",
		"'throw'",
		"'try'",
		"'type'",
		"'union'",
		"'virtual'",
		"'void'",
		"'while'",

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