/** @file test.h
	Contains the definition of the test case type used by the parser. */
#ifndef __rlc_parser_test_h_defined
#define __rlc_parser_test_h_defined

#include "../src/string.h"
#include "blockstatement.h"
#include "parser.h"
#include "scopeentry.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedTest
{
	RLC_DERIVE(struct, RlcParsedScopeEntry);

	struct RlcSrcString fName;
	struct RlcParsedBlockStatement fBody;
};

void rlc_parsed_test_create(
	struct RlcParsedTest * this);
void rlc_parsed_test_destroy(
	struct RlcParsedTest * this);
_Nodiscard int rlc_parsed_test_parse(
	struct RlcParsedTest * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates);

void rlc_parsed_test_print(
	struct RlcParsedTest * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer);

#ifdef __cplusplus
}
#endif

#endif