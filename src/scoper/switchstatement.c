#include "switchstatement.h"
#include "expression.h"
#include "casestatement.h"
#include "../parser/switchstatement.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_scoped_switch_statement_create(
	struct RlcScopedSwitchStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedSwitchStatement const * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedSwitchStatement,
		parsed->fIsVariableSwitchValue,
		parent);

	rlc_scoped_control_label_create(
		&this->label,
		file,
		&parsed->fLabel);

	if(parsed->fIsVariableSwitchValue)
		rlc_scoped_maybe_exp_or_var_create_variable(
			&this->switchValue,
			file,
			&parsed->fSwitchValue.fVariable,
			RLC_BASE_CAST(this, RlcScopedStatement)->scope);
	else
		rlc_scoped_maybe_exp_or_var_create_expression(
			&this->switchValue,
			file,
			parsed->fSwitchValue.fExpression);

	this->cases = NULL;
	if((this->caseCount = parsed->fCaseCount))
	{
		rlc_malloc(
			(void**)&this->cases,
			this->caseCount * sizeof(struct RlcScopedCaseStatement));

		for(RlcSrcIndex i = 0; i < this->caseCount; i++)
		{
			rlc_scoped_case_statement_create(
				&this->cases[i],
				file,
				&parsed->fCases[i],
				parent);
		}
	}
}

void rlc_scoped_switch_statement_destroy(
	struct RlcScopedSwitchStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_control_label_destroy(&this->label);

	rlc_scoped_maybe_exp_or_var_destroy(&this->switchValue);

	if(this->caseCount)
	{
		for(RlcSrcIndex i = 0; i < this->caseCount; i++)
			rlc_scoped_case_statement_destroy(&this->cases[i]);

		rlc_free((void**)&this->cases);
		this->caseCount = 0;
	}

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));
}