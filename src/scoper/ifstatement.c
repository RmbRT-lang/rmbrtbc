#include "ifstatement.h"
#include "expression.h"
#include "variable.h"
#include "scope.h"
#include "identifier.h"
#include "../parser/ifstatement.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedIfStatement kRlcParsedIfStatement

void rlc_scoped_if_statement_create(
	struct RlcScopedIfStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedIfStatement const * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedIfStatement,
		1,
		parent);

	if(!parsed->fCondition.fIsVariable)
	{
		rlc_scoped_maybe_exp_or_var_create_expression(
			&this->condition,
			file,
			parsed->fCondition.fExpression);
	} else
	{
		rlc_scoped_maybe_exp_or_var_create_variable(
			&this->condition,
			file,
			&parsed->fCondition.fVariable,
			RLC_BASE_CAST(this, RlcScopedStatement)->scope);
	}

	rlc_scoped_control_label_create(&this->label, file, &parsed->fIfLabel);

	struct RlcScopedScope * scope = RLC_BASE_CAST(this, RlcScopedStatement)->scope;
	this->ifBody = rlc_scoped_statement_new(file, parsed->fIf, scope);
	this->elseBody = parsed->fElse
		? rlc_scoped_statement_new(file, parsed->fElse, scope)
		: NULL;
}

void rlc_scoped_if_statement_destroy(
	struct RlcScopedIfStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));

	rlc_scoped_maybe_exp_or_var_destroy(&this->condition);

	rlc_scoped_control_label_destroy(&this->label);

	rlc_scoped_statement_delete(this->ifBody);
	if(this->elseBody)
		rlc_scoped_statement_delete(this->elseBody);
}