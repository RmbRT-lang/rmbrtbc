#include "typename.h"
#include "resolver.h"
#include "scopeitem.h"
#include "../scoper/typename.h"
#include "../scoper/functionsignature.h"
#include "../scoper/member.h"
#include "../parser/typename.h"
#include "../scoper/symbol.h"

#include "../malloc.h"

static void rlc_resolved_signature_create(
	struct RlcResolvedSignature * this,
	struct RlcScopedFunctionSignature const * name,
	struct RlcParsedFunctionSignature const * parsed,
	struct RlcScopedScope * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);
	RLC_DASSERT(scope != NULL);

	this->types = NULL;
	this->typeCount = parsed->fArgumentCount + 1;

	rlc_malloc(
		(void**)&this->types,
		this->typeCount * sizeof(struct RlcResolvedTypeName));

	rlc_resolved_type_name_create(
		&this->types[this->typeCount-1],
		&name->fResult,
		scope);

	for(RlcSrcIndex i = 0; i < parsed->fArgumentCount; i++)
	{
		rlc_resolved_type_name_create(
			&this->types[i],
			&name->fArguments[i],
			scope);
	}
}

static void rlc_resolved_signature_destroy(
	struct RlcResolvedSignature * this)
{
	for(RlcSrcIndex i = 0; i < this->typeCount; i++)
		rlc_resolved_type_name_destroy(&this->types[i]);
	rlc_free((void**)&this->types);
}

void rlc_resolved_type_name_create(
	struct RlcResolvedTypeName * this,
	struct RlcScopedTypeName const * name,
	struct RlcScopedScope * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);
	RLC_DASSERT(scope != NULL);

	this->scoped = name;

	switch(name->value)
	{
	default: RLC_DASSERT(!"unhandled type kind");
	case kRlcParsedTypeNameValueVoid:
		{
			this->kind = kRlcResolvedTypeKindVoid;
		} break;
	case kRlcParsedTypeNameValueName:
		{
			this->kind = kRlcResolvedTypeKindScopeItem;
			rlc_resolved_scope_item_create(
				&this->type.scopeItem,
				name->fName,
				scope);
		} break;
	case kRlcParsedTypeNameValueFunction:
		{
			this->kind = kRlcResolvedTypeKindSignature;
			rlc_resolved_signature_create(
				&this->type.signature,
				name->fFunction,
				name->fParsed->fFunction,
				scope);
		} break;
	case kRlcParsedTypeNameValueExpression:
		{
			RLC_DASSERT(!"not implemented");
		} break;
	}
}

void rlc_resolved_type_name_destroy(
	struct RlcResolvedTypeName * this)
{
	switch(this->kind)
	{
	default: RLC_DASSERT(!"unhandled type name kind");
	case kRlcResolvedTypeKindVoid: break;
	case kRlcResolvedTypeKindScopeItem:
		{
			rlc_resolved_scope_item_destroy(&this->type.scopeItem);
		} break;
	case kRlcResolvedTypeKindSignature:
		{
			rlc_resolved_signature_destroy(&this->type.signature);
		} break;
	}
}