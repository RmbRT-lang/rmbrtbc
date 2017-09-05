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
/** UTF-32 character type.
	This type is used to store the source code. */
typedef uint32_t rlc_char_t;
#define PRINTF_RLC_CHAR "lc"
#define PRINTF_RLC_STR "ls"

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
	rlc_char_t const * str);
/** Compares two strings.
@param[in] a:
	The first string.
@param[in] b:
	The second string.
@return
	0, if the strings are equal, otherwise ```sign(a[i] - b[i]) * (i + 1)```, where ```i``` is the index of the first character that did not match. */
int rlc_strcmp(
	rlc_char_t const * a,
	rlc_char_t const * b);
/** Compares two strings.
	The UTF-8 string is converted into UTF-32 and then compared to the first string.
@param[in] a:
	The first string.
@param[in] b:
	The second string. This string has to be a valid UTF-8 string.
@return
	0, if the strings are equal, otherwise ```sign(a[i] - b[i]) * (i + 1)``` where ```ì``` is the index of the first character in the first string that did not match. */
int rlc_strcmp_utf8(
	rlc_char_t const * a,
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
	rlc_char_t const * a,
	rlc_char_t const * b,
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
	rlc_char_t const * a,
	rlc_utf8_t const * b,
	size_t len);

/** Converts an UTF-8 string to an UTF-32 string.
	Fails if the given string is not a valid UTF-8 string.
@param[in] str:
	The string to convert.
@return
	The converted string, if successful, otherwise null. The returned pointer has to be released by the caller. */
rlc_char_t * rlc_utf8_to_utf32(
	rlc_utf8_t const * str);
/** Converts an UTF-32 string to an UTF-8 string.
@param[in] str:
	The string to convert.
@return
	The converted string, if successful, otherwise null. */
rlc_utf8_t * rlc_utf32_to_utf8(
	rlc_char_t const * str);

#ifdef __cplusplus
}
#endif

#endif