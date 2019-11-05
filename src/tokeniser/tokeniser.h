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

#ifdef __cplusplus
}
#endif

#endif