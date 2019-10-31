#include "statement.h"

#include "expressionstatement.h"
#include "blockstatement.h"
#include "ifstatement.h"
#include "loopstatement.h"
#include "variablestatement.h"
#include "returnstatement.h"
#include "switchstatement.h"
#include "breakstatement.h"

#include "../assert.h"
#include "../malloc.h"


#include <stdint.h>
#include <string.h>

void rlc_parsed_statement_create(
	struct RlcParsedStatement * this,
	enum RlcParsedStatementType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DERIVING_TYPE(this) = type;
}

void rlc_parsed_statement_destroy_base(
	struct RlcParsedStatement * this)
{
	RLC_DASSERT(this != NULL);
	;
}

void rlc_parsed_statement_destroy_virtual(
	struct RlcParsedStatement * this)
{
	RLC_DASSERT(this != NULL);

	typedef void (*thiscall_t) (
		void * this);

	static struct Destructor {
		thiscall_t fAddress;
		ptrdiff_t fOffset;
	} const k_vtable[] = {
		{
			(thiscall_t)&rlc_parsed_expression_statement_destroy,
			RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsedExpressionStatement)
		}, {
			(thiscall_t)&rlc_parsed_block_statement_destroy,
			RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsedBlockStatement)
		}, {
			(thiscall_t)&rlc_parsed_if_statement_destroy,
			RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsedIfStatement)
		}, {
			(thiscall_t)&rlc_parsed_loop_statement_destroy,
			RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsedLoopStatement)
		}, {
			(thiscall_t)&rlc_parsed_variable_statement_destroy,
			RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsedVariableStatement)
		}, {
			(thiscall_t)&rlc_parsed_return_statement_destroy,
			RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsedReturnStatement)
		}, {
			(thiscall_t)&rlc_parsed_switch_statement_destroy,
			RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsedSwitchStatement)
		}, {
			(thiscall_t)&rlc_parsed_case_statement_destroy,
			RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsedCaseStatement)
		}, {
			(thiscall_t)&rlc_parsed_break_statement_destroy,
			RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsedBreakStatement)
		}
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedStatementType), "ill-sized vtable.");

	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcParsedStatementType));

	k_vtable[RLC_DERIVING_TYPE(this)].fAddress(
		(void*)((uintptr_t)this + k_vtable[RLC_DERIVING_TYPE(this)].fOffset));
}

union RlcStatementStorage
{
	struct RlcParsedExpressionStatement fRlcParsedExpressionStatement;
	struct RlcParsedBlockStatement fRlcParsedBlockStatement;
	struct RlcParsedIfStatement fRlcParsedIfStatement;
	struct RlcParsedLoopStatement fRlcParsedLoopStatement;
	struct RlcParsedVariableStatement fRlcParsedVariableStatement;
	struct RlcParsedReturnStatement fRlcParsedReturnStatement;
	struct RlcParsedSwitchStatement fRlcParsedSwitchStatement;
	struct RlcParsedCaseStatement fRlcParsedCaseStatement;
	struct RlcParsedBreakStatement fRlcParsedBreakStatement;
};

struct RlcParsedStatement * rlc_parsed_statement_parse(
	struct RlcParser * parser,
	int flags)
{
	RLC_DASSERT(parser != NULL);

	union RlcStatementStorage storage;

	typedef int (*parse_fn_t)(
		union RlcStatementStorage *,
		struct RlcParser *);


#define ENTRY(Type, parse) { \
		k ## Type,\
		(parse_fn_t)parse, \
		sizeof(struct Type), \
		RLC_DERIVE_OFFSET(RlcParsedStatement, struct Type) }

	static struct {
		enum RlcParsedStatementType fType;
		parse_fn_t fParseFn;
		size_t fTypeSize;
		size_t fOffset;
	} const k_parse_lookup[] = {
		ENTRY(RlcParsedBlockStatement, &rlc_parsed_block_statement_parse),
		ENTRY(RlcParsedReturnStatement, &rlc_parsed_return_statement_parse),
		ENTRY(RlcParsedIfStatement, &rlc_parsed_if_statement_parse),
		ENTRY(RlcParsedLoopStatement, &rlc_parsed_loop_statement_parse),
		ENTRY(RlcParsedVariableStatement, &rlc_parsed_variable_statement_parse),
		ENTRY(RlcParsedSwitchStatement, &rlc_parsed_switch_statement_parse),
		ENTRY(RlcParsedCaseStatement, &rlc_parsed_case_statement_parse),
		ENTRY(RlcParsedBreakStatement, &rlc_parsed_break_statement_parse),
		// expression has to come after variable.
		ENTRY(RlcParsedExpressionStatement, &rlc_parsed_expression_statement_parse),
	};

	static_assert(RLC_COVERS_ENUM(k_parse_lookup, RlcParsedStatementType), "ill-sized parse table.");

	struct RlcParsedStatement * ret;

	for(size_t i = 0; i < _countof(k_parse_lookup); i++)
	{
		if(RLC_FLAG(k_parse_lookup[i].fType) & flags)
		{
			if(k_parse_lookup[i].fParseFn(
				&storage,
				parser))
			{
				void * temp = NULL;
				rlc_malloc(&temp, k_parse_lookup[i].fTypeSize);

				memcpy(temp, &storage, k_parse_lookup[i].fTypeSize);

				ret = (void*) ((uint8_t*)temp + k_parse_lookup[i].fOffset);

				return ret;
			}
		}
	}

	return NULL;
}

void rlc_parsed_statement_list_create(
	struct RlcParsedStatementList * this)
{
	RLC_DASSERT(this != NULL);

	this->fStatements = NULL;
	this->fStatementCount = 0;
}

void rlc_parsed_statement_list_destroy(
	struct RlcParsedStatementList * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fStatements)
	{
		for(size_t i = 0; i < this->fStatementCount; i++)
		{
			rlc_parsed_statement_destroy_virtual(this->fStatements[i]);
			rlc_free((void**)&this->fStatements[i]);
		}

		rlc_free((void**) &this->fStatements);

		this->fStatementCount = 0;
	}
}

void rlc_parsed_statement_list_add(
	struct RlcParsedStatementList * this,
	struct RlcParsedStatement * stmt)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(stmt != NULL);

	rlc_realloc(
		(void**)&this->fStatements,
		sizeof(struct RlcParsedStatement *) * ++this->fStatementCount);

	this->fStatements[this->fStatementCount-1] = stmt;
}