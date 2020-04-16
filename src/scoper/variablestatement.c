#include "variablestatement.h"
#include "scope.h"
#include "variable.h"
#include "../parser/variablestatement.h"

#include "../assert.h"

void rlc_scoped_variable_statement_create(
	struct RlcScopedVariableStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedVariableStatement const * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedVariableStatement,
		0,
		parent);

	struct RlcScopedScopeEntry * entry = rlc_scoped_scope_add_entry(
		parent,
		file,
		RLC_BASE_CAST(&parsed->fVariable, RlcParsedScopeEntry));

	this->variable = RLC_DERIVE_CAST(
		entry,
		RlcScopedScopeEntry,
		struct RlcScopedGlobalVariable);
}

void rlc_scoped_variable_statement_destroy(
	struct RlcScopedVariableStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));
}