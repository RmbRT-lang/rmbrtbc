#include "templatedecl.h"
#include "../assert.h"
#include "../malloc.h"

static void rlc_scoped_template_decl_child_create(
	struct RlcScopedTemplateDeclChild * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTemplateDeclChild const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_identifier_create(
		&this->name,
		file,
		&parsed->fName);

	this->type = parsed->fType;

	if(this->type == kRlcParsedTemplateDeclTypeValue)
		rlc_scoped_type_name_create(
			&this->valueType,
			file,
			&parsed->fValueType);
}

static void rlc_scoped_template_decl_child_destroy(
	struct RlcScopedTemplateDeclChild * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_identifier_destroy(&this->name);

	if(this->type == kRlcParsedTemplateDeclTypeValue)
		rlc_scoped_type_name_destroy(&this->valueType);
}

void rlc_scoped_template_decl_create(
	struct RlcScopedTemplateDecl * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTemplateDecl const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	this->children = NULL;

	if((this->childCount = parsed->fChildCount))
	{
		rlc_malloc(
			(void**)&this->children,
			this->childCount * sizeof(struct RlcScopedTemplateDeclChild));

		for(RlcSrcIndex i = 0; i < this->childCount; i++)
			rlc_scoped_template_decl_child_create(
				&this->children[i],
				file,
				&parsed->fChildren[i]);
	}
}

void rlc_scoped_template_decl_destroy(
	struct RlcScopedTemplateDecl * this)
{
	RLC_DASSERT(this != NULL);

	if(this->childCount)
	{
		for(RlcSrcIndex i = 0; i < this->childCount; i++)
			rlc_scoped_template_decl_child_destroy(&this->children[i]);

		rlc_free((void**)&this->children);
	}
}