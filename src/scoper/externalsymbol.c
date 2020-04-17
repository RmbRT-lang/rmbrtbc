#include "externalsymbol.h"
#include "../parser/externalsymbol.h"

#include "../assert.h"

void rlc_scoped_external_symbol_create(
	struct RlcScopedExternalSymbol * this,
	struct RlcSrcFile const * file,
	struct RlcParsedExternalSymbol const * parsed,
	struct RlcScopedScopeItemGroup * parent)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);
	RLC_DASSERT(parent != NULL);

	rlc_scoped_scope_entry_create(
		RLC_BASE_CAST(this, RlcScopedScopeEntry),
		file,
		RLC_BASE_CAST(parsed, RlcParsedScopeEntry),
		parent,
		kRlcScopedExternalSymbol,
		NULL);

	if((this->hasCustomLinkName = parsed->fHasCustomLinkName))
	{
		rlc_scoped_text_create(
			&this->customLinkName,
			file,
			&parsed->fCustomLinkName);
	}

	rlc_scoped_type_name_create(&this->type, file, &parsed->fType);
}

void rlc_scoped_external_symbol_destroy(
	struct RlcScopedExternalSymbol * this)
{
	if(this->hasCustomLinkName)
		rlc_scoped_text_destroy(&this->customLinkName);

	rlc_scoped_type_name_destroy(&this->type);

	rlc_scoped_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcScopedScopeEntry));
}