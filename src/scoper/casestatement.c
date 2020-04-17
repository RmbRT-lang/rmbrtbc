#include "casestatement.h"
#include "expression.h"
#include "../parser/casestatement.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_scoped_case_statement_create(
	struct RlcScopedCaseStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedCaseStatement const * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedCaseStatement,
		0,
		parent);

	rlc_scoped_control_label_create(
		&this->controlLabel,
		file,
		&parsed->fControlLabel);

	this->values = NULL;
	if((this->valueCount = parsed->fValues.fCount))
	{
		rlc_malloc(
			(void**)&this->values,
			this->valueCount * sizeof(struct RlcScopedExpression *));

		for(RlcSrcIndex i = 0; i < this->valueCount; i++)
			this->values[i] = rlc_scoped_expression_new(
				parsed->fValues.fValues[i],
				file);
	}

	this->isDefault = parsed->fIsDefault;

	this->body = rlc_scoped_statement_new(
		file,
		parsed->fBody,
		parent);
}

void rlc_scoped_case_statement_destroy(
	struct RlcScopedCaseStatement * this)
{
	RLC_DASSERT(this != NULL);


	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));

	rlc_scoped_control_label_destroy(&this->controlLabel);

	if(this->valueCount)
	{
		for(RlcSrcIndex i = 0; i < this->valueCount; i++)
			rlc_scoped_expression_delete_virtual(this->values[i]);
		rlc_free((void**)&this->values);
	}

	rlc_scoped_statement_delete(this->body);
}