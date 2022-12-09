#include "string.h"
#include "file.h"
#include "../assert.h"
#include "../malloc.h"

#include <string.h>

struct RlcSrcString const kRlcSrcStringEmpty = { 0, 0, 1, 1 };

int rlc_src_string_valid(
	struct RlcSrcString const * this,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	return this->start + this->length <= file->fContentLength;
}

int rlc_src_string_cmp(
	struct RlcSrcFile const * file,
	struct RlcSrcString const * a,
	struct RlcSrcString const * b)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(a != NULL);
	RLC_DASSERT(b != NULL);

	RLC_DASSERT(rlc_src_string_valid(a, file));
	RLC_DASSERT(rlc_src_string_valid(b, file));

	if(a->length < b->length)
		return -1;
	else if(a->length > b->length)
		return 1;
	else
		return strncmp(
			&file->fContents[a->start],
			&file->fContents[b->start],
			a->length);
}

int rlc_src_string_cmp_cstr(
	struct RlcSrcFile const * file,
	struct RlcSrcString const * a,
	char const * cstr)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(a != NULL);
	RLC_DASSERT(cstr != NULL);

	RLC_DASSERT(rlc_src_string_valid(a, file));

	size_t cstr_len = strlen(cstr);

	if(a->length < cstr_len)
		return -1;
	else if(a->length > cstr_len)
		return 1;
	else
		return strncmp(
			&file->fContents[a->start],
			cstr,
			cstr_len);
}

char const * rlc_src_string_cstr(
	struct RlcSrcString const * this,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	RLC_DASSERT(rlc_src_string_valid(this, file));

	char * ret = NULL;
	rlc_malloc((void**)&ret, this->length + 1);

	memcpy(ret, &file->fContents[this->start], this->length);
	ret[this->length] = '\0';

	return ret;
}

void rlc_src_string_print(
	struct RlcSrcString const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(out != NULL);

	static char const * s_special_identifiers[] = {
		"char", "int", "short", "double", "long", "float", "unsigned", "bool",
		"auto",
		"class",
		"concept",
		"enum",
		"extern",
		"inline",
		"static",
		"operator",
		"final",
		"override",
		"virtual",
		"public",
		"private",
		"protected",
		"sizeof",
		"const",
		"delete",
		"new",
		"struct",
		"template",
		"typedef",
		"typename",
		"namespace",
		"using"
		"volatile",
		"register",
		"std", // avoid having c++ std stuff appear in rl std.
		"nullptr",
		"nullptr_t",
		"main",
		"true",
		"false",
		"if",
		"else",
		"while",
		"do",
		"for",
		"throw",
		"catch",
		"switch",
		"break",
		"continue",
		"case",
		"default",
		"return",
		"this",
		"try",
		"union",
		"void",
		"return",
		"and",
		"bitand",
		"bitor",
		"compl",
		"not",
		"or",
		"xor",
		"NULL"
	};

	for(size_t i = 0; i < _countof(s_special_identifiers); i++)
		if(0 == rlc_src_string_cmp_cstr(file, this, s_special_identifiers[i]))
		{
			fputs("__rlc_", out);
			break;
		}

	fprintf(out, "%.*s", (int)this->length, &file->fContents[this->start]);
}

void rlc_src_string_print_noreplace(
	struct RlcSrcString const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fprintf(out, "%.*s", (int)this->length, &file->fContents[this->start]);
}