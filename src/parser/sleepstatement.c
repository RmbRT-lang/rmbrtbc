#include "sleepstatement.h"
#include "expression.h"
#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_sleep_statement_create(
	struct RlcParsedSleepStatement * this)
{
	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedSleepStatement);

	this->fDuration = NULL;
}

void rlc_parsed_sleep_statement_destroy(
	struct RlcParsedSleepStatement * this)
{
	if(this->fDuration)
	{
		rlc_parsed_expression_destroy_virtual(this->fDuration);
		rlc_free((void**)&this->fDuration);
	}

	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

_Nodiscard int rlc_parsed_sleep_statement_parse(
	struct RlcParsedSleepStatement * out,
	struct RlcParser * parser)
{
	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokTripleDot))
		return 0;

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "sleep", &tracer);

	rlc_parsed_sleep_statement_create(out);

	out->fDuration = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType));

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);
	rlc_parser_untrace(parser, &tracer);
	return 1;
}

void rlc_parsed_sleep_statement_print(
	struct RlcParsedSleepStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	fputs("__rl_sleep(", out);
	if(this->fDuration)
		rlc_parsed_expression_print(this->fDuration, file, out);
	fputs(");\n", out);
}