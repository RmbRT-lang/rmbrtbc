#include "templateargs.h"
#include "../scoper/symbol.h"
#include "../malloc.h"

void rlc_resolved_template_args_create(
	struct RlcResolvedTemplateArgs * this,
	struct RlcScopedSymbolChild const * symbol,
	struct RlcScopedScope * scope)
{
	RLC_DASSERT(this != NULL);

	this->args = NULL;
	if((this->argCount = symbol->templateCount))
	{
		rlc_malloc(
			(void**)&this->args,
			this->argCount * sizeof(struct RlcResolvedTemplateArg));

		for(RlcSrcIndex i = 0; i < this->argCount; i++)
		{
			rlc_resolved_template_arg_resolve(
				&this->args[i],
				&symbol->templates[i],
				scope);
		}
	}
}

void rlc_resolved_template_args_destroy(
	struct RlcResolvedTemplateArgs * this)
{
	RLC_DASSERT(this != NULL);

	for(RlcSrcIndex i = 0; i < this->argCount; i++)
		if(this->args[i].isExpression)
			rlc_value_destroy(&this->args[i].value);
		else
			rlc_resolved_type_name_destroy(&this->args[i].type);
	rlc_free((void**)&this->args);
}

void rlc_resolved_template_arg_resolve(
	struct RlcResolvedTemplateArg * this,
	struct RlcScopedTemplateArgument const * scoped,
	struct RlcScopedScope * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(scoped != NULL);
	RLC_DASSERT(scope != NULL);

	if(scoped->isExpression)
	{
		RLC_ASSERT(!"not implemented");
	} else
	{
		rlc_resolved_type_name_create(
			&this->type,
			&scoped->type,
			scope);
	}
}