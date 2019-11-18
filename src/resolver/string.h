/** @file strings.h
	Contains the utilities to retrieve the string contents of strings and character constants. */
#ifndef __rlc_resolver_strings_h_defined
#define __rlc_resolver_strings_h_defined

#include "../src/string.h"
#include "../tokeniser/tokens.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Endianness values. */
enum RlcEndian
{
	kRlcEndianNative,
	kRlcEndianLittle,
	kRlcEndianBig
};

/** A string's or character literal's contents. */
struct RlcResolvedText
{
	/** Whether the string is a string or a character literal. */
	int fIsString;
	/** The string's raw byte representation, excluding 0-terminator. */
	void const * fRaw;
	/** The element count (fSymbolSize) in the string, excluding 0-terminator. */
	RlcSrcSize fElements;
	/** The string's character size, in bytes. */
	uint8_t fSymbolSize;
	/** The string's endianness. */
	enum RlcEndian fEndian;
};

void rlc_resolve_text(
	struct RlcResolvedText * out,
	struct RlcSrcFile const * file,
	struct RlcToken const * token);

void rlc_resolved_text_destroy(
	struct RlcResolvedText * this);

#ifdef __cplusplus
}
#endif


#endif