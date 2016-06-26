#include "unicode.h"
#include "malloc.h"
#include <assert.h>
#include <stdio.h>

int rlc_is_ascii(rlc_utf8_t character);
int rlc_is_utf8_group_2_start(rlc_utf8_t character);
int rlc_is_utf8_group_3_start(rlc_utf8_t character);
int rlc_is_utf8_group_4_start(rlc_utf8_t character);
int rlc_is_utf8_group_start(rlc_utf8_t character);
int rlc_is_utf8_follow(rlc_utf8_t character);
int rlc_is_utf8_valid(rlc_utf8_t character);
int rlc_is_utf8_valid_seq(rlc_utf8_t const * character);
unsigned rlc_character_length(rlc_utf8_t character);
int rlc_utf8_char_to_utf32_char(rlc_utf8_t const * str, rlc_char_t * out);

int rlc_is_ascii(rlc_utf8_t character)
{
	return (rlc_utf8_t)(character & (rlc_utf8_t)0x80) == (rlc_utf8_t)0;
}

int rlc_is_utf8_group_2_start(rlc_utf8_t character)
{
	return (rlc_utf8_t)(character & (rlc_utf8_t)0xe0) == (rlc_utf8_t)0xc0;
}

int rlc_is_utf8_group_3_start(rlc_utf8_t character)
{
	return (rlc_utf8_t)(character & (rlc_utf8_t)0xf0) == (rlc_utf8_t)0xe0;
}

int rlc_is_utf8_group_4_start(rlc_utf8_t character)
{
	return (rlc_utf8_t)(character & (rlc_utf8_t)0xf8) == (rlc_utf8_t)0xf0;
}

int rlc_is_utf8_group_start(rlc_utf8_t character)
{
	return rlc_is_utf8_group_2_start(character)
		|| rlc_is_utf8_group_3_start(character)
		|| rlc_is_utf8_group_4_start(character);
}

int rlc_is_utf8_follow(rlc_utf8_t character)
{
	return character & (rlc_utf8_t)0xc0 == (rlc_utf8_t)0x80;
}

int rlc_is_utf8_valid(rlc_utf8_t character)
{
	return rlc_is_utf8_follow(character)
		|| rlc_is_ascii(character)
		|| rlc_is_utf8_group_2_start(character)
		|| rlc_is_utf8_group_3_start(character)
		|| rlc_is_utf8_group_4_start(character);
}


int rlc_is_utf8_valid_seq(rlc_utf8_t const * character)
{
	assert(character != NULL);
	if(!rlc_is_utf8_valid(*character))
		return 0;

	unsigned length = rlc_character_length(*character);
	if(!length)
		return 0;

	for(unsigned i = 0; i<length-1; i++)
		if(!rlc_is_utf8_follow(character[i+1]))
			return 0;
	return 1;
}

unsigned rlc_character_length(rlc_utf8_t character)
{
	if(rlc_is_utf8_follow(character))
		return 0;
	else if(rlc_is_ascii(character))
		return 1;
	else if(rlc_is_utf8_group_2_start(character))
		return 2;
	else if(rlc_is_utf8_group_3_start(character))
		return 3;
	else if(rlc_is_utf8_group_4_start(character))
		return 4;
	else assert(!"invalid utf-8 character!");
}

int rlc_utf8_char_to_utf32_char(rlc_utf8_t const * str, rlc_char_t * out)
{
	if(!rlc_is_utf8_valid_seq(str))
		return 0;

	unsigned len = rlc_character_length(*str);
	rlc_utf8_t temp[4] = {0, 0, 0, 0};
	// copy the string in reverse byte order into the buffer.
	// now the first bytes are follow bytes, and the last nonzero byte is the group start byte.
	for(unsigned i = 0; i<len; i++)
		temp[i] = str[len-1-i];
	// mapping rlcUtf8CharType => used bits.
	static rlc_utf8_t const bitmasks[5] = { 0x3f, 0x7f, 0x1f, 0x0f, 0x07 };
	static uint8_t const bitmask_count[5] = { 6, 7, 5, 4, 3 };
	uint8_t bits = 0;
	*out = 0;

	for(uint8_t i = 0; i < len; i++)
	{
		uint8_t index = i < len-1 ? 0 : len;
		rlc_char_t bitstowrite_mask = (rlc_char_t)bitmasks[index] << bits;

		*out |= bitstowrite_mask & ((rlc_char_t)temp[i] << bits);
		bits += bitmask_count[i];
	}

	return len;
}

size_t rlc_utf8_is_valid_string(rlc_utf8_t const * str)
{
	size_t len = 0;
	while(*str)
	{
		if(!rlc_is_utf8_valid_seq(str))
		{
			printf("invalid UTF-8 sequence '%.2x%.2x%.2x%.2x\n", str[0], str[1], str[2], str[3]);
			return 0;
		}
		str += rlc_character_length(*str);
		++len;
	}

	return len+1;
}

size_t rlc_strlen(rlc_char_t const * str)
{
	size_t len = 0;
	while(*str++)
		++len;
	return len;
}

signed sign(signed n)
{
	return !n
		? 0
		: (n<0
			? -1
			: +1);
}

int rlc_strcmp(rlc_char_t const * a, rlc_char_t const * b)
{
	size_t i = 0;
	while(a[i] && (a[i] == b[i]))
		++i;
	return sign((signed)a[i] - (signed)b[i]);
}

int rlc_strcmp_utf8(rlc_char_t const * a, rlc_utf8_t const * b)
{
	assert(rlc_utf8_is_valid_string(b));

	size_t i = 0;
	while(a[i])
	{
		rlc_char_t conv_b;
		int len = rlc_utf8_char_to_utf32_char(b, &conv_b);

		if(a[i] == conv_b)
			++i;
		else
			return a[i] > conv_b ? i+1 : -i-1;

		b += len;
	}
}

int rlc_strncmp(rlc_char_t const * a, rlc_char_t const * b, size_t n)
{
	assert(a && b);
	size_t i = 0;
	while(i<n)
	{
		if(!a[i])
		{
			return !b[i] ? 0 : -i-1;
		}
		else
		{
			if(a[i] != b[i])
				return a[i] > b[i] ? i+1 : -i-1;
		}
		++i;
	}

	return 0;
}

int rlc_strncmp_utf8(rlc_char_t const * a, rlc_utf8_t const * b, size_t n)
{
	assert(rlc_utf8_is_valid_string(b));

	size_t i = 0;
	while(i<n)
	{
		rlc_char_t conv_b;
		int len = rlc_utf8_char_to_utf32_char(b, &conv_b);
		if(!a[i])
		{
			return !conv_b ? 0 : -i-1;
		}
		else
			if(a[i] != conv_b)
				return a[i] > conv_b ? i+1 : -i-1;
		++i;
		b += len;
	}

	return 0;
}

rlc_char_t * rlc_utf8_to_utf32(rlc_utf8_t const * str)
{
	size_t len;
	if(!(len = rlc_utf8_is_valid_string(str)))
		return 0;

	rlc_char_t * buffer = 0, * ret;
	rlc_malloc((void**)&buffer, len-- * sizeof(rlc_char_t));
	ret = buffer;
	while(len--)
	{
		str += rlc_utf8_char_to_utf32_char(str, buffer++);
	}
	*buffer = 0;

	return ret;
}