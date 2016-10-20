/** @file preprocessor.h
	Contains the preprocessor declarations. */
#ifndef __rlc_preprocessor_preprocessor_h_defined
#define __rlc_preprocessor_preprocessor_h_defined

#include "../unicode.h"
#include "../tokeniser/tokens.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Represents a file that has been preprocessed. */
struct RlcPreprocessedFile
{
	/** The tokenised file that was preprocessed. */
	struct RlcFile const * fBaseFile;

	/** The new token stream.
		Points to tokens held by fBaseFile. */
	struct RlcToken const ** fTokens;
	/** The new token count. */
	size_t fTokenCount;
};

/** Represents a preprocessor symbol. */
struct RlcPreprocessorSymbol
{
	/** The preprocessor symbol's name. */
	char const * fName;
	/** The preprocessor symbol's content. */
	char const * fValue;
};

/** Stores a symbol table of defines / macros. */
struct RlcPreprocessorSymbols
{
	/** Dummy field. */
	int fUnused;
};

/** Preprocesses a tokenised file.
@memberof RlcPreprocessedFile

@param[in] source:
	The tokenised file.
@param[in] input:
	The pre-defined preprocessor symbols.
@param[out] out:
	The preprocessed file.
@return
	0 on error, non-zero otherwise. */
int rlc_preprocess(
	struct RlcFile const * source,
	struct RlcPreprocessorSymbols const * input,
	struct RlcPreprocessedFile * out);

/** Destroys a preprocessed file.
@memberof RlcPreprocessedFile

@param[in,out] this:
	The preprocessed file to destroy.*/
void rlc_preprocessed_file_destroy(
	struct RlcPreprocessedFile * this);

#ifdef __cplusplus
}
#endif

#endif