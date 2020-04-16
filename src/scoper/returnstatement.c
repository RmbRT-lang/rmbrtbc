#include "returnstatement.h"
#include "expression.h"
#include "../parser/returnstatement.h"
#include "../assert.h"

void rlc_scoped_return_statement_create(
	struct RlcScopedReturnStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedReturnStatement const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedReturnStatement,
		0);

	if(parsed->fExpression)
		this->value = rlc_scoped_expression_new(parsed->fExpression, file);
	else
		this->value = NULL;
}

void rlc_scoped_return_statement_destroy(
	struct RlcScopedReturnStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->value)
		rlc_scoped_expression_delete_virtual(this->value);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));
}