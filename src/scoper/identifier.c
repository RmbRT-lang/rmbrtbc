#include "identifier.h"
#include "../src/string.h"
#include "../tokeniser/tokens.h"
#include "../assert.h"
#include "../malloc.h"
#include <string.h>

struct RlcScopedIdentifier const kRlcScopedIdentifierConstructor = {
	kRlcScopedIdentifierTypeConstructor, NULL
};
struct RlcScopedIdentifier const kRlcScopedIdentifierDestructor = {
	kRlcScopedIdentifierTypeDestructor, NULL
};

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

void rlc_scoped_identifier_from_token(
	struct RlcScopedIdentifier * this,
	struct RlcSrcFile const * file,
	struct RlcToken const * name)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(name != NULL);

	switch(name->type)
	{
	case kRlcTokConstructor:
		{
			*this = kRlcScopedIdentifierConstructor;
		} break;
	case kRlcTokDestructor:
		{
			*this = kRlcScopedIdentifierDestructor;
		} break;
	default:
		{
			RLC_DASSERT(name->type == kRlcTokIdentifier);

			rlc_scoped_identifier_create(this, file, &name->content);
		} break;
	}
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

char const * rlc_scoped_identifier_cstr(
	struct RlcScopedIdentifier const * this)
{
	RLC_DASSERT(this != NULL);

	switch(this->type)
	{
	case kRlcScopedIdentifierTypeConstructor:
		return "constructor";
	case kRlcScopedIdentifierTypeDestructor:
		return "destructor";
	default:
		return this->name;
	}
}