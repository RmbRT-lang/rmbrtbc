#include "typename.h"
#include "symbol.h"
#include "functionsignature.h"
#include "expression.h"
#include "../macros.h"
#include "../malloc.h"
#include "../parser/typename.h"

void rlc_scoped_type_name_create(
	struct RlcScopedTypeName * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTypeName const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	this->fName = NULL;
	this->fParsed = parsed;
	switch(parsed->fValue)
	{
	default:
		RLC_DASSERT(!"unhandled enum value");
	case kRlcParsedTypeNameValueName:
		{
			rlc_malloc(
				(void**)&this->fName,
				sizeof(struct RlcScopedSymbol));
			rlc_scoped_symbol_create(
				this->fName,
				file,
				parsed->fName);
		} break;
	case kRlcParsedTypeNameValueFunction:
		{
			rlc_malloc(
				(void**)&this->fFunction,
				sizeof(struct RlcScopedFunctionSignature));
			rlc_scoped_function_signature_create(
				this->fFunction,
				file,
				parsed->fFunction);
		} break;
	case kRlcParsedTypeNameValueExpression:
		{
			this->fExpression = rlc_scoped_expression_new(
				parsed->fExpression,
				file);
		} break;
	case kRlcParsedTypeNameValueVoid:
		{
			;
		} break;
	}
}

void rlc_scoped_type_name_destroy(
	struct RlcScopedTypeName * this)
{
	RLC_DASSERT(this != NULL);

	switch(this->fParsed->fValue)
	{
	default:
		RLC_DASSERT(!"unhandled enum value");
	case kRlcParsedTypeNameValueName:
		{
			rlc_scoped_symbol_destroy(this->fName);
			rlc_free((void**)&this->fName);
		} break;
	case kRlcParsedTypeNameValueFunction:
		{
			rlc_scoped_function_signature_destroy(
				this->fFunction,
				this->fParsed->fFunction);
			rlc_free((void**)&this->fFunction);
		} break;
	case kRlcParsedTypeNameValueExpression:
		{
			rlc_scoped_expression_delete_virtual(this->fExpression);
			this->fExpression = NULL;
		} break;
	case kRlcParsedTypeNameValueVoid:
		{
			;
		} break;
	}
}