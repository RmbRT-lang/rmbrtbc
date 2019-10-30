/** @file controllabel.h
	Contains the parsing code for control labels. */
#ifndef __rlc_parser_controllabel_h_defined
#define __rlc_parser_controllabel_h_defined

#include "parser.h"

#include "../tokeniser/tokens.h"
#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A control label. */
struct RlcControlLabel
{
	int fExists;
	struct RlcToken fLabel;
};

/** Parses a control label.
	Does not produce an error if there is no control label.
@related RlcParserData
@param[out] out:
	The control label's identifier token.
	If there was no label, it is null.
	@dassert @nonnull
@param[in,out] parser:
	The parser data. */
void rlc_control_label_parse(
	struct RlcControlLabel * out,
	struct RlcParser * parser);


#ifdef __cplusplus
}
#endif

#endif