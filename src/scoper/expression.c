#include "expression.h"
#include "../parser/symbolexpression.h"
#include "symbolexpression.h"
#include "../parser/numberexpression.h"
#include "symbolchildexpression.h"
#include "../parser/symbolchildexpression.h"
#include "numberexpression.h"
#include "../parser/characterexpression.h"
#include "characterexpression.h"
#include "../parser/stringexpression.h"
#include "stringexpression.h"
#include "../parser/operatorexpression.h"
#include "operatorexpression.h"

struct RlcScopedExpression * rlc_scoped_expression_new(
	struct RlcParsedExpression const * parsed,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	typedef char * (*constructor_t)(
		void const *,
		struct RlcSrcFile const *);

#define ENTRY(type, ctor) { \
		(constructor_t) &ctor, \
		RLC_DERIVE_OFFSET(RlcScopedExpression, struct RlcScoped##type), \
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsed##type), \
		kRlcScoped##type \
	}
#define NOENTRY(type) { NULL }
	static struct {
		constructor_t ctor;
		intptr_t offset;
		intptr_t parsed_offset;
		enum RlcScopedExpressionType type;
	} const k_vtable[] = {
		ENTRY(SymbolExpression, rlc_scoped_symbol_expression_new),
		ENTRY(SymbolChildExpression, rlc_scoped_symbol_child_expression_new),
		ENTRY(NumberExpression, rlc_scoped_number_expression_new),
		ENTRY(CharacterExpression, rlc_scoped_character_expression_new),
		ENTRY(StringExpression, rlc_scoped_string_expression_new),
		ENTRY(OperatorExpression, rlc_scoped_operator_expression_new),
		NOENTRY(ThisExpression),
		NOENTRY(CastExpression),
		NOENTRY(SizeofExpression)
	};
#undef ENTRY
#undef NOENTRY

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcScopedExpressionType),
		"Ill-sized vtable.");

	enum RlcParsedExpressionType type = RLC_DERIVING_TYPE(parsed);

	RLC_DASSERT(k_vtable[type].ctor != NULL);
	RLC_DASSERT(k_vtable[type].type == type);

	char * obj = k_vtable[type].ctor(
		(char const *)parsed + k_vtable[type].parsed_offset,
		file);
	obj += k_vtable[type].offset;
	return (struct RlcScopedExpression*) obj;
}

void rlc_scoped_expression_delete_virtual(
	struct RlcScopedExpression * this)
{
	RLC_DASSERT(this != NULL);

	typedef void (*destructor_t)(
		void *);

#define ENTRY(type, dtor) { \
		(destructor_t) &dtor, \
		RLC_DERIVE_OFFSET(RlcScopedExpression, struct RlcScoped##type), \
		kRlcScoped##type \
	}
#define NOENTRY(type) { NULL }
	static struct {
		destructor_t dtor;
		intptr_t offset;
		enum RlcScopedExpressionType type;
	} const k_vtable[] = {
		ENTRY(SymbolExpression, rlc_scoped_symbol_expression_delete),
		ENTRY(SymbolChildExpression, rlc_scoped_symbol_child_expression_delete),
		ENTRY(NumberExpression, rlc_scoped_number_expression_delete),
		ENTRY(CharacterExpression, rlc_scoped_character_expression_delete),
		ENTRY(StringExpression, rlc_scoped_string_expression_delete),
		ENTRY(OperatorExpression, rlc_scoped_operator_expression_delete),
		NOENTRY(ThisExpression),
		NOENTRY(CastExpression),
		NOENTRY(SizeofExpression)
	};
#undef ENTRY
#undef NOENTRY

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcScopedExpressionType),
		"Ill-sized vtable.");

	enum RlcParsedExpressionType type = RLC_DERIVING_TYPE(this);

	RLC_DASSERT(k_vtable[type].type == type);

	k_vtable[type].dtor(
		(char *)this + k_vtable[type].offset);
}

void rlc_scoped_expression_create(
	struct RlcScopedExpression * this,
	struct RlcParsedExpression const * parsed,
	enum RlcScopedExpressionType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type,RlcParsedExpressionType));
	RLC_DASSERT((int)type == (int)RLC_DERIVING_TYPE(parsed));

	RLC_DERIVING_TYPE(this) = type;
	this->fParsed = parsed;
}

void rlc_scoped_expression_destroy_base(
	struct RlcScopedExpression * this)
{
	RLC_DASSERT(this != NULL);

	this->fParsed = NULL;
}