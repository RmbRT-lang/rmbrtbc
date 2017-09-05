#include "chartype.h"
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

#define CHARS_LOWER "abcdefghijklmnopqrstuvwxyz"
#define CHARS_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHARS_ALPHA CHARS_LOWER CHARS_UPPER
#define CHARS_DECIMAL "0123456789"
#define CHARS_ALNUM CHARS_ALPHA CHARS_DECIMAL
#define CHARS_OCTAL "01234567"
#define CHARS_HEX "0123456789aAbBcCdDeEfF"
#define CHARS_IDENT_FC CHARS_ALPHA
#define CHARS_IDENT_LC CHARS_ALNUM "_"
#define CHARS_WHITESPACE "\t\r\n" " "
#define CHARS_SPECIAL "^!$%&/()=?{[]}+*~<>|,;.:-\'\"\\"

typedef uint16_t bitmask_t;

enum
{
	ID_FC,
	ID_LC,
	WS,
	DEC,
	OCT,
	HEX,

	CT_COUNT
};

static void rlc_set_buffer(bitmask_t * buffer, unsigned bit, rlc_utf8_t const * trues)
{
	assert(bit < CT_COUNT);

	bitmask_t const set = 1 << bit;
	bitmask_t const clear = ~set;
	for(unsigned i = 256; i--;)
		buffer[i] &= clear;

	while(*trues)
	{
		buffer[*trues++] |= set;
	}
}

static inline int rlc_is_char_type(rlc_utf8_t character, unsigned type)
{
	assert(type < CT_COUNT);

	static bitmask_t s_types[256];
	static int s_inited = 0;


	if(!s_inited)
	{
		for(uint8_t i = 255; i--;)
			s_types[i] = 0;
		rlc_set_buffer(s_types, ID_FC, CHARS_IDENT_FC);
		rlc_set_buffer(s_types, ID_LC, CHARS_IDENT_LC);
		rlc_set_buffer(s_types, WS, CHARS_WHITESPACE);
		rlc_set_buffer(s_types, DEC, CHARS_DECIMAL);
		rlc_set_buffer(s_types, OCT, CHARS_OCTAL);
		rlc_set_buffer(s_types, HEX, CHARS_HEX);
		s_inited = 1;
	}

	return (s_types[character] & (bitmask_t)(1 << type)) != 0;
}

int rlc_is_ident_first_char(rlc_char_t character)
{
	return character > 0x7f || rlc_is_char_type(character, ID_FC);
}

int rlc_is_ident_last_char(rlc_char_t character)
{
	return character > 0x7f || rlc_is_char_type(character, ID_LC);
}

int rlc_is_whitespace(rlc_char_t character)
{
	return character <= 0x7f && rlc_is_char_type(character, WS);
}

int rlc_is_decimal(rlc_char_t character)
{
	return character <= 0x7f && rlc_is_char_type(character, DEC);
}

int rlc_is_octal(rlc_char_t character)
{
	return character <= 0x7f && rlc_is_char_type(character, OCT);
}

int rlc_is_hex(rlc_char_t character)
{
	return character <= 0x7f && rlc_is_char_type(character, HEX);
}