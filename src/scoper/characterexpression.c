#include "characterexpression.h"
#include "../parser/characterexpression.h"
#include "../resolver/resolver.h"
#include "../malloc.h"

#include <string.h>

#define kRlcScopedCharacterExpression kRlcParsedCharacterExpression

struct RlcScopedCharacterExpression * rlc_scoped_character_expression_new(
	struct RlcParsedCharacterExpression * parsed,
	struct RlcSrcFile * file)
{
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(file != NULL);

	struct RlcScopedCharacterExpression * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcScopedCharacterExpression));

	rlc_scoped_expression_create(
		RLC_BASE_CAST(ret, RlcScopedExpression),
		RLC_BASE_CAST(parsed, RlcParsedExpression),
		kRlcScopedCharacterExpression);

	rlc_scoped_text_create(
		&ret->fValue,
		file,
		&parsed->fToken);
	RLC_DASSERT(!ret->fValue.fIsString);

	return ret;
}

void rlc_scoped_character_expression_delete(
	struct RlcScopedCharacterExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_scoped_expression_destroy_base(
		RLC_BASE_CAST(this, RlcScopedExpression));
	rlc_scoped_text_destroy(&this->fValue);
	rlc_free((void**)&this);
}