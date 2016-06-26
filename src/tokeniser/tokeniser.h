/** @file tokeniser.h
	Contains the functions used to tokenise a source file and
	perform a lexical analysis. */

#ifndef __rlc_tokeniser_h_defined
#define __rlc_tokeniser_h_defined

#include "tokens.h"
#include "../unicode.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Reads one token in the given source, beginning at the given offset.
@param[in] src: The RL source code to be tokenised.
@param[in] offset: The offset in src to begin tokenizing at.
@param[out] out: The address of the resulting token.
@return 1 on success, 0 on failure. */
RlcTokResult rlc_next_token(
	rlc_char_t const * src,
	size_t offset,
	struct RlcToken * out,
	size_t * error_index);

/** Reads all tokens in the given source.
@param[in] src: The RL source code to be tokenised.
@param[out] out: The address of a pointer that should hold the tokens.
@param[in] skip_whitespaces: Whether to discard whitespace tokens or not.
@param[out] count: The token count.
@param[out] error_index: The character an error occured at.
@return 1 on success, 0 on failure. */
RlcTokResult rlc_tokenise(
	rlc_char_t const * src,
	struct RlcToken ** out,
	int skip_whitespaces,
	size_t * count,
	size_t * error_index);

/** Parses a character in a string or character literal.
@param[in] src: The source code.
@param[in] index: Where to parse.
@param[out] length: The number of characters in src consumed.
@return
	RL_TOK_RESULT_OK on success, otherwise
	an error constant to be found in rlcTokResult. */
RlcTokResult rlc_parse_character(
	rlc_char_t const * src,
	size_t index,
	size_t * length);

#ifdef __cplusplus
}
#endif

#endif