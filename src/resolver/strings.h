/** @file strings.h
	Contains the utilities to retrieve the string contents of strings and character constants. */
#ifndef __rlc_resolver_strings_h_defined
#define __rlc_resolver_strings_h_defined

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

/** A string's contents. */
struct RlcResolvedString
{
	/** The string's characters as in the source code. */
	rlc_char_t * fSource;
	/** The string's raw data. */
	char const * fRaw;
	/** The string as utf8. */
	char const * fUtf8;
	/** The string's byte count. */
	size_t fUtf8Length;
	/** The string's symbol count. */
	size_t fSymbols;
	/** The string's character size, in bytes. */
	int fSymbolSize;
	/** The string's endianness. */
	enum RlcEndian fEndian;
};

#ifdef __cplusplus
}
#endif


#endif