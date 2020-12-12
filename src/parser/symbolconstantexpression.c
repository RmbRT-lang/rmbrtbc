#include "symbolconstantexpression.h"

#include "../assert.h"
#include "../malloc.h"

#include <string.h>

static RlcSrcSize symbolCount = 0;
static char const ** symbols = NULL;

void rlc_parsed_symbol_constant_register(
	struct RlcSrcFile const * file,
	struct RlcSrcString const * name)
{
	RlcSrcIndex left = 0, right = symbolCount;

	char const * str = rlc_src_string_cstr(name, file);

	RlcSrcIndex i = 0;
	while(left < right)
	{
		i = (left+right)>>1;
		signed sign = strcmp(str, symbols[i]);
		if(sign < 0)
			right = i;
		else if(sign > 0)
			left = i+1;
		else
		{
			rlc_free((void**)&str);
			return;
		}
	}

	rlc_realloc(
		(void**)&symbols,
		++symbolCount * sizeof(char*));

	for(RlcSrcIndex k = symbolCount; k-->left+1;)
		symbols[k] = symbols[k-1];

	symbols[left] = str;
}

void rlc_parsed_symbol_constant_free()
{
	if(symbolCount)
	{
		do {
			rlc_free((void**)&symbols[--symbolCount]);
		} while(symbolCount);

		rlc_free((void**)&symbols);
	}
}

void rlc_parsed_symbol_constant_print(FILE * out)
{
	fputs("namespace __rl::constant {\n", out);

	for(RlcSrcIndex i = 0; i < symbolCount; i++)
	{
		char const * name = symbols[i];
		fprintf(out, "struct _t_%s {} const _v_%s{};\n", name, name);
	}

	fputs("}\n", out);
}


void rlc_parsed_symbol_constant_expression_create(
	struct RlcParsedSymbolConstantExpression * this,
	struct RlcToken const * op,
	struct RlcToken const * name)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedSymbolConstantExpression,
		*op, *name);

	this->fName = name->content;
}

int rlc_parsed_symbol_constant_expression_parse(
	struct RlcParsedSymbolConstantExpression * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	struct RlcToken op, name;
	if(!rlc_parser_consume(parser, &op, kRlcTokColon))
		return 0;

	struct RlcParserTracer trace;
	rlc_parser_trace(parser, "symbol constant expression", &trace);

	rlc_parser_expect(parser, &name, 1, kRlcTokIdentifier);
	rlc_parsed_symbol_constant_expression_create(out, &op, &name);
	rlc_parsed_symbol_constant_register(rlc_parser_file(parser), &out->fName);

	rlc_parser_untrace(parser, &trace);
	return 1;
}

void rlc_parsed_symbol_constant_expression_destroy(
	struct RlcParsedSymbolConstantExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));
}

void rlc_parsed_symbol_constant_expression_print(
	struct RlcParsedSymbolConstantExpression * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fputs("::__rl::constant::_v_", out);
	rlc_src_string_print_noreplace(&this->fName, file, out);
}