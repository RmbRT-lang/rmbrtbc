#include "blockstatement.h"
#include "../parser/blockstatement.h"
#include "../assert.h"
#include "../malloc.h"

#define kRlcScopedBlockStatement kRlcParsedBlockStatement

void rlc_scoped_block_statement_create(
	struct RlcScopedBlockStatement * this,
	struct RlcSrcFile const * file,
	struct RlcParsedBlockStatement const * parsed,
	struct RlcScopedScope * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	rlc_scoped_statement_create(
		RLC_BASE_CAST(this, RlcScopedStatement),
		RLC_BASE_CAST(parsed, RlcParsedStatement),
		kRlcScopedBlockStatement,
		1,
		parent);

	this->statementCount = parsed->fList.fStatementCount;
	this->statements = NULL;

	if(!this->statementCount)
		return;

	rlc_malloc(
		(void**)&this->statements,
		this->statementCount * sizeof(struct RlcScopedtatement *));

	for(RlcSrcIndex i = 0; i < this->statementCount; i++)
		this->statements[i] = rlc_scoped_statement_new(
			file,
			parsed->fList.fStatements[i],
			RLC_BASE_CAST(this, RlcScopedStatement)->scope);
}

void rlc_scoped_block_statement_destroy(
	struct RlcScopedBlockStatement * this)
{
	RLC_DASSERT(this != NULL);

	while(this->statementCount)
		rlc_scoped_statement_delete(this->statements[--this->statementCount]);

	if(this->statements)
		rlc_free((void**)&this->statements);

	rlc_scoped_statement_destroy_base(
		RLC_BASE_CAST(this, RlcScopedStatement));
}