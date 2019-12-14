#include "symbol.h"
#include "../assert.h"
#include "../malloc.h"
#include "../parser/member.h"

#include <string.h>

void rlc_scoped_symbol_child_create(
	struct RlcScopedSymbolChild * this,
	struct RlcSrcFile * file,
	struct RlcParsedScopeEntry * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);

	switch(RLC_DERIVING_TYPE(parsed))
	{
	case kRlcParsedConstructor:
		this->type = kRlcScopedSymbolChildTypeConstructor;
		this->name = NULL;
		break;
	case kRlcParsedDestructor:
		this->type = kRlcScopedSymbolChildTypeDestructor;
		this->name = NULL;
		break;
	default:
		RLC_DASSERT(parsed->fName.length != 0);
		this->type = kRlcScopedSymbolChildTypeIdentifier;
		this->name = rlc_src_string_cstr(&parsed->fName, file);
	}
}

void rlc_scoped_symbol_child_destroy(
	struct RlcScopedSymbolChild * this)
{
	RLC_DASSERT(this != NULL);

	if(this->name)
		rlc_free((void**)&this->name);
}

int rlc_scoped_symbol_child_compare(
	struct RlcScopedSymbolChild const * a,
	struct RlcScopedSymbolChild const * b)
{
	int diff = a->type - b->type;
	if(diff)
		return diff;
	else if(a->type == kRlcScopedSymbolChildTypeIdentifier)
	{
		return strcmp(a->name, b->name);
	} else
		return 0;
}