#include "number.h"
#include "../resolver/resolver.h"
#include "../assert.h"
#include <string.h>

static int hexdigit(char c)
{
	if(c >= 'A' && c <= 'F')
		return c - ('A' - 0xa);
	else if(c >= 'a' && c <= 'f')
		return c - ('a' - 0xa);
	else
		return c - '0';
}

inline static int is_decimal(char c)
{
	return (unsigned char)(c -'0') < 10;
}
inline static int is_octal(char c)
{
	return (unsigned char)(c -'0') < 8;
}
inline static int is_binary(char c)
{
	return (unsigned char)(c -'0') < 2;
}
static int is_hexadecimal(char c)
{
	return (unsigned char)(c - 'a') < 6
		|| (unsigned char)(c - 'A') < 6
		|| is_decimal(c);
}

static char const * atoi_n(
	struct RlcNumber * out,
	char const * number,
	size_t length)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(number != NULL);
	RLC_DASSERT(length != 0);

	out->value = 0;

	// Decimal number?
	if(number[0] != '0')
	{
		if(length > 20
		|| (length == 20 && strncmp(number, "18446744073709551615", 20) > 0))
			return "decimal number too large";

		int64_t multiplier = 1;
		for(size_t i = 0; i < length; i++)
		{
			char c = number[length-1-i];
			RLC_DASSERT(is_decimal(c));

			out->value += multiplier * (c - '0');
			multiplier *= 10;
		}

		return NULL;
	}

	// Zero?
	if(length == 1)
		return NULL;

	switch(number[1])
	{
	// Hexadecimal number.
	case 'x':
	case 'X':
		{
			if(length-2 > 16)
				return "hexadecimal number too large";

			for(size_t i = 0; i < length-2; i++)
			{
				char c = number[length-1-i];
				RLC_DASSERT(is_hexadecimal(c));

				out->value |= (int64_t)hexdigit(c) << (4 * i);
			}
		} break;
	// Binary number.
	case 'b':
	case 'B':
		{
			if(length-2 > 64)
				return "binary number too large";

			for(size_t i = 0; i < length-2; i++)
			{
				char c = number[length-1-i];
				RLC_DASSERT(is_binary(c));

				out->value |= (int64_t)(c - '0') << i;
			}
		} break;
	// Octal number.
	default:
		{
			if(length-1 > 22
			|| (length-1 == 22 && strncmp(number+1, "1777777777777777777777", 22) > 0))
				return "octal number too large";

			for(size_t i = 0; i < length-1; i++)
			{
				char c = number[length-1-i];
				RLC_DASSERT(is_octal(c));

				out->value |= (c - '0') << (i * 3);
			}
		} break;
	}

	return NULL;
}

void rlc_number_from_token(
	struct RlcNumber * this,
	struct RlcSrcFile const * file,
	struct RlcToken const * token)
{
	RLC_DASSERT(token->type == kRlcTokNumberLiteral);

	char const * error = atoi_n(
		this,
		&file->fContents[token->content.start],
		token->content.length);

	if(error)
		rlc_resolver_fail(
			&token->content,
			file,
			"could not parse number: %s",
			error);
}

void rlc_number_add(
	struct RlcNumber * dst,
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs,
	struct RlcResolverFailContext const * context)
{
	RLC_DASSERT(dst != NULL);
	RLC_DASSERT(lhs != NULL);
	RLC_DASSERT(rhs != NULL);
	RLC_DASSERT(context != NULL);

	if(__builtin_add_overflow(lhs->value, rhs->value, &dst->value))
		rlc_resolver_fail_ctx(
			context,
			"Overflow: %llu + %llu",
			lhs->value,
			rhs->value);
}

void rlc_number_sub(
	struct RlcNumber * dst,
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs,
	struct RlcResolverFailContext const * context)
{
	RLC_DASSERT(dst != NULL);
	RLC_DASSERT(lhs != NULL);
	RLC_DASSERT(rhs != NULL);
	RLC_DASSERT(context != NULL);

	if(__builtin_sub_overflow(lhs->value, rhs->value, &dst->value))
		rlc_resolver_fail_ctx(
			context,
			"Overflow: %lld - %lld",
			lhs->value,
			rhs->value);
}

void rlc_number_mul(
	struct RlcNumber * dst,
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs,
	struct RlcResolverFailContext const * context)
{
	RLC_DASSERT(lhs != NULL);
	RLC_DASSERT(rhs != NULL);
	RLC_DASSERT(context != NULL);

	if(__builtin_mul_overflow(lhs->value, rhs->value, &dst->value))
		rlc_resolver_fail_ctx(
			context,
			"Overflow: %lld * %lld",
			lhs->value,
			rhs->value);
}

void rlc_number_div(
	struct RlcNumber * quo,
	struct RlcNumber * rem,
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs,
	struct RlcResolverFailContext const * context)
{
	RLC_DASSERT(quo != NULL || rem != NULL);
	RLC_DASSERT(lhs != NULL);
	RLC_DASSERT(rhs != NULL);
	RLC_DASSERT(context != NULL);

	if(rhs->value == 0)
		rlc_resolver_fail_ctx(context, "%llu / 0", lhs->value);

	if(quo)
		quo->value = lhs->value / rhs->value;
	if(rem)
		rem->value = lhs->value % rhs->value;
}

void rlc_number_neg(
	struct RlcNumber * dst,
	struct RlcNumber const * src,
	struct RlcResolverFailContext const * context)
{
	RLC_DASSERT(dst != NULL);
	RLC_DASSERT(src != NULL);

	int64_t v = src->value;
	if(__builtin_sub_overflow(0, v, &dst->value))
		rlc_resolver_fail_ctx(
			context,
			"Overflow: -%lld",
			v);
}

int rlc_number_cmp(
	struct RlcNumber const * lhs,
	struct RlcNumber const * rhs)
{
	RLC_DASSERT(lhs != NULL);
	RLC_DASSERT(rhs != NULL);

	return lhs->value - rhs->value;
}

void rlc_number_fprint(
	struct RlcNumber const * this,
	FILE * file)
{
	fprintf(file, "%ld", this->value);
}