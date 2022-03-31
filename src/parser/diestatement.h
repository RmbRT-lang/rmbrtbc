#ifndef __rlc_parser_diestatement_h_defined
#define __rlc_parser_diestatement_h_defined

#include "statement.h"
#include "../macros.h"
#include "parser.h"
#include "stringexpression.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedDieStatement
{
	RLC_DERIVE(struct,RlcParsedStatement);

	struct RlcSrcPosition fPosition;
	int fHasMessage;
	struct RlcParsedStringExpression fMessage;
};

void rlc_parsed_die_statement_create(
	struct RlcParsedDieStatement * this);

void rlc_parsed_die_statement_destroy(
	struct RlcParsedDieStatement * this);

_Nodiscard int rlc_parsed_die_statement_parse(
	struct RlcParsedDieStatement * out,
	struct RlcParser * parser);

void rlc_parsed_die_statement_print(
	struct RlcParsedDieStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif