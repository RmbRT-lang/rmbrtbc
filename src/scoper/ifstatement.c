#include "ifstatement.h"
#include "expression.h"
#include "variable.h"
#include "scope.h"
#include "../parser/ifstatement.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedIfStatement kRlcParsedIfStatement

void rlc_scoped_if_statement_create(
	struct RlcScopedIfStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedIfStatement const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedIfStatement,
		1);

	this->condition.isVariable = parsed->fCondition.fIsVariable;
	if(this->condition.isVariable)
	{
		struct RlcScopedScopeEntry * variable = rlc_scoped_scope_add_entry(
			RLC_BASE_CAST(this, RlcScopedStatement)->scope,
			file,
			RLC_BASE_CAST(&parsed->fCondition.fVariable, RlcParsedScopeEntry));

		this->condition.variable = RLC_DERIVE_CAST(
			variable,
			RlcScopedScopeEntry,
			struct RlcScopedVariable);
	} else
	{
		this->condition.expression = rlc_scoped_expression_new(
			parsed->fCondition.fExpression,
			file);
	}

	this->label = NULL;
	if(parsed->fIfLabel.fExists)
	{
		rlc_malloc((void**)&this->label, sizeof(struct RlcScopedIdentifier));
		rlc_scoped_identifier_create(this->label, file, &parsed->fIfLabel.fLabel.content);
	}

	this->ifBody = rlc_scoped_statement_new(file, parsed->fIf);
	this->elseBody = parsed->fElse
		? rlc_scoped_statement_new(file, parsed->fElse)
		: NULL;
}

void rlc_scoped_if_statement_destroy(
	struct RlcScopedIfStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));

	if(!this->condition.isVariable)
		rlc_scoped_expression_delete_virtual(this->condition.expression);

	if(this->label)
	{
		rlc_scoped_identifier_destroy(this->label);
		rlc_free((void**)&this->label);
	}

	rlc_scoped_statement_delete(this->ifBody);
	if(this->elseBody)
		rlc_scoped_statement_delete(this->elseBody);
}