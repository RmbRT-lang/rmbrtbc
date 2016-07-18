#include "symbol.h"

#include "../malloc.h"
#include <assert.h>


void rlc_symbol_child_add_template(
	struct RlcSymbolChild * this,
	enum RlcExpression * template_argument)
{
	assert(this != NULL);
	assert(template_argument != NULL);

	rlc_realloc(
		(void**)&this->fTemplates,
		sizeof(enum RlcExpression *) * ++ this->fTemplateCount);

	this->fTemplates[this->fTemplateCount-1] = template_argument;
}

void rlc_symbol_destroy(
	struct RlcSymbol * this)
{
	assert(this != NULL);

	if(this->fChildren)
		rlc_free((void**)&this->fChildren);

	this->fChildCount = 0;
	this->fIsRoot = 0;
}


void rlc_symbol_add_child(
	struct RlcSymbol * this,
	struct RlcSymbolChild const * child)
{
	assert(this != NULL);
	assert(child != NULL);

	rlc_realloc(
		(void**)&this->fChildren,
		sizeof (struct RlcSymbolChild) * ++ this->fChildCount);

	this->fChildren[this->fChildCount-1] = *child;
}