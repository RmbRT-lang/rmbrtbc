#include "stringexpression.h"
#include "../parser/stringexpression.h"
#include "../resolver/resolver.h"
#include "../malloc.h"

#include <string.h>

#define kRlcScopedStringExpression kRlcParsedStringExpression

struct RlcScopedStringExpression * rlc_scoped_string_expression_new(
	struct RlcParsedStringExpression * parsed,
	struct RlcSrcFile * file)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	struct RlcScopedStringExpression * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcScopedStringExpression));

	rlc_scoped_expression_create(
		RLC_BASE_CAST(ret, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedStringExpression);

	rlc_scoped_text_create(
		&ret->fValue,
		file,
		&parsed->fTokens[0]);
	RLC_DASSERT(ret->fValue.fIsString);

	for(RlcSrcIndex i = 1; i < parsed->fTokenCount; i++)
	{
		struct RlcScopedText rhs;
		rlc_scoped_text_create(
			&rhs,
			file,
			&parsed->fTokens[i]);
		RLC_DASSERT(rhs.fIsString);

		if(ret->fValue.fSymbolSize != rhs.fSymbolSize
		|| ret->fValue.fEndian != rhs.fEndian)
		{
			rlc_resolver_fail(
				&parsed->fTokens[i],
				file,
				"compound string literal has inconsistent size or endianness");
		}

		rlc_realloc(
			(void **)&ret->fValue.fRaw,
			(ret->fValue.fElements + rhs.fElements) * ret->fValue.fSymbolSize);
		memcpy(
			(char *)ret->fValue.fRaw + ret->fValue.fElements * ret->fValue.fSymbolSize,
			(char *)rhs.fRaw,
			rhs.fElements * ret->fValue.fSymbolSize);

		rlc_scoped_text_destroy(&rhs);
	}

	return ret;
}

void rlc_scoped_string_expression_delete(
	struct RlcScopedStringExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));
	rlc_scoped_text_destroy(&this->fValue);
	rlc_free((void**)&this);
}