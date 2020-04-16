#include "breakstatement.h"
#include "../parser/breakstatement.h"
#include "../macros.h"

#define kRlcScopedBreakStatement kRlcParsedBreakStatement

void rlc_scoped_break_statement_create(
	struct RlcScopedBreakStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedBreakStatement const * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedBreakStatement,
		0,
		parent);

	rlc_scoped_control_label_create(&this->label, file, &parsed->fLabel);
}

void rlc_scoped_break_statement_destroy(
	struct RlcScopedBreakStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_control_label_destroy(&this->label);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));
}