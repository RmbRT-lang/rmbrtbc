#include "loopstatement.h"
#include "identifier.h"
#include "expression.h"
#include "../parser/loopstatement.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedLoopStatement kRlcParsedLoopStatement

void rlc_scoped_loop_statement_create(
	struct RlcScopedLoopStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedLoopStatement * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedLoopStatement,
		1,
		parent);

	rlc_scoped_control_label_create(&this->label, file, &parsed->fLabel);

	this->isPostCondition = parsed->fIsPostCondition;

	if(parsed->fIsVariableInitial)
		rlc_scoped_maybe_exp_or_var_create_variable(
			&this->initial,
			file,
			&parsed->fInitial.fVariable,
			RLC_BASE_CAST(this, RlcScopedStatement)->scope);
	else if(parsed->fInitial.fExpression != NULL)
		rlc_scoped_maybe_exp_or_var_create_expression(
			&this->initial,
			file,
			parsed->fInitial.fExpression);
	else
		rlc_scoped_maybe_exp_or_var_create_empty(&this->initial);

	if(parsed->fIsVariableCondition)
		rlc_scoped_maybe_exp_or_var_create_variable(
			&this->condition,
			file,
			&parsed->fCondition.fVariable,
			RLC_BASE_CAST(this, RlcScopedStatement)->scope);
	else if(parsed->fCondition.fExpression != NULL)
		rlc_scoped_maybe_exp_or_var_create_expression(
			&this->condition,
			file,
			parsed->fCondition.fExpression);
	else
		rlc_scoped_maybe_exp_or_var_create_empty(&this->condition);

	this->postLoop = parsed->fPostLoop
		? rlc_scoped_expression_new(parsed->fPostLoop, file)
		: NULL;

	this->body = rlc_scoped_statement_new(
		file,
		parsed->fBody,
		RLC_BASE_CAST(this, RlcScopedStatement)->scope);
}

void rlc_scoped_loop_statement_destroy(
	struct RlcScopedLoopStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_control_label_destroy(&this->label);

	rlc_scoped_maybe_exp_or_var_destroy(&this->initial);
	rlc_scoped_maybe_exp_or_var_destroy(&this->condition);
	if(this->postLoop)
		rlc_scoped_expression_delete_virtual(this->postLoop);
	rlc_scoped_statement_delete(this->body);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));
}