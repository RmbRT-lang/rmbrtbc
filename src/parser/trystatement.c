#include "trystatement.h"
#include "../malloc.h"

void rlc_parsed_try_statement_create(
	struct RlcParsedTryStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedTryStatement);

	this->fBody = NULL;
	this->fCatches = NULL;
	this->fCatchCount = 0;
	this->fFinally = NULL;
}

void rlc_parsed_try_statement_destroy(
	struct RlcParsedTryStatement * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fBody)
	{
		rlc_parsed_statement_destroy_virtual(this->fBody);
		rlc_free((void**)&this->fBody);
	}

	if(this->fCatches)
	{
		for(RlcSrcIndex i = 0; i < this->fCatchCount; i++)
			rlc_parsed_catch_statement_destroy(&this->fCatches[i]);
		rlc_free((void**)&this->fCatches);
	}

	if(this->fFinally)
	{
		rlc_parsed_statement_destroy_virtual(this->fFinally);
		rlc_free((void**)&this->fFinally);
	}

	rlc_parsed_statement_destroy_base(RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_try_statement_parse(
	struct RlcParsedTryStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokTry))
		return 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "try statement", &tracer);
	rlc_parsed_try_statement_create(out);

	if(!(out->fBody = rlc_parsed_statement_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedStatementType)
		&~RLC_FLAG(kRlcParsedTryStatement)
		&~RLC_FLAG(kRlcParsedVariableStatement))))
		rlc_parser_fail(parser, "expected statement (not try, variable)");

	struct RlcParsedCatchStatement catch_stmt;
	while(rlc_parsed_catch_statement_parse(&catch_stmt, parser))
	{
		rlc_realloc(
			(void**)&out->fCatches,
			sizeof(struct RlcParsedCatchStatement) * ++out->fCatchCount);
		out->fCatches[out->fCatchCount-1] = catch_stmt;
	}

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokFinally))
	{
		struct RlcParserTracer tracer;
		rlc_parser_trace(parser, "finally clause", &tracer);
		if(!(out->fFinally = rlc_parsed_statement_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedStatementType)
			&~RLC_FLAG(kRlcParsedVariableStatement))))
			rlc_parser_fail(parser, "expected statement (not variable)");
		rlc_parser_untrace(parser, &tracer);
	} else
		out->fFinally = NULL;

	rlc_parser_untrace(parser, &tracer);
	return 1;
}

int rlc_parsed_catch_statement_parse(
	struct RlcParsedCatchStatement * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokCatch))
		return 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "catch clause", &tracer);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseOpen);

	if(!rlc_parsed_variable_parse(
		&out->fException,
		parser,
		NULL,
		0,
		0,
		0,
		0,
		1))
		rlc_parser_fail(parser, "expected variable");

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokParentheseClose);

	if(!(out->fBody = rlc_parsed_statement_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedStatementType)
		&~RLC_FLAG(kRlcParsedTryStatement)
		&~RLC_FLAG(kRlcParsedVariableStatement))))
		rlc_parser_fail(parser, "expected statement (not try, variable)");

	rlc_parser_untrace(parser, &tracer);
	return 1;
}

void rlc_parsed_catch_statement_destroy(
	struct RlcParsedCatchStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_variable_destroy(&this->fException);
	rlc_parsed_statement_destroy_virtual(this->fBody);
	rlc_free((void**)&this->fBody);
}