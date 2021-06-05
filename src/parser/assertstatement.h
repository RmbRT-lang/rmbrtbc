#ifndef __rlc_parser_assertstatement_h_defined
#define __rlc_parser_assertstatement_h_defined

#include "statement.h"
#include "../macros.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedAssertStatement
{
	RLC_DERIVE(struct,RlcParsedStatement);

	struct RlcParsedExpression * fAssertion;
};

void rlc_parsed_assert_statement_create(
	struct RlcParsedAssertStatement * this);

void rlc_parsed_assert_statement_destroy(
	struct RlcParsedAssertStatement * this);

_Nodiscard int rlc_parsed_assert_statement_parse(
	struct RlcParsedAssertStatement * out,
	struct RlcParser * parser);

void rlc_parsed_assert_statement_print(
	struct RlcParsedAssertStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif