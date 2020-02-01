#include "identifier.h"
#include "../src/string.h"
#include "../assert.h"
#include "../malloc.h"
#include <string.h>

void rlc_scoped_identifier_create(
	struct RlcScopedIdentifier * this,
	struct RlcSrcFile const * file,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(name != NULL);

	this->type = kRlcScopedIdentifierTypeIdentifier;
	this->name = rlc_src_string_cstr(name, file);
}

void rlc_scoped_identifier_destroy(
	struct RlcScopedIdentifier * this)
{
	RLC_DASSERT(this != NULL);

	if(this->name)
		rlc_free((void**)&this->name);
}

int rlc_scoped_identifier_compare(
	struct RlcScopedIdentifier const * a,
	struct RlcScopedIdentifier const * b)
{
	RLC_DASSERT(a != NULL);
	RLC_DASSERT(b != NULL);

	if(a->type != b->type)
		return a->type - b->type;

	if(a->type == kRlcScopedIdentifierTypeIdentifier)
		return strcmp(a->name, b->name);
	else
		return 0;
}