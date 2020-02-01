#include "symbol.h"
#include "../assert.h"
#include "../malloc.h"
#include "../parser/member.h"

#include <string.h>

struct RlcScopedSymbolChild const kRlcScopedSymbolChildConstructor = {
	{ kRlcParsedSymbolChildTypeConstructor, NULL }
};

struct RlcScopedSymbolChild const kRlcScopedSymbolChildDestructor = {
	{ kRlcParsedSymbolChildTypeDestructor, NULL }
};

void rlc_scoped_symbol_child_create(
	struct RlcScopedSymbolChild * this,
	struct RlcSrcFile const * file,
	struct RlcParsedSymbolChild const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);

	switch(parsed->fType)
	{
	case kRlcParsedSymbolChildTypeConstructor:
		*this = kRlcScopedSymbolChildConstructor;
		break;
	case kRlcParsedSymbolChildTypeDestructor:
		*this = kRlcScopedSymbolChildDestructor;
		break;
	case kRlcParsedSymbolChildTypeIdentifier:
		rlc_scoped_identifier_create(&this->name, file, &parsed->fName);
		break;
	default:
		RLC_DASSERT(!"unhandled symbol child type");
	}
}

void rlc_scoped_symbol_child_destroy(
	struct RlcScopedSymbolChild * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_identifier_destroy(&this->name);
}

void rlc_scoped_symbol_create(
	struct RlcScopedSymbol * this,
	struct RlcSrcFile const * file,
	struct RlcParsedSymbol const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	this->fIsAbsolute = parsed->fIsRoot;
	this->fChildren = NULL;
	this->fChildCount = parsed->fChildCount;

	rlc_malloc(
		(void**)&this->fChildren,
		sizeof(struct RlcScopedSymbolChild) * this->fChildCount);

	for(RlcSrcIndex i = 0; i < this->fChildCount; i++)
		rlc_scoped_symbol_child_create(
			&this->fChildren[i],
			file,
			&parsed->fChildren[i]);
}

void rlc_scoped_symbol_destroy(
	struct RlcScopedSymbol * this)
{
	RLC_DASSERT(this != NULL);
	if(this->fChildCount)
	{
		for(RlcSrcIndex i = 0; i < this->fChildCount; i++)
			rlc_scoped_symbol_child_destroy(&this->fChildren[i]);
		rlc_free((void**)&this->fChildren);
		this->fChildCount = 0;
	}
}