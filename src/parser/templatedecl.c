#include "templatedecl.h"

#include "../malloc.h"

#include <assert.h>

void rlc_template_decl_create(
	struct RlcTemplateDecl * this)
{
	assert(this != NULL);

	this->fChildren = NULL;
	this->fChildCount = 0;
}

void rlc_template_decl_add_child(
	struct RlcTemplateDecl * this,
	struct RlcTemplateDeclChild const * child)
{
	assert(this != NULL);
	assert(child != NULL);

	rlc_realloc(
		(void**)&this->fChildren,
		sizeof(struct RlcTemplateDecl) * ++ this->fChildCount);

	this->fChildren[this->fChildCount-1] = *child;
}

void rlc_template_decl_destroy(
	struct RlcTemplateDecl * this)
{
	assert(this != NULL);

	if(this->fChildren)
	{
		rlc_free((void**)&this->fChildren);
		this->fChildCount = 0;
	}
}