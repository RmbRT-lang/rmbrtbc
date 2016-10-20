#ifndef __rlc_chartype_h_defined
#define __rlc_chartype_h_defined

#include "unicode.h"

#ifdef __cplusplus
extern "C" {
#endif

int rlc_is_ident_first_char(rlc_char_t character);
int rlc_is_ident_last_char(rlc_char_t character);
int rlc_is_whitespace(rlc_char_t character);
int rlc_is_decimal(rlc_char_t character);
int rlc_is_octal(rlc_char_t character);
int rlc_is_hex(rlc_char_t character);

#ifdef __cplusplus
}
#endif

#endif