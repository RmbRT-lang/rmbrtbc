#include "typeswitchstatement.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_type_switch_statement_create(
	struct RlcParsedTypeSwitchStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedTypeSwitchStatement);
	this->fIsStatic = 0;
	this->fExpression = NULL;
	this->fCases = NULL;
	this->fCaseCount = 0;
}

static void rlc_parsed_type_case_statement_destroy(
	struct RlcParsedTypeCaseStatement * this)
{
	if(!this->fIsDefault)
	{
		for(RlcSrcIndex i = 0; i < this->fTypeNameCount; i++)
			rlc_parsed_symbol_destroy(&this->fTypeNames[i]);
		if(this->fTypeNames)
			rlc_free((void**)&this->fTypeNames);
	}
	rlc_parsed_statement_destroy_virtual(this->fBodyStatement);
	rlc_free((void**)&this->fBodyStatement);
}

void rlc_parsed_type_switch_statement_destroy(
	struct RlcParsedTypeSwitchStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_virtual(this->fExpression);
	rlc_free((void**)&this->fExpression);

	for(RlcSrcIndex i = 0; i < this->fCaseCount; i++)
		rlc_parsed_type_case_statement_destroy(&this->fCases[i]);
	if(this->fCases)
	{
		rlc_free((void**)&this->fCases);
		this->fCaseCount = 0;
	}

	rlc_parsed_statement_destroy_base(RLC_BASE_CAST(this, RlcParsedStatement));
}


static void rlc_parsed_type_case_statement_parse(
	struct RlcParsedTypeCaseStatement * out,
	struct RlcParser * parser)
{
	out->fIsDefault = kRlcTokDefault == rlc_parser_expect(parser, NULL, 2,
		kRlcTokDefault, kRlcTokCase);
	out->fTypeNames = NULL;
	out->fTypeNameCount = 0;

	if(!out->fIsDefault)
		do
		{
			struct RlcParsedSymbol typeName;
			if(!rlc_parsed_symbol_parse(&typeName, parser, 0))
				rlc_parser_fail(parser, "expected type name");

			rlc_realloc(
				(void**)&out->fTypeNames,
				sizeof(struct RlcParsedSymbol) * ++out->fTypeNameCount);
			out->fTypeNames[out->fTypeNameCount-1] = typeName;
		} while(rlc_parser_consume(parser, NULL, kRlcTokComma));
	rlc_parser_expect(parser, NULL, 1, kRlcTokColon);
	if(!(out->fBodyStatement = rlc_parsed_statement_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedStatementType)
		&~RLC_FLAG(kRlcParsedCaseStatement)
		&~RLC_FLAG(kRlcParsedVariableStatement))))
		rlc_parser_fail(parser, "expected block statement");
}

int rlc_parsed_type_switch_statement_parse(
	struct RlcParsedTypeSwitchStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_is_ahead(parser, kRlcTokSwitch)
	|| !rlc_parser_consume(
		parser,
		NULL,
		kRlcTokType))
		return 0;
	rlc_parser_skip(parser);

	struct RlcParserTracer ctx;
	rlc_parser_trace(parser, "type switch", &ctx);

	rlc_parsed_type_switch_statement_create(out);

	out->fIsStatic = rlc_parser_consume(parser, NULL, kRlcTokStatic);

	rlc_parsed_control_label_parse(&out->fLabel, parser);

	rlc_parser_expect(parser, NULL, 1, kRlcTokParentheseOpen);

	if(!(out->fExpression = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType))))
		rlc_parser_fail(parser, "expected expression");

	rlc_parser_expect(parser, NULL, 1, kRlcTokParentheseClose);
	rlc_parser_expect(parser, NULL, 1, kRlcTokBraceOpen);

	do
	{
		struct RlcParsedTypeCaseStatement stmt;
		rlc_parsed_type_case_statement_parse(&stmt, parser);

		rlc_realloc(
			(void**)&out->fCases,
			++out->fCaseCount * sizeof(struct RlcParsedTypeCaseStatement));
		out->fCases[out->fCaseCount-1] = stmt;
	} while(!rlc_parser_consume(parser, NULL, kRlcTokBraceClose));

	rlc_parser_untrace(parser, &ctx);
	return 1;
}

void rlc_parsed_type_switch_statement_print(
	struct RlcParsedTypeSwitchStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fprintf(out, "switch(__rl::%s(",
		this->fIsStatic ? "type_number" : "deriving_type_number");
	rlc_parsed_expression_print(this->fExpression, file, out);
	fputs("))\n{", out);
	for(RlcSrcSize i = 0; i < this->fCaseCount; i++)
	{
		if(this->fCases[i].fIsDefault)
			fputs("default:", out);
		else for(RlcSrcIndex j = 0; j<this->fCases[i].fTypeNameCount; j++)
		{
			fputs("case __rl::type_number<", out);
			rlc_parsed_symbol_print(&this->fCases[i].fTypeNames[j], file, out);
			fputs(">():", out);
		}

		rlc_parsed_statement_print(this->fCases[i].fBodyStatement, file, out);
		fputs("break;\n", out);
	}
	fputs("}\n", out);
}