#ifndef __rlc_parser_sleepstatement_h_defined
#define __rlc_parser_sleepstatement_h_defined

#include "statement.h"
#include "expression.h"
#include "../macros.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedSleepStatement
{
	RLC_DERIVE(struct,RlcParsedStatement);
	struct RlcParsedExpression * fDuration;
};

void rlc_parsed_sleep_statement_create(
	struct RlcParsedSleepStatement * this);

void rlc_parsed_sleep_statement_destroy(
	struct RlcParsedSleepStatement * this);

_Nodiscard int rlc_parsed_sleep_statement_parse(
	struct RlcParsedSleepStatement * out,
	struct RlcParser * parser);

void rlc_parsed_sleep_statement_print(
	struct RlcParsedSleepStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif