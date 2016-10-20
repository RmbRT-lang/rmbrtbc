#include "symbol.h"

#include "../malloc.h"
#include "../assert.h"


void rlc_symbol_child_add_template(
	struct RlcParsedSymbolChild * this,
	struct RlcParsedExpression * template_argument)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(template_argument != NULL);

	rlc_realloc(
		(void**)&this->fTemplates,
		sizeof(struct RlcParsedExpression *) * ++ this->fTemplateCount);

	this->fTemplates[this->fTemplateCount-1] = template_argument;
}

void rlc_symbol_destroy(
	struct RlcParsedSymbol * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fChildren)
		rlc_free((void**)&this->fChildren);

	this->fChildCount = 0;
	this->fIsRoot = 0;
}


void rlc_symbol_add_child(
	struct RlcParsedSymbol * this,
	struct RlcParsedSymbolChild * child)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(child != NULL);

	rlc_realloc(
		(void**)&this->fChildren,
		sizeof (struct RlcParsedSymbolChild) * ++ this->fChildCount);

	this->fChildren[this->fChildCount-1] = *child;
#ifdef RLC_DEBUG
	// invalidate the old data.
	rlc_parsed_symbol_child_create(child);
#endif
}

void rlc_parsed_symbol_create(
	struct RlcParsedSymbol * this)
{
	RLC_DASSERT(this != NULL);

	this->fChildren = NULL;
	this->fChildCount = 0;
	this->fIsRoot = 0;
}