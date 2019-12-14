/** @file strings.h
	Contains the utilities to retrieve the string contents of strings and character constants. */
#ifndef __rlc_scoper_strings_h_defined
#define __rlc_scoper_strings_h_defined

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
struct RlcScopedText
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

void rlc_scoped_text_create(
	struct RlcScopedText * out,
	struct RlcSrcFile const * file,
	struct RlcToken const * token);

void rlc_scoped_text_destroy(
	struct RlcScopedText * this);

#ifdef __cplusplus
}
#endif


#endif