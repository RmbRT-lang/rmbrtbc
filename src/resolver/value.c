#include "value.h"
#include "../assert.h"
#include "../macros.h"
#include "../scoper/symbolexpression.h"
#include "../scoper/numberexpression.h"
#include "../scoper/symbolchildexpression.h"
#include "../scoper/characterexpression.h"
#include "../scoper/stringexpression.h"
#include "../scoper/operatorexpression.h"
#include "../scoper/thisexpression.h"
#include "../scoper/castexpression.h"
#include "../scoper/sizeofexpression.h"

static char const * kTypeNames[] = {
	"boolean",
	"number",
	"string",
	"character"
};

static_assert(
	RLC_COVERS_ENUM(kTypeNames, RlcValueType),
	"invalid table size");

int rlc_value_cmp(
	struct RlcValue const * lhs,
	struct RlcValue const * rhs,
	struct RlcResolverFailContext const * context)
{
	RLC_DASSERT(lhs != NULL);
	RLC_DASSERT(rhs != NULL);
	RLC_DASSERT(context != NULL);

	if(lhs->type != rhs->type)
		rlc_resolver_fail_ctx(context,
			"Cannot compare %s and %s",
			kTypeNames[lhs->type],
			kTypeNames[rhs->type]);

	return rlc_value_order(lhs, rhs);
}

int rlc_value_order(
	struct RlcValue const * lhs,
	struct RlcValue const * rhs)
{
	RLC_DASSERT(lhs != NULL);
	RLC_DASSERT(rhs != NULL);

	if(lhs->type != rhs->type)
		return lhs->type - rhs->type;

	switch(lhs->type)
	{
	default:
		RLC_DASSERT(!"unhandled value type.");
	case kRlcValueTypeBoolean:
		return lhs->boolean - rhs->boolean;
	case kRlcValueTypeNumber:
		return rlc_number_cmp(&lhs->number, &rhs->number);
	case kRlcValueTypeString:
		return rlc_scoped_text_cmp(&lhs->string, &rhs->string);
	case kRlcValueTypeCharacter:
		return rlc_scoped_text_cmp(&lhs->character, &rhs->character);
	}
}

void rlc_value_destroy(
	struct RlcValue * this)
{
	switch(this->type)
	{
	default:
		RLC_DASSERT(!"unhandled value type.");
	case kRlcValueTypeBoolean:
	case kRlcValueTypeNumber:
		break;
	case kRlcValueTypeString:
	case kRlcValueTypeCharacter:
		rlc_scoped_text_destroy(&this->string);
	}
}


static int always(
	void const * this,
	struct RlcScopedScope const * scope)
{
	(void) this;
	(void) scope;

	return 1;
}
static int never(
	void const * this,
	struct RlcScopedScope const * scope)
{
	(void) this;
	(void) scope;

	return 0;
}

static int rlc_scoped_operator_expression_is_constant(
	struct RlcScopedOperatorExpression const * this,
	struct RlcScopedScope const * scope)
{
	RLC_DASSERT(this != NULL);

	RlcSrcSize argCount = rlc_scoped_operator_expression_argument_count(this);

	for(RlcSrcSize i = 0; i < argCount; i++)
	{
		if(!rlc_scoped_expression_is_constant(
			this->arguments[i],
			scope))
			return 0;
	}

	return 1;
}


int rlc_scoped_expression_is_constant(
	struct RlcScopedExpression const * this,
	struct RlcScopedScope const * scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(scope != NULL);

	typedef int (*is_constant_fn_t)(
		void const *,
		struct RlcScopedScope const *);

#define ENTRY(type, isConstantFn) { \
		(is_constant_fn_t) &isConstantFn, \
		RLC_DERIVE_OFFSET(RlcScopedExpression, struct RlcScoped##type), \
		kRlcScoped##type \
	}
#define NOENTRY(type) { NULL }
	static struct {
		is_constant_fn_t is_constant;
		intptr_t offset;
		enum RlcScopedExpressionType type;
	} const k_vtable[] = {
		ENTRY(SymbolExpression, never),
		ENTRY(SymbolChildExpression, never),
		ENTRY(NumberExpression, always),
		ENTRY(CharacterExpression, always),
		ENTRY(StringExpression, always),
		ENTRY(OperatorExpression, rlc_scoped_operator_expression_is_constant),
		ENTRY(ThisExpression, never),
		ENTRY(CastExpression, never),
		ENTRY(SizeofExpression, never)
	};
#undef ENTRY
#undef NOENTRY

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcScopedExpressionType),
		"Ill-sized vtable.");

	enum RlcParsedExpressionType type = RLC_DERIVING_TYPE(this);

	RLC_DASSERT(k_vtable[type].type == type);

	return k_vtable[type].is_constant(
		(char *)this + k_vtable[type].offset,
		scope);

}