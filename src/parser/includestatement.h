#ifndef __rlc_parser_includestatment_h_defined
#define __rlc_parser_includestatment_h_defined

#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Include statement. */
struct RlcIncludeStatement
{
	/** Whether the filepath is relative to the current file. */
	size_t fIsRelative;
	/** The file name to include. */
	size_t fFileName;
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
int rlc_include_statement_parse(
	struct RlcIncludeStatement * out,
	struct RlcParserData * parser);

#ifdef __cplusplus
}
#endif


#endif