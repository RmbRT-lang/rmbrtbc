#include "expression.h"
#include "symbolexpression.h"
#include "symbolchildexpression.h"
#include "numberexpression.h"
#include "stringexpression.h"
#include "typenameexpression.h"
#include "operatorexpression.h"

#include "../assert.h"
#include "../malloc.h"

#include <string.h>

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
		(destructor_t)&rlc_parsed_symbol_child_expression_destroy,
		(destructor_t)&rlc_parsed_number_expression_destroy,
		(destructor_t)&rlc_parsed_string_expression_destroy,
		(destructor_t)&rlc_parsed_type_name_expression_destroy,
		(destructor_t)&rlc_parsed_operator_expression_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedExpressionType), "ill sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSymbolExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSymbolChildExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedNumberExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedStringExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedTypeNameExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedOperatorExpression)
	};

	static_assert(RLC_COVERS_ENUM(k_offsets, RlcParsedExpressionType), "ill sized offset table.");

	k_vtable[RLC_DERIVING_TYPE(this)]((uint8_t*)this + k_offsets[RLC_DERIVING_TYPE(this)]);
}

void rlc_parsed_expression_destroy_base(
	struct RlcParsedExpression * this)
{
	RLC_DASSERT(this != NULL);
}

union RlcExpressionStorage
{
	struct RlcParsedExpression * fPointer;
	struct RlcParsedNumberExpression fRlcParsedNumberExpression;
	struct RlcParsedSymbolExpression fRlcParsedSymbolExpression;
	struct RlcParsedSymbolChildExpression fRlcParsedSymbolChildExpression;
	struct RlcParsedTypeNameExpression fRlcParsedTypeNameExpression;
	struct RlcParsedOperatorExpression fRlcParsedOperatorExpression;
};

static int dummy_rlc_parsed_operator_expression_parse(
	union RlcExpressionStorage * storage,
	struct RlcParserData * parser)
{
	storage->fPointer = rlc_parsed_operator_expression_parse(parser);

	return storage->fPointer != NULL;
}

struct RlcParsedExpression * rlc_parsed_expression_parse(
	struct RlcParserData * parser,
	int flags)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(!parser->fErrors);
	RLC_DASSERT(!parser->fErrorCount);

	union RlcExpressionStorage storage;

	typedef int (*parse_fn_t)(
		union RlcExpressionStorage *,
		struct RlcParserData *);


#define ENTRY(Type, parse, error, ispointer) { \
		k ## Type,\
		(parse_fn_t)parse, \
		error, \
		sizeof(struct Type), \
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct Type), \
		ispointer }

	static struct {
		enum RlcParsedExpressionType fType;
		parse_fn_t fParseFn;
		enum RlcParseError fErrorCode;
		size_t fTypeSize;
		size_t fOffset;
		int fIsPointer;
	} const k_parse_lookup[] = {
		ENTRY(RlcParsedOperatorExpression, &dummy_rlc_parsed_operator_expression_parse, kRlcParseErrorExpectedOperatorExpression, 1),
		ENTRY(RlcParsedNumberExpression, &rlc_parsed_number_expression_parse, kRlcParseErrorExpectedNumberExpression, 0),
		ENTRY(RlcParsedStringExpression, &rlc_parsed_string_expression_parse, kRlcParseErrorExpectedStringExpression, 0),
		ENTRY(RlcParsedTypeNameExpression, &rlc_parsed_type_name_expression_parse, kRlcParseErrorExpectedTypeNameExpression, 0),
		ENTRY(RlcParsedSymbolExpression, &rlc_parsed_symbol_expression_parse, kRlcParseErrorExpectedSymbolExpression, 0),
		ENTRY(RlcParsedSymbolChildExpression, &rlc_parsed_symbol_child_expression_parse, kRlcParseErrorExpectedSymbolChildExpression, 0)
	};

	static_assert(RLC_COVERS_ENUM(k_parse_lookup, RlcParsedExpressionType), "ill-sized parse table.");

	enum RlcParseError error_code;
	struct RlcParsedExpression * ret;

	for(int i = 0; i < _countof(k_parse_lookup); i++)
	{
		if(RLC_FLAG(k_parse_lookup[i].fType) & flags)
		{
			if(k_parse_lookup[i].fParseFn(
				&storage,
				parser))
			{
				if(!k_parse_lookup[i].fIsPointer)
				{
					void * temp = NULL;
					rlc_malloc(&temp, k_parse_lookup[i].fTypeSize);

					memcpy(temp, &storage, k_parse_lookup[i].fTypeSize);

					ret = (void*) ((uint8_t*)temp + k_parse_lookup[i].fOffset);

					return ret;
				} else
					return storage.fPointer;
			} else if(parser->fErrorCount)
			{
				error_code = k_parse_lookup[i].fErrorCode;
				goto failure;
			}
		}
	}

	if(rlc_parser_data_consume(
		parser,
		kRlcTokParentheseOpen))
	{
		ret = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType));

		if(rlc_parser_data_consume(
			parser,
			kRlcTokParentheseClose))
		{
			return ret;
		} else
		{
			rlc_parsed_expression_destroy_virtual(ret);
			rlc_free((void**)&ret);

			error_code = kRlcParseErrorExpectedParentheseClose;
			goto failure;
		}
	} else return NULL;

failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	return NULL;
}