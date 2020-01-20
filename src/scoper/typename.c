#include "typename.h"
#include "../macros.h"

void rlc_scoped_type_name_create(
	struct RlcScopedTypeName * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTypeName const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	RLC_ASSERT(!"not implemented yet");
}

void rlc_scoped_type_name_destroy(
	struct RlcScopedTypeName * this)
{
	RLC_DASSERT(this != NULL);

	RLC_ASSERT(!"not implemented yet");
}