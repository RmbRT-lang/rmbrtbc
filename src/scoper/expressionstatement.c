#include "expressionstatement.h"
#include "expression.h"
#include "../parser/expressionstatement.h"
#include "../assert.h"

#define kRlcScopedExpressionStatement kRlcParsedExpressionStatement

void rlc_scoped_expression_statement_create(
	struct RlcScopedExpressionStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedExpressionStatement const * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedExpressionStatement,
		0,
		parent);

	this->expression = rlc_scoped_expression_new(
		parsed->fExpression,
		file);
}

void rlc_scoped_expression_statement_destroy(
	struct RlcScopedExpressionStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));

	rlc_scoped_expression_delete_virtual(this->expression);
}