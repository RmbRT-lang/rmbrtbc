/** @file unicode.h
	Contains functions for unicode string manipulation. */
#ifndef __rlc_unicode_h_defined
#define __rlc_unicode_h_defined

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** UTF-8 character type. */
typedef uint8_t rlc_utf8_t;
/** UTF-16 Character type. */
typedef uint16_t rlc_utf16_t;
/** UTF-32 character type. */
typedef uint32_t rlc_utf32_t;

/** Holds a single UTF-16 character. */
struct RlcUtf16Buf
{
	rlc_utf16_t data[2];
	unsigned size;
};

/** Checks whether the given UTF-8 string is valid.
@return
	0 if invalid, otherwise count of characters + 1 (not byte count). */
size_t rlc_utf8_is_valid_string(
	rlc_utf8_t const * str);
/** Returns the length of a string.
@param[in] str:
	The string whose length to return.
@return
	The index of the 0-character that terminates the string. */
size_t rlc_strlen(
	rlc_utf32_t const * str);
/** Compares two strings.
@param[in] a:
	The first string.
@param[in] b:
	The second string.
@return
	0, if the strings are equal, otherwise ```sign(a[i] - b[i]) * (i + 1)```, where ```i``` is the index of the first character that did not match. */
int rlc_strcmp(
	rlc_utf32_t const * a,
	rlc_utf32_t const * b);
/** Compares two strings.
	The UTF-8 string is converted into UTF-32 and then compared to the first string.
@param[in] a:
	The first string.
@param[in] b:
	The second string. This string has to be a valid UTF-8 string.
@return
	0, if the strings are equal, otherwise ```sign(a[i] - b[i]) * (i + 1)``` where ```ì``` is the index of the first character in the first string that did not match. */
int rlc_strcmp_utf8(
	rlc_utf32_t const * a,
	rlc_utf8_t const * b);
/** Compares two strings.
@param[in] a:
	The first string.
@param[in] b:
	The second string.
@param[in] len:
	The maximum amount of characters to compare.
@return
	0, if the strings are equal, otherwise ```sign(a[i] - b[i]) * (i + 1)``` where ```ì``` is the index of the first character in the first string that did not match. */
int rlc_strncmp(
	rlc_utf32_t const * a,
	rlc_utf32_t const * b,
	size_t len);
/** Compares two strings.
	The second string is converted to UTF-32 and then compared to the first string.
@param[in] a:
	The first string.
@param[in] b:
	The second string. This string has to be a valid UTF-8 string.
@param[in] len:
	The maximum character count in `a` to compare.
@return
	0, if the strings are equal, otherwise `sign(a[i] - b[i]) * (i + 1)` where `i` is the index of the first character in the first string that did not match. */
int rlc_strncmp_utf8(
	rlc_utf32_t const * a,
	rlc_utf8_t const * b,
	size_t len);

/** Converts an UTF-8 string to an UTF-32 string.
	Fails if the given string is not a valid UTF-8 string.
@param[in] str:
	The string to convert.
@return
	The converted string, if successful, otherwise null. The returned pointer has to be released by the caller. */
rlc_utf32_t * rlc_utf8_to_utf32(
	rlc_utf8_t const * str,
	size_t * out_len);
/** Converts an UTF-8 string to an UTF-16 string.
	Fails if the given string is not a valid UTF-8 string.
@param[in] str:
	The string to convert.
@return
	The converted string, if successful, otherwise null. The returned pointer has to be released by the caller. */
rlc_utf16_t * rlc_utf8_to_utf16(
	rlc_utf8_t const * str,
	size_t * out_len);
/** Converts an UTF-32 string to an UTF-8 string.
@param[in] str:
	The string to convert.
@return
	The converted string, if successful, otherwise null. */
rlc_utf8_t * rlc_utf32_to_utf8(
	rlc_utf32_t const * str,
	size_t length,
	size_t * out_len);
/** Converts an UTF-32 string to an UTF-16 string.
	Fails if the given string is not a valid UTF-32 string.
@param[in] str:
	The string to convert.
@return
	The converted string, if successful, otherwise null. The returned pointer has to be released by the caller. */
rlc_utf16_t * rlc_utf32_to_utf16(
	rlc_utf32_t const * str,
	size_t length,
	size_t * out_len);

/** Checks whether a byte is a valid UTF-8 character. */
int rlc_is_utf8_valid(rlc_utf8_t character);
/** Checks whether a sequence of bytes forms a valid UTF-8 character. */
int rlc_is_utf8_valid_seq(rlc_utf8_t const * character);
/** Retrieves the byte size of a UTF-8 character. */
unsigned rlc_character_length(rlc_utf8_t character);
/** Converts a UTF-8 character to a UTF-16 character. */
int rlc_utf8_char_to_utf16_char(rlc_utf8_t const * str, struct RlcUtf16Buf * out);
/** Converts a UTF-8 character to a UTF-32 character. */
int rlc_utf8_char_to_utf32_char(rlc_utf8_t const * str, rlc_utf32_t * out);

#ifdef __cplusplus
}
#endif

#endif