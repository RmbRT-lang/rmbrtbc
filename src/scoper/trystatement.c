#include "trystatement.h"
#include "../parser/trystatement.h"

#include "variable.h"
#include "scope.h"

#include "../malloc.h"
#include "../assert.h"

static void rlc_scoped_catch_statement_create(
	struct RlcScopedCatchStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedCatchStatement const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedCatchStatement,
		1);

	this->exception = NULL;
	if(!(this->isVoid = parsed->fIsVoid))
	{
		struct RlcScopedScopeEntry * entry = rlc_scoped_scope_add_entry(
			RLC_BASE_CAST(this, RlcScopedStatement)->scope,
			file,
			RLC_BASE_CAST(&parsed->fException, RlcParsedScopeEntry));

		this->exception = RLC_DERIVE_CAST(
			entry,
			RlcScopedScopeEntry,
			struct RlcScopedGlobalVariable);
	}

	this->body = rlc_scoped_statement_new(file, parsed->fBody);
}

static void rlc_scoped_catch_statement_destroy(
	struct RlcScopedCatchStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));
	rlc_scoped_statement_delete(this->body);
}

void rlc_scoped_try_statement_create(
	struct RlcScopedTryStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTryStatement const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedTryStatement,
		0);

	this->body = rlc_scoped_statement_new(
		file,
		RLC_BASE_CAST(parsed, RlcParsedStatement));

	this->catches = NULL;
	if((this->catchCount = parsed->fCatchCount))
	{
		rlc_malloc(
			(void**)&this->catches,
			this->catchCount * sizeof(struct RlcScopedCatchStatement));

		for(RlcSrcIndex i = 0; i < this->catchCount; i++)
			rlc_scoped_catch_statement_create(
				&this->catches[i],
				file,
				&parsed->fCatches[i]);
	}
}

void rlc_scoped_try_statement_destroy(
	struct RlcScopedTryStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));

	if(this->catchCount)
	{
		for(RlcSrcIndex i = 0; i < this->catchCount; i++)
			rlc_scoped_catch_statement_destroy(&this->catches[i]);
		rlc_free((void**)&this->catches);
	}
}