/** @file unicode.h Contains functions for unicode string manipulation. */
#ifndef __rlc_unicode_h_defined
#define __rlc_unicode_h_defined

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t rlc_utf8_t;
typedef uint32_t rlc_char_t;

/** Checks whether the given UTF-8 string is valid.
	@return 0 if invalid, otherwise count of characters + 1 (not byte count).*/
size_t rlc_utf8_is_valid_string(rlc_utf8_t const * str);
size_t rlc_strlen(rlc_char_t const * str);
int rlc_strcmp(rlc_char_t const * a, rlc_char_t const * b);
int rlc_strcmp_utf8(rlc_char_t const * a, rlc_utf8_t const * b);
int rlc_strncmp(rlc_char_t const * a, rlc_char_t const * b, size_t len);
int rlc_strncmp_utf8(rlc_char_t const * a, rlc_utf8_t const * b, size_t len);

rlc_char_t * rlc_utf8_to_utf32(rlc_utf8_t const * str);
rlc_utf8_t * rlc_utf32_to_utf8(rlc_char_t const * str);

#ifdef __cplusplus
}
#endif

#endif