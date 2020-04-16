#include "throwstatement.h"
#include "expression.h"

void rlc_scoped_throw_statement_create(
	struct RlcScopedThrowStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedThrowStatement const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	this->type = parsed->fType;
	if(parsed->fValue)
		this->value = rlc_scoped_expression_new(parsed->fValue, file);
	else
		this->value = NULL;
}

void rlc_scoped_throw_statement_destroy(
	struct RlcScopedThrowStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->value)
		rlc_scoped_expression_delete_virtual(this->value);
}