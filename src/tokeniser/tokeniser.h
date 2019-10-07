/** @file tokeniser.h
	Contains the functions used to tokenise a source file and
	perform a lexical analysis. */

#ifndef __rlc_tokeniser_h_defined
#define __rlc_tokeniser_h_defined

#include "../src/file.h"
#include "tokens.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** The tokeniser state. */
struct RlcTokeniser
{
	/** The source file that is being tokenised. */
	struct RlcSrcFile const * fSource;
	/** The tokeniser's current index in the source file. */
	RlcSrcIndex fIndex;
	/** The start of the current token. */
	RlcSrcIndex fStart;
	/** The current token's type. */
	enum RlcTokenType fType;
};

/** Creates a tokeniser for a source file.
@memberof RlcTokeniser
@param[out] this:
	The tokeniser to create.
	@dassert @nonnull
@param[in] file:
	The file to tokenise.
	@dassert @nonnull */
void rlc_tokeniser_create(
	struct RlcTokeniser * this,
	struct RlcSrcFile const * file);

/** Reads a single token.
	Aborts if there is an invalid token or no token.
@memberof RlcTokeniser
@param[in,out] this:
	The tokeniser.
	@dassert @nonnull
@param[out] token:
	The token to read into.
@return
	Whether there is another token. */
int rlc_tokeniser_read(
	struct RlcTokeniser * this,
	struct RlcToken * token);

_Noreturn void rlc_tokeniser_tok_error(
	char const * message);

/** Reads one token in the given source, beginning at the given offset.
@param[in] src:
	The source code to be tokenised.
@param[in] offset:
	The offset in src to begin tokenizing at.
@param[out] out:
	The address of the resulting token.
@param[out] error_index:
	The index of the lexing error, on failure.
@return
	Nonzero on success, 0 on failure. */
enum RlcTokResult rlc_next_token(
	rlc_char_t const * src,
	size_t offset,
	struct RlcToken * out,
	size_t * error_index);

/** Parses a character in a string or character literal.
@param[in] src: The source code.
@param[in] index: Where to parse.
@param[out] length: The number of characters in src consumed.
@return
	RL_TOK_RESULT_OK on success, otherwise
	an error constant to be found in rlcTokResult. */
enum RlcTokResult rlc_parse_character(
	rlc_char_t const * src,
	size_t index,
	size_t * length);

#ifdef __cplusplus
}
#endif

#endif