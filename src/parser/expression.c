#include "expression.h"
#include "symbolexpression.h"
#include "numberexpression.h"
#include "typenameexpression.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_expression_create(
	struct RlcParsedExpression * this,
	enum RlcParsedExpressionType type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedExpressionType));

	RLC_DERIVING_TYPE(this) = type;
	this->fFirst = 0;
	this->fLast = 0;
}

void rlc_parsed_expression_destroy_virtual(
	struct RlcParsedExpression * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcParsedExpressionType));

	typedef void (*destructor_t) (void *);

	static destructor_t const k_vtable[] = {
		(destructor_t)&rlc_parsed_symbol_expression_destroy,
		(destructor_t)&rlc_parsed_number_expression_destroy,
		(destructor_t)&rlc_parsed_type_name_expression_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedExpressionType), "ill sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSymbolExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedNumberExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedTypeNameExpression)
	};

	static_assert(RLC_COVERS_ENUM(k_offsets, RlcParsedExpressionType), "ill sized offset table.");

	k_vtable[RLC_DERIVING_TYPE(this)]((uint8_t*)this + k_offsets[RLC_DERIVING_TYPE(this)]);
}

int rlc_parsed_expression_parse(
	struct RlcParsedExpression ** out,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(*out == NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(!parser->fErrorCount);

	union RlcExpressionStorage
	{
		struct RlcParsedExpression fRlcParsedExpression;
		struct RlcParsedNumberExpression fRlcParsedNumberExpression;
		struct RlcParsedSymbolExpression fRlcParsedSymbolExpression;
		struct RlcParsedTypeNameExpression fRlcParsedTypeNameExpression;
	} storage;

	if(rlc_parsed_number_expression_parse(
		&storage.fRlcParsedNumberExpression,
		parser))
	{
		rlc_malloc((void**)out, sizeof(struct RlcParsedNumberExpression));
		*(struct RlcParsedNumberExpression *)*out = storage.fRlcParsedNumberExpression;

		return 1; 
	} else if(!parser->fErrorCount && rlc_parsed_symbol_expression_parse(
		&storage.fRlcParsedSymbolExpression,
		parser))
	{
		rlc_malloc((void**)out, sizeof(struct RlcParsedSymbolExpression));
		*(struct RlcParsedSymbolExpression *)*out = storage.fRlcParsedSymbolExpression;

		return 1;
	} else if(!parser->fErrorCount && rlc_parsed_type_name_expression_parse(
			&storage.fRlcParsedTypeNameExpression,
			parser))
	{
		rlc_malloc((void**)out, sizeof(struct RlcParsedTypeNameExpression));
		*(struct RlcParsedTypeNameExpression *)*out = storage.fRlcParsedTypeNameExpression;

		return 1;
	} else if(parser->fErrorCount)
	{
		rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedExpression);
	}

	return 0;
}