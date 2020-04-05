#include "statement.h"
#include "scope.h"

#include "expressionstatement.h"
#include "blockstatement.h"
#include "ifstatement.h"
#include "loopstatement.h"
#include "breakstatement.h"
#include "continuestatement.h"
#include "../parser/expressionstatement.h"
#include "../parser/blockstatement.h"
#include "../parser/ifstatement.h"
#include "../parser/loopstatement.h"
#include "../parser/breakstatement.h"
#include "../parser/continuestatement.h"

#include "../assert.h"
#include "../malloc.h"

struct RlcScopedStatement * rlc_scoped_statement_new(
	struct RlcSrcFile const * file,
	struct RlcParsedStatement const * parsed)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	typedef void (*constructor_t)(
		void *,
		struct RlcSrcFile const *,
		void const *);

#define ENTRY(type, ctor) { \
		(constructor_t) ctor, \
		RLC_DERIVE_OFFSET(RlcScopedStatement, struct RlcScoped ## type), \
		RLC_DERIVE_OFFSET(RlcParsedStatement, struct RlcParsed ## type), \
		sizeof(struct RlcScoped ## type), \
		kRlcParsed ## type \
	}
#define NOENTRY(type) { NULL, 0, 0, 0, kRlcParsed ## type }

	static struct {
		constructor_t ctor;
		ptrdiff_t offset;
		ptrdiff_t parsed_offset;
		size_t size;
		enum RlcScopedStatementType type;
	} const k_vtable[] = {
		ENTRY(ExpressionStatement, rlc_scoped_expression_statement_create),
		ENTRY(BlockStatement, rlc_scoped_block_statement_create),
		ENTRY(IfStatement, rlc_scoped_if_statement_create),
		ENTRY(LoopStatement, rlc_scoped_loop_statement_create),
		NOENTRY(VariableStatement),
		NOENTRY(ReturnStatement),
		NOENTRY(SwitchStatement),
		NOENTRY(CaseStatement),
		ENTRY(BreakStatement, rlc_scoped_break_statement_create),
		ENTRY(ContinueStatement, rlc_scoped_continue_statement_create),
		NOENTRY(TryStatement),
		NOENTRY(ThrowStatement)
	};
#undef ENTRY
#undef NOENTRY


	static_assert(RLC_COVERS_ENUM(k_vtable, RlcScopedStatementType),
		"ill-sized vtable");

	unsigned type = RLC_DERIVING_TYPE(parsed);
	RLC_DASSERT(k_vtable[type].ctor != NULL);
	RLC_DASSERT(k_vtable[type].type == type);

	struct RlcScopedStatement * this = NULL;
	rlc_malloc((void**)&this, k_vtable[type].size);
	k_vtable[type].ctor(
		((uint8_t *) this) + k_vtable[type].offset,
		file,
		((uint8_t const *) parsed) + k_vtable[type].parsed_offset);

	return this;
}

void rlc_scoped_statement_delete(
	struct RlcScopedStatement * this)
{
	RLC_DASSERT(this != NULL);

	typedef void (*destructor_t)(
		void *);

#define ENTRY(type, dtor) { \
		(destructor_t) dtor, \
		RLC_DERIVE_OFFSET(RlcScopedStatement, struct RlcScoped ## type), \
		kRlcParsed ## type \
	}
#define NOENTRY(type) { NULL, 0, kRlcParsed ## type }

	static struct {
		destructor_t dtor;
		ptrdiff_t offset;
		enum RlcScopedStatementType type;
	} const k_vtable[] = {
		ENTRY(ExpressionStatement, rlc_scoped_expression_statement_destroy),
		ENTRY(BlockStatement, rlc_scoped_block_statement_destroy),
		ENTRY(IfStatement, rlc_scoped_if_statement_destroy),
		ENTRY(LoopStatement, rlc_scoped_loop_statement_destroy),
		NOENTRY(VariableStatement),
		NOENTRY(ReturnStatement),
		NOENTRY(SwitchStatement),
		NOENTRY(CaseStatement),
		ENTRY(BreakStatement, rlc_scoped_break_statement_destroy),
		ENTRY(ContinueStatement, rlc_scoped_continue_statement_destroy),
		NOENTRY(TryStatement),
		NOENTRY(ThrowStatement)
	};
#undef ENTRY
#undef NOENTRY

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcScopedStatementType),
		"ill-sized vtable");

	unsigned type = RLC_DERIVING_TYPE(this);
	RLC_DASSERT(k_vtable[type].dtor != NULL);
	RLC_DASSERT(k_vtable[type].type == type);

	k_vtable[type].dtor(
		((uint8_t *) this) + k_vtable[type].offset);

	rlc_free((void**)&this);
}

void rlc_scoped_statement_create(
	struct RlcScopedStatement * this,
	struct RlcParsedStatement const * parsed,
	enum RlcScopedStatementType type,
	int make_scope)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcScopedStatementType));
	RLC_DASSERT((int)type == (int)RLC_DERIVING_TYPE(parsed));

	RLC_DERIVING_TYPE(this) = type;
	this->parsed = parsed;

	this->scope = make_scope
		? rlc_scoped_scope_new_for_statement(this)
		: NULL;
}

void rlc_scoped_statement_destroy_base(
	struct RlcScopedStatement * this)
{
	if(this->scope)
		rlc_scoped_scope_delete(this->scope);
}