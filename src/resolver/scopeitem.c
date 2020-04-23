#include "scopeitem.h"
#include "resolver.h"
#include "templateargs.h"
#include "../scoper/scope.h"
#include "../scoper/scopeitem.h"
#include "../scoper/scopeitemgroup.h"
#include "../scoper/member.h"
#include "../scoper/scopeentry.h"

#include "../malloc.h"

struct FindNameContext
{
	/** The scope from which the name is referenced. */
	struct RlcScopedScope * origin;
	/** The scope item that was found. */
	struct RlcScopedScopeItem * found;
	struct RlcResolverFailContext const * failCtx;
};

static int find_type_name(
	struct RlcScopedScopeItem * item,
	struct FindNameContext * context)
{
	switch(RLC_DERIVING_TYPE(item))
	{
	default: RLC_DASSERT(!"unhandled enum value");
	case kRlcScopedMember:
		{
			struct RlcScopedMember * member = RLC_DERIVE_CAST(
				item,
				RlcScopedScopeItem,
				struct RlcScopedMember);

			switch(RLC_DERIVING_TYPE(member))
			{
			default:
				context->found = item;
				return 0;
			case kRlcScopedConstructor:
			case kRlcScopedDestructor:
			case kRlcScopedMemberFunction:
			case kRlcScopedMemberVariable:
				rlc_resolver_fail_ctx(
					context->failCtx,
					"%s is not a type entity",
					item->group->name);
			}
		} break;
	case kRlcScopedScopeEntry:
		{
			struct RlcScopedScopeEntry * member = RLC_DERIVE_CAST(
				item,
				RlcScopedScopeItem,
				struct RlcScopedScopeEntry);

			switch(RLC_DERIVING_TYPE(member))
			{
			default:
				context->found = item;
				return 0;
			case kRlcScopedFunction:
			case kRlcScopedVariable:
				rlc_resolver_fail_ctx(
					context->failCtx,
					"%s is not a type entity",
					item->group->name);
			}
		} break;
	}

	return 0;
}

void rlc_resolved_scope_item_create(
	struct RlcResolvedScopeItem * this,
	struct RlcScopedSymbol * symbol,
	struct RlcScopedScope * scope)
{
	struct FindNameContext context;
	context.origin = scope;

	if(symbol->fIsAbsolute)
		scope = rlc_scoped_scope_root(scope);

	this->templateCount = symbol->fChildCount;
	this->templates = NULL;
	rlc_malloc(
		(void**)&this->templates,
		this->templateCount * sizeof(struct RlcResolvedTemplateArgs));

	for(RlcSrcIndex i = 0; i < symbol->fChildCount; i++)
	{
		rlc_resolved_template_args_create(
			&this->templates[i],
			&symbol->fChildren[i],
			scope);
	}

	context.found = NULL;
	if(!rlc_scoped_scope_filter(
		scope,
		&symbol->fChildren[0],
		(rlc_scoped_scope_filter_fn_t)&find_type_name,
		&context,
		1,
		1))
	{
		rlc_resolver_fail(
			&symbol->parsed->fChildren[0].fName,
			scope->file,
			"failed to resolve symbol child '%s'",
			rlc_src_string_cstr(
				&symbol->parsed->fChildren[0].fName,
				scope->file));
	}

	for(RlcSrcIndex i = 1; i < symbol->fChildCount; i++)
	{
		if(!rlc_scoped_scope_filter(
			context.found->children,
			&symbol->fChildren[0],
			(rlc_scoped_scope_filter_fn_t)&find_type_name,
			&context,
			0,
			1))
		{
			rlc_resolver_fail(
				&symbol->parsed->fChildren[i].fName,
				scope->file,
				"failed to resolve symbol child '%s'",
				rlc_src_string_cstr(
					&symbol->parsed->fChildren[i].fName,
					scope->file));
		}
	}


}

void rlc_resolved_scope_item_destroy(
	struct RlcResolvedScopeItem * this)
{
	RLC_DASSERT(this != NULL);
	for(RlcSrcIndex i = 0; i < this->templateCount; i++)
		rlc_resolved_template_args_destroy(&this->templates[i]);
	rlc_free((void**)&this->templates);
}