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

		"'break'",
		"'case'",
		"'constructor'",
		"'continue'",
		"'default'",
		"'destructor'",
		"'do'",
		"'else'",
		"'enum'",
		"'extern'",
		"'for'",
		"'if'",
		"'include'",
		"'inline'",
		"'isolated'",
		"'namespace'",
		"'noinit'",
		"'number'",
		"'operator'",
		"'private'",
		"'protected'",
		"'public'",
		"'return'",
		"'sizeof'",
		"'static'",
		"'switch'",
		"'this'",
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