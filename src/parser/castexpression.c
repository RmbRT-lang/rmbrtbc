#include "castexpression.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_cast_expression_create(
	struct RlcParsedCastExpression * this,
	struct RlcToken first,
	struct RlcToken last)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedCastExpression,
		first,
		last);

	this->fValues = NULL;
	this->fValueCount = 0;
}


void rlc_parsed_cast_expression_destroy(
	struct RlcParsedCastExpression * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fValues)
	{
		for(RlcSrcIndex i = 0; i < this->fValueCount; i++)
		{
			rlc_parsed_expression_destroy_virtual(this->fValues[i]);
			rlc_free((void**)&this->fValues[i]);
		}
		rlc_free((void**)&this->fValues);
		this->fValueCount = 0;
	}

	rlc_parsed_type_name_destroy(&this->fType);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

int rlc_parsed_cast_expression_parse(
	struct RlcParsedCastExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	struct RlcToken first;

	static struct {
		enum RlcTokenType open, close;
		enum RlcCastType type;
		int allowMultipleArgs, expectArgs;
	} const k_lookup[] = {
		{ kRlcTokLess, kRlcTokGreater, kRlcCastTypeStatic, 1, 0},
		{ kRlcTokDoubleLess, kRlcTokDoubleGreater, kRlcCastTypeDynamic, 0, 1 },
		{ kRlcTokTripleLess, kRlcTokTripleGreater, kRlcCastTypeConcept, 1, 1 }
	};

	unsigned type;
	for(type = 0; type < _countof(k_lookup); type++)
		if(rlc_parser_consume(parser, &first, k_lookup[type].open))
			break;
	if(type == _countof(k_lookup))
		return 0;

	rlc_parsed_type_name_parse(
		&out->fType,
		parser,
		1);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		k_lookup[type].close);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	struct RlcParsedExpression ** values = NULL;
	RlcSrcIndex valueCount = 0;

	struct RlcToken end;
	if(k_lookup[type].expectArgs
	|| !rlc_parser_consume(parser, &end, kRlcTokParentheseClose))
	{
		do
		{
			struct RlcParsedExpression * value = rlc_parsed_expression_parse(
				parser,
				RLC_ALL_FLAGS(RlcParsedExpressionType));
			if(!value)
				rlc_parser_fail(parser, "expected expression");

			rlc_realloc((void**)&values, ++valueCount * sizeof(struct RlcParsedExpression *));
			values[valueCount-1] = value;
		} while(k_lookup[type].allowMultipleArgs
				&& rlc_parser_consume(parser, NULL, kRlcTokComma));

		rlc_parser_expect(
			parser,
			&end,
			1,
			kRlcTokParentheseClose);
	}
	rlc_parsed_cast_expression_create(out, first, end);
	out->fValues = values;
	out->fValueCount = valueCount;
	out->fMethod = k_lookup[type].type;

	return 1;
}

void rlc_parsed_cast_expression_print(
	struct RlcParsedCastExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	int needDeRef = 0;
	int needRef = 0;
	fputc('(', out);
	switch(this->fMethod)
	{
	case kRlcCastTypeStatic:
		fputs("::__rl::__rl_cast<", out);
		rlc_parsed_type_name_print(&this->fType, file, out);
		fputs(">", out);
		break;
	case kRlcCastTypeDynamic:
		{
		int isNonNull;
		needDeRef = rlc_parsed_type_is_ptr(&this->fType, &isNonNull);
		needRef = !isNonNull;
		if(isNonNull)
			fputs("&", out);
		fputs("dynamic_cast<", out);
		if(isNonNull)
			fputs("std::remove_pointer_t<", out);
		rlc_parsed_type_name_print(&this->fType, file, out);
		if(isNonNull)
			fputs("> &", out);
		fputs(">", out);
		}break;
	case kRlcCastTypeConcept:
		rlc_parsed_type_name_print(&this->fType, file, out);
		fputs("::FROM", out);
		break;
	default:
		RLC_DASSERT(!"unhandled type");
	}
	fputs("(", out);
	if(needRef) fputs("&(", out);
	if(needDeRef) fputs("*(", out);
	for(RlcSrcIndex i = 0; i < this->fValueCount; i++)
	{
		if(i) fputs(", ", out);
		rlc_parsed_expression_print(this->fValues[i], file, out);
	}
	if(needRef) fputc(')', out);
	if(needDeRef) fputc(')', out);
	fputs("))", out);
}