#include "expression.h"
#include "symbolexpression.h"
#include "symbolchildexpression.h"
#include "numberexpression.h"
#include "characterexpression.h"
#include "stringexpression.h"
#include "operatorexpression.h"
#include "thisexpression.h"
#include "nullexpression.h"
#include "castexpression.h"
#include "sizeofexpression.h"
#include "typeofexpression.h"
#include "symbolconstantexpression.h"

#include "../assert.h"
#include "../malloc.h"

#include <string.h>

void rlc_parsed_expression_create(
	struct RlcParsedExpression * this,
	enum RlcParsedExpressionType type,
	struct RlcToken first,
	struct RlcToken last)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedExpressionType));

	RLC_DERIVING_TYPE(this) = type;
	this->fStart = first;
	this->fEnd = last;
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
		(destructor_t)&rlc_parsed_character_expression_destroy,
		(destructor_t)&rlc_parsed_string_expression_destroy,
		(destructor_t)&rlc_parsed_operator_expression_destroy,
		(destructor_t)&rlc_parsed_this_expression_destroy,
		(destructor_t)&rlc_parsed_null_expression_destroy,
		(destructor_t)&rlc_parsed_cast_expression_destroy,
		(destructor_t)&rlc_parsed_sizeof_expression_destroy,
		(destructor_t)&rlc_parsed_typeof_expression_destroy,
		(destructor_t)&rlc_parsed_symbol_constant_expression_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedExpressionType), "ill sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSymbolExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSymbolChildExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedNumberExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedCharacterExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedStringExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedOperatorExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedThisExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedNullExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedCastExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSizeofExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedTypeofExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSymbolConstantExpression),
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
	struct RlcParsedOperatorExpression fRlcParsedOperatorExpression;
	struct RlcParsedThisExpression fRlcParsedThisExpression;
	struct RlcParsedThisExpression fRlcParsedNullExpression;
	struct RlcParsedCastExpression fRlcParsedCastExpression;
	struct RlcParsedSizeofExpression fRlcParsedSizeofExpression;
	struct RlcParsedSizeofExpression fRlcParsedTypeofExpression;
	struct RlcParsedSymbolConstantExpression fRlcParsedSymbolConstantExpression;
};

_Nodiscard static int dummy_rlc_parsed_operator_expression_parse(
	union RlcExpressionStorage * storage,
	struct RlcParser * parser)
{
	storage->fPointer = rlc_parsed_operator_expression_parse(parser);

	return storage->fPointer != NULL;
}

struct RlcParsedExpression * rlc_parsed_expression_parse(
	struct RlcParser * parser,
	int flags)
{
	RLC_DASSERT(parser != NULL);

	union RlcExpressionStorage storage;

	typedef _Nodiscard int (*parse_fn_t)(
		union RlcExpressionStorage *,
		struct RlcParser *);


#define ENTRY(Type, parse, ispointer) { \
		k ## Type,\
		(parse_fn_t)parse, \
		sizeof(struct Type), \
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct Type), \
		ispointer }

	static struct {
		enum RlcParsedExpressionType fType;
		parse_fn_t fParseFn;
		size_t fTypeSize;
		size_t fOffset;
		int fIsPointer;
	} const k_parse_lookup[] = {
		ENTRY(RlcParsedOperatorExpression, &dummy_rlc_parsed_operator_expression_parse, 1),
		ENTRY(RlcParsedNumberExpression, &rlc_parsed_number_expression_parse, 0),
		ENTRY(RlcParsedCharacterExpression, &rlc_parsed_character_expression_parse, 0),
		ENTRY(RlcParsedStringExpression, &rlc_parsed_string_expression_parse, 0),
		ENTRY(RlcParsedSymbolExpression, &rlc_parsed_symbol_expression_parse, 0),
		ENTRY(RlcParsedSymbolChildExpression, &rlc_parsed_symbol_child_expression_parse, 0),
		ENTRY(RlcParsedThisExpression, &rlc_parsed_this_expression_parse, 0),
		ENTRY(RlcParsedNullExpression, &rlc_parsed_null_expression_parse, 0),
		ENTRY(RlcParsedCastExpression, &rlc_parsed_cast_expression_parse, 0),
		ENTRY(RlcParsedSizeofExpression, &rlc_parsed_sizeof_expression_parse, 0),
		ENTRY(RlcParsedTypeofExpression, &rlc_parsed_typeof_expression_parse, 0),
		ENTRY(RlcParsedSymbolConstantExpression, &rlc_parsed_symbol_constant_expression_parse, 0)
	};

	static_assert(RLC_COVERS_ENUM(k_parse_lookup, RlcParsedExpressionType), "ill-sized parse table.");

	struct RlcParsedExpression * ret = NULL;

	for(size_t i = 0; i < _countof(k_parse_lookup); i++)
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
			}
		}
	}

	struct RlcToken tok;
	if(rlc_parser_consume(
		parser,
		&tok,
		kRlcTokParentheseOpen))
	{
		struct RlcParsedOperatorExpression * opexp = NULL;
		do {
			if(ret && !opexp)
			{
				opexp = make_operator_expression(
					kTuple,
					ret->fStart, tok);
				rlc_parsed_operator_expression_add(opexp, ret);
			}

			ret = rlc_parsed_expression_parse(
				parser,
				RLC_ALL_FLAGS(RlcParsedExpressionType));
			if(!ret)
				rlc_parser_fail(parser, "expected expression");

			if(opexp)
				rlc_parsed_operator_expression_add(opexp, ret);
			else
				ret->fStart = tok;

		} while(kRlcTokComma == rlc_parser_expect(
			parser,
			&tok,
			2,
			kRlcTokParentheseClose,
			kRlcTokComma));

		if(opexp)
			ret = RLC_BASE_CAST(opexp, RlcParsedExpression);
		ret->fEnd = tok;
		return ret;
	}

	return NULL;
}

void rlc_parsed_expression_print(
	struct RlcParsedExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{

	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcParsedExpressionType));

	typedef void (*print_fn_t) (
		void *,
		struct RlcSrcFile const *,
		FILE *);

	static print_fn_t const k_vtable[] = {
		(print_fn_t)&rlc_parsed_symbol_expression_print,
		(print_fn_t)&rlc_parsed_symbol_child_expression_print,
		(print_fn_t)&rlc_parsed_number_expression_print,
		(print_fn_t)&rlc_parsed_character_expression_print,
		(print_fn_t)&rlc_parsed_string_expression_print,
		(print_fn_t)&rlc_parsed_operator_expression_print,
		(print_fn_t)&rlc_parsed_this_expression_print,
		(print_fn_t)&rlc_parsed_null_expression_print,
		(print_fn_t)&rlc_parsed_cast_expression_print,
		(print_fn_t)&rlc_parsed_sizeof_expression_print,
		(print_fn_t)&rlc_parsed_typeof_expression_print,
		(print_fn_t)&rlc_parsed_symbol_constant_expression_print,
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedExpressionType), "ill sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSymbolExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSymbolChildExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedNumberExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedCharacterExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedStringExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedOperatorExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedThisExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedNullExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedCastExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSizeofExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedTypeofExpression),
		RLC_DERIVE_OFFSET(RlcParsedExpression, struct RlcParsedSymbolConstantExpression),
	};

	static_assert(RLC_COVERS_ENUM(k_offsets, RlcParsedExpressionType), "ill sized offset table.");

	if(k_vtable[RLC_DERIVING_TYPE(this)])
		k_vtable[RLC_DERIVING_TYPE(this)](
			(uint8_t*)this + k_offsets[RLC_DERIVING_TYPE(this)],
			file,
			out);
}

void rlc_parsed_expression_list_create(
	struct RlcParsedExpressionList * this)
{
	RLC_DASSERT(this != NULL);

	this->fValues = NULL;
	this->fCount = 0;
}

void rlc_parsed_expression_list_destroy(
	struct RlcParsedExpressionList * this)
{
	RLC_DASSERT(this != NULL);


	if(this->fValues)
	{
		for(size_t i = 0; i < this->fCount; i++)
		{
			rlc_parsed_expression_destroy_virtual(this->fValues[i]);
			rlc_free((void**)&this->fValues[i]);
		}
		rlc_free((void**)&this->fValues);
	}
	this->fCount = 0;
}

void rlc_parsed_expression_list_append(
	struct RlcParsedExpressionList * this,
	struct RlcParsedExpression * value)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(value != NULL);

	rlc_realloc(
		(void**)&this->fValues,
		sizeof(struct RlcParsedExpression *) * ++this->fCount);
	this->fValues[this->fCount-1] = value;
}