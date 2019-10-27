/** @file includestatement.h
	Contains the include statement type as used by the parser. */
#ifndef __rlc_parser_includestatment_h_defined
#define __rlc_parser_includestatment_h_defined

#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Include statement as used by the parser. */
struct RlcParsedIncludeStatement
{
	/** Whether the filepath is relative to the current file. */
	int fIsRelative;
	/** The file name to include. */
	struct RlcToken fFileName;
};

/** Parses an include statement.
@memberof RlcIncludeStatement
@param[out] out:
	The include statement to parse.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@return
	Whether the parsing succeeded. */
int rlc_parsed_include_statement_parse(
	struct RlcParsedIncludeStatement * out,
	struct RlcParser * parser);

#ifdef __cplusplus
}
#endif


#endif