#include "symbol.h"
#include "expression.h"
#include "../assert.h"
#include "../malloc.h"
#include "../parser/member.h"

#include <string.h>

struct RlcScopedSymbolChild const kRlcScopedSymbolChildConstructor = {
	{ kRlcParsedSymbolChildTypeConstructor, NULL },
	NULL,
	0
};

struct RlcScopedSymbolChild const kRlcScopedSymbolChildDestructor = {
	{ kRlcParsedSymbolChildTypeDestructor, NULL },
	NULL,
	0
};

static void rlc_scoped_template_argument_create(
	struct RlcScopedTemplateArgument * this,
	struct RlcSrcFile const * file,
	struct RlcParsedSymbolChildTemplate const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	if((this->isExpression = parsed->fIsExpression))
		this->expression = rlc_scoped_expression_new(
			parsed->fExpression,
			file);
	else
		rlc_scoped_type_name_create(
			&this->type,
			file,
			parsed->fTypeName);
}

static void rlc_scoped_template_argument_destroy(
struct RlcScopedTemplateArgument * this)
{
	RLC_DASSERT(this != NULL);

	if(this->isExpression)
		rlc_scoped_expression_delete_virtual(this->expression);
	else
		rlc_scoped_type_name_destroy(&this->type);
}

void rlc_scoped_symbol_child_create(
	struct RlcScopedSymbolChild * this,
	struct RlcSrcFile const * file,
	struct RlcParsedSymbolChild const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);

	switch(parsed->fType)
	{
	default:
		RLC_DASSERT(!"unhandled symbol child type");
	case kRlcParsedSymbolChildTypeConstructor:
		*this = kRlcScopedSymbolChildConstructor;
		break;
	case kRlcParsedSymbolChildTypeDestructor:
		*this = kRlcScopedSymbolChildDestructor;
		break;
	case kRlcParsedSymbolChildTypeIdentifier:
		rlc_scoped_identifier_create(&this->name, file, &parsed->fName);
		break;
	}

	this->templates = NULL;
	if((this->templateCount = parsed->fTemplateCount))
	{
		rlc_malloc(
			(void**)&this->templates,
			this->templateCount * sizeof(struct RlcScopedTemplateArgument));

		for(RlcSrcIndex i = 0; i < this->templateCount; i++)
			rlc_scoped_template_argument_create(
				&this->templates[i],
				file,
				&parsed->fTemplates[i]);
	}
}

void rlc_scoped_symbol_child_destroy(
	struct RlcScopedSymbolChild * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_identifier_destroy(&this->name);

	for(RlcSrcIndex i = 0; i < this->templateCount; i++)
		rlc_scoped_template_argument_destroy(&this->templates[i]);

	if(this->templates)
		rlc_free((void**)&this->templates);
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