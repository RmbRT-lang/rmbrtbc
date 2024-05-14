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

	static char const * const ctors[] = { "default_init_t", "bare_init_t" };

	for(RlcSrcIndex i = 0; i < symbolCount; i++)
	{
		char const * name = symbols[i];
		fprintf(out,
			"struct _t_%s : public __rl::SymbolBase<_t_%s> {\n"
				"	static constexpr char const * const __rl_type_name_v = \":%s\"; "
				"	_t_%s() = default;",
			name, name, name, name);
		for(unsigned c = 0; c < _countof(ctors); c++)
			fprintf(out,
				" _t_%s(::__rl::%s) {}\n",
				name, ctors[c]);
		fprintf(out, "} const _v_%s{};\n", name);


		fprintf(out,
			"template<class T>\n"
			"struct _tt_%s : public __rl::SymbolBase<_tt_%s<T>> {\n"
				"	static constexpr char const * const __rl_type_name_v = \":%s\"; "
			"	_tt_%s() = default;", name, name, name, name);
		for(unsigned c = 0; c < _countof(ctors); c++)
			fprintf(out,
			" _tt_%s(::__rl::%s) {}\n",
				name, ctors[c]);
		fprintf(out,
			"};\n"
			"template<class T>\n"
			"_tt_%s<T> const __t_v_%s{};\n",
			name, name);
	}

	fputs("}\n", out);
}


void rlc_parsed_symbol_constant_expression_create(
	struct RlcParsedSymbolConstantExpression * this,
	struct RlcToken const * op,
	struct RlcToken const * name,
	struct RlcParsedTypeName const * type)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this, RlcParsedExpression),
		kRlcParsedSymbolConstantExpression,
		*op, *name);

	this->fName = *name;
	if(type)
	{
		this->fHasType = 1;
		this->fType = *type;
	} else
		this->fHasType = 0;
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

	rlc_parser_expect(parser, &name, 7,
		kRlcTokIdentifier,
		kRlcTokLess, // acquire
		kRlcTokGreater, // release
		kRlcTokLessGreater, // acq_rel
		kRlcTokQuestionMark, // relaxed
		kRlcTokExclamationMark, // seq_cst
		kRlcTokLessMinus // consume
	);

	if(rlc_parser_consume(parser, NULL, kRlcTokDot))
	{
		struct RlcParsedTypeName type;
		if(!rlc_parsed_type_name_parse(&type, parser, 0))
			rlc_parser_fail(parser, "expected type name");
		rlc_parsed_symbol_constant_expression_create(out, &op, &name, &type);
	} else
		rlc_parsed_symbol_constant_expression_create(out, &op, &name, NULL);

	if(name.type == kRlcTokIdentifier)
		rlc_parsed_symbol_constant_register(
			rlc_parser_file(parser),
			&out->fName.content);

	rlc_parser_untrace(parser, &trace);
	return 1;
}

void rlc_parsed_symbol_constant_expression_destroy(
	struct RlcParsedSymbolConstantExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this, RlcParsedExpression));

	if(this->fHasType)
	{
		rlc_parsed_type_name_destroy(&this->fType);
		this->fHasType = 0;
	}
}

void rlc_parsed_symbol_constant_expression_print(
	struct RlcParsedSymbolConstantExpression * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fputs("::__rl::constant::", out);
	if(this->fHasType)
		fputs("template __t", out);

	switch(this->fName.type)
	{
	case kRlcTokIdentifier:
		fputs("_v_", out);
		rlc_src_string_print_noreplace(&this->fName.content, file, out);
		break;
	case kRlcTokLess:
		fputs("__v_less", out);
		break;
	case kRlcTokGreater:
		fputs("__v_greater", out);
		break;
	case kRlcTokLessGreater:
		fputs("__v_less_greater", out);
		break;
	case kRlcTokQuestionMark:
		fputs("__v_question_mark", out);
		break;
	case kRlcTokExclamationMark:
		fputs("__v_exclamation_mark", out);
		break;
	case kRlcTokLessMinus:
		fputs("__v_less_minus", out);
		break;
	default:
		RLC_DASSERT(!"unhandled symbol constant");
	}

	if(this->fHasType)
	{
		fputc('<', out);
		rlc_parsed_type_name_print(&this->fType, file, out);
		fputc('>', out);
	}
}