#include "functionsignature.h"
#include "../parser/typename.h"
#include "../assert.h"
#include "../malloc.h"

void rlc_scoped_function_signature_create(
	struct RlcScopedFunctionSignature * this,
	struct RlcSrcFile const * file,
	struct RlcParsedFunctionSignature const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	this->fArguments = NULL;
	if(parsed->fArgumentCount)
		rlc_malloc(
			(void**)&this->fArguments,
			sizeof(struct RlcScopedTypeName) * parsed->fArgumentCount);

	for(RlcSrcSize i = 0; i < parsed->fArgumentCount; i++)
		rlc_scoped_type_name_create(
			&this->fArguments[i],
			file,
			&parsed->fArguments[i]);

	rlc_scoped_type_name_create(
		&this->fResult,
		file,
		&parsed->fResult);
}

void rlc_scoped_function_signature_destroy(
	struct RlcScopedFunctionSignature * this,
	struct RlcParsedFunctionSignature const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);

	for(RlcSrcSize i = 0; i < parsed->fArgumentCount; i++)
		rlc_scoped_type_name_destroy(&this->fArguments[i]);

	if(this->fArguments)
		rlc_free((void**)&this->fArguments);

	rlc_scoped_type_name_destroy(&this->fResult);
}