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
		{
			rlc_free((void**)&this->fTypeNames);
			rlc_free((void**)&this->fAddTypeName);
		}
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
	out->fIsDefault = rlc_parser_consume(parser, NULL, kRlcTokDefault);
	out->fTypeNames = NULL;
	out->fAddTypeName = NULL;
	out->fTypeNameCount = 0;

	if(!out->fIsDefault)
		do
		{
			struct RlcParsedSymbol typeName;
			if(!rlc_parsed_symbol_parse(&typeName, parser))
				rlc_parser_fail(parser, "expected type name");

			rlc_realloc(
				(void**)&out->fTypeNames,
				sizeof(struct RlcParsedSymbol) * ++out->fTypeNameCount);
			rlc_realloc((void**)&out->fAddTypeName, out->fTypeNameCount);
			out->fTypeNames[out->fTypeNameCount-1] = typeName;
			out->fAddTypeName[out->fTypeNameCount-1] =
				rlc_parser_consume(parser, NULL, kRlcTokPlus);
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

	out->fIsStrict = !rlc_parser_consume(parser, NULL, kRlcTokQuestionMark);
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
	fputs("{auto const& __rl_type_switch_var = ", out);
	rlc_parsed_expression_print(this->fExpression, file, out);
	fputs(";\n", out);

	int hasDefault = 0;
	if(this->fIsStatic)
	{

		int defaultIdx = -1;
		for(RlcSrcSize i = 0; i < this->fCaseCount; i++)
		{
			if(this->fCases[i].fIsDefault)
			{
				RLC_ASSERT(defaultIdx == -1
					&& "duplicate default case in static type switch");

				hasDefault = 1;
				defaultIdx = i;
				continue;
			}

			if(i) fputs(" else ", out);
			fputs("if constexpr(", out);
			for(RlcSrcIndex j = 0; j<this->fCases[i].fTypeNameCount; j++)
			{
				if(j) fputs("\n|| ", out);
				fputs("::std::is_same<::std::decay_t<decltype(__rl_type_switch_var)>, ", out);
				rlc_parsed_symbol_print(&this->fCases[i].fTypeNames[j], file, out);
				fputs(">()", out);
			}
			fputs(")\n", out);

			rlc_parsed_statement_print(this->fCases[i].fBodyStatement, file, out);
		}

		if(defaultIdx != -1)
		{
			if(this->fCaseCount > 1)
				fputs(" else\n", out);

			rlc_parsed_statement_print(this->fCases[defaultIdx].fBodyStatement, file, out);
		}
	} else
	{
		fputs("switch(__rl::deriving_type_number(__rl_type_switch_var))\n{", out);
		for(RlcSrcSize i = 0; i < this->fCaseCount; i++)
		{
			if(this->fCases[i].fIsDefault)
			{
				hasDefault = 1;
				fputs("default:", out);
			}
			else for(RlcSrcIndex j = 0; j<this->fCases[i].fTypeNameCount; j++)
			{
				fputs("case __rl::type_number<", out);
				if(this->fCases[i].fAddTypeName[j])
					fputs("typename ", out);
				rlc_parsed_symbol_print(&this->fCases[i].fTypeNames[j], file, out);
				fputs(">():", out);
			}

			rlc_parsed_statement_print(this->fCases[i].fBodyStatement, file, out);
			fputs("break;\n", out);
		}
	}

	if(!hasDefault && this->fIsStrict)
	{
		if(this->fIsStatic && this->fCaseCount > 1)
			fputs(" else\n", out);
		else if(!this->fIsStatic)
			fputs("default: ", out);

		struct RlcSrcString exp = this->fExpression->fStart.content;
		struct RlcSrcString end = this->fExpression->fEnd.content;
		exp.length = end.start + end.length - exp.start;

		fprintf(out, "throw "
			"__rl::Tuple<char const *, char const *>(__rl::createTuple, "
			"\"%s:%u:%u: value \"",
			file->fName,
			exp.line,
			exp.column);

		fputs(" __rl_assert_stringify_code(", out);
		rlc_src_string_print(&exp, file, out);
		fprintf(out,
			") \" not covered in strict type switch (consider TYPE SWITCH%s?(...))\""
			", __rl::%stype_name(__rl_type_switch_var));\n",
			this->fIsStatic ? " STATIC":"",
			this->fIsStatic ? "" : "deriving_");
	}

	if(!this->fIsStatic)
		fputs("}", out);
	fputs("}\n", out);
}