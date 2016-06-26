#ifndef __rlc_preprocessor_preprocessor_h_defined
#define __rlc_preprocessor_preprocessor_h_defined

#include "../unicode.h"
#include "../tokens.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RlcPreprocessedFile
{
	struct RlcFile const * fBaseFile;

	struct RlcToken const ** fTokens;
	size_t fTokenCount;
};

struct RlcPreprocessorSymbols
{
	int fUnused;
};

int rlc_preprocess(
	RlcFile const * source,
	struct RlcPreprocessorSymbols const * input,
	struct RlcPreprocessedFile * out);

void rlc_preprocessed_file_destroy(struct RlcPreprocessedFile * this);

#ifdef __cplusplus
}
#endif

#endif