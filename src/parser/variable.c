#include "variable.h"

#include "../malloc.h"

#include <assert.h>

void rlc_variable_destroy(
	struct RlcVariable * this)
{
	assert(this != NULL);

	rlc_type_name_destroy(&this->fType);

	if(this->fInitExp)
	{
		rlc_expression_destroy(this->fInitExp);
		rlc_free((void**)&this->fInitExp);
	}
}