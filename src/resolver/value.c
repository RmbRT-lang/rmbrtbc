#include "value.h"
#include "../assert.h"
#include "../macros.h"

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

	switch(lhs->type)
	{
	default:
		RLC_ASSERT(!"unhandled value type.");
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