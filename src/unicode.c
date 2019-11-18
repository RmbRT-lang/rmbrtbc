#include "unicode.h"
#include "malloc.h"
#include "assert.h"
#include <stdio.h>

inline int rlc_is_ascii(rlc_utf8_t character);
inline int rlc_is_utf8_group_2_start(rlc_utf8_t character);
inline int rlc_is_utf8_group_3_start(rlc_utf8_t character);
inline int rlc_is_utf8_group_4_start(rlc_utf8_t character);
inline int rlc_is_utf8_group_start(rlc_utf8_t character);
inline int rlc_is_utf8_follow(rlc_utf8_t character);

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
	return (rlc_utf8_t)(character & (rlc_utf8_t)0xc0) == (rlc_utf8_t)0x80;
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
	RLC_DASSERT(character != NULL);

	if(!rlc_is_utf8_valid(*character))
		return 0;

	unsigned length = rlc_character_length(*character);
	if(!length)
		return 0;

	for(unsigned i = 0; i<length-1; i++)
	{
		if(!rlc_is_utf8_follow(character[i+1]))
			return 0;
	}
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
	else RLC_ASSERT(!"invalid utf-8 character!");
}

int rlc_utf8_char_to_utf32_char(
	rlc_utf8_t const * str,
	rlc_utf32_t * out)
{
	RLC_DASSERT(str != NULL);
	RLC_DASSERT(out != NULL);

	if(!rlc_is_utf8_valid_seq(str))
		return 0;

	unsigned len = rlc_character_length(*str);
	rlc_utf8_t temp[4] = {0, 0, 0, 0};
	// copy the string in reverse byte order into the buffer.
	// now the first bytes are follow bytes, and the last nonzero byte is the group start byte.
	for(unsigned i = 0; i<len; i++)
		temp[i] = str[len-1-i];
	// mapping byte index => used bits.
	static rlc_utf8_t const bitmasks[5] = { 0x3f, 0x7f, 0x1f, 0x0f, 0x07 };
	static uint8_t const bitmask_count[5] = { 6, 7, 5, 4, 3 };
	uint8_t bits = 0;
	*out = 0;

	for(uint8_t i = 0; i < len; i++)
	{
		uint8_t index = i < len-1 ? 0 : len;
		rlc_utf32_t bitstowrite_mask = (rlc_utf32_t)bitmasks[index] << bits;

		*out |= bitstowrite_mask & ((rlc_utf32_t)temp[i] << bits);
		bits += bitmask_count[i];
	}

	return len;
}

int rlc_utf8_char_to_utf16_char(
	rlc_utf8_t const * str,
	struct RlcUtf16Buf * out)
{
	RLC_DASSERT(str != NULL);
	RLC_DASSERT(out != NULL);

	rlc_utf32_t u32;
	if(!rlc_utf8_char_to_utf32_char(str, &u32))
		return 0;

	if(u32 >= (1<<20))
		return 0;

	static rlc_utf16_t const kW1 = 0xD800;
	static rlc_utf16_t const kW2 = 0xDC00;
	static rlc_utf16_t const kMask = (1<<10)-1;

	if(u32 >= (1<<10))
	{
		out->data[0] = kW1 | (u32 >> 10);
		out->data[1] = kW2 | (u32 & kMask);
		out->size = 2;
	} else
	{
		out->data[0] = u32;
		out->data[1] = 0;
		out->size = 1;
	}

	return 1;
}

size_t rlc_utf8_is_valid_string(rlc_utf8_t const * str)
{
	RLC_DASSERT(str != NULL);

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

size_t rlc_strlen(rlc_utf32_t const * str)
{
	RLC_DASSERT(str != NULL);

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

int rlc_strcmp(
	rlc_utf32_t const * a,
	rlc_utf32_t const * b)
{
	RLC_DASSERT(a != NULL);
	RLC_DASSERT(b != NULL);

	size_t i = 0;
	while(a[i] && (a[i] == b[i]))
		++i;
	return sign((signed)a[i] - (signed)b[i]) *  (i + 1);
}

int rlc_strcmp_utf8(
	rlc_utf32_t const * a,
	rlc_utf8_t const * b)
{
	RLC_DASSERT(a != NULL);
	RLC_DASSERT(b != NULL);
	RLC_DASSERT(rlc_utf8_is_valid_string(b));

	size_t i = 0;
	while(a[i])
	{
		rlc_utf32_t conv_b;
		int len = rlc_utf8_char_to_utf32_char(b, &conv_b);

		if(a[i] == conv_b)
			++i;
		else
			return a[i] > conv_b ? i+1 : -i-1;

		b += len;
	}
	return b[i] ? -i : i;
}

int rlc_strncmp(
	rlc_utf32_t const * a,
	rlc_utf32_t const * b,
	size_t n)
{
	RLC_DASSERT(a != NULL);
	RLC_DASSERT(b != NULL);

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

int rlc_strncmp_utf8(
	rlc_utf32_t const * a,
	rlc_utf8_t const * b,
	size_t len)
{
	RLC_DASSERT(a != NULL);
	RLC_DASSERT(b != NULL);
	RLC_DASSERT(rlc_utf8_is_valid_string(b));

	size_t i = 0;
	while(i<len)
	{
		rlc_utf32_t conv_b;
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

rlc_utf32_t * rlc_utf8_to_utf32(
	rlc_utf8_t const * str,
	size_t len,
	size_t * out_len)
{
	RLC_DASSERT(str != NULL);
	RLC_DASSERT(out_len != NULL);

	*out_len = 0;
	{
		rlc_utf8_t const * it = str;
		for(size_t i = 0; i < len; i++)
		{
			it += rlc_character_length(*it);
			++*out_len;
		}
	}

	rlc_utf32_t * buffer = 0, * ret;
	rlc_malloc(
		(void**)&buffer,
		sizeof(rlc_utf32_t) * (*out_len+1));
	ret = buffer;
	while(len--)
		str += rlc_utf8_char_to_utf32_char(str, buffer++);
	*buffer = '\0';

	return ret;
}

rlc_utf16_t * rlc_utf8_to_utf16(
	rlc_utf8_t const * str)
{
	size_t len;
	if(!(len = rlc_utf8_is_valid_string(str)))
		return 0;
	rlc_utf16_t * buffer = NULL, * ret;
	rlc_malloc((void**)&buffer, len-- * sizeof(rlc_utf16_t));
	ret = buffer;
	while(len--)
	{
		struct RlcUtf16Buf ch;
		str += rlc_utf8_char_to_utf16_char(str, &ch);
		for(unsigned i = 0; i < ch.size; i++)
			buffer[i] = ch.data[i];
		buffer += ch.size;
	}
	*buffer = 0;

	return ret;
}

rlc_utf8_t * rlc_utf32_to_utf8(
	rlc_utf32_t const * str)
{
}