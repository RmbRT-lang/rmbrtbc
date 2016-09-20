#include "typename.h"
#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_type_name_destroy(
	struct RlcParsedTypeName * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fTypeModifiers)
	{
		rlc_free((void**)&this->fTypeModifiers);
		this->fTypeModifierCount = 0;
	}

	if(!this->fIsVoid)
		rlc_parsed_symbol_destroy(&this->fName);
}


void rlc_parsed_type_name_add_modifier(
	struct RlcParsedTypeName * this,
	struct RlcTypeModifier const * modifier)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(modifier != NULL);

	rlc_realloc(
		(void**)&this->fTypeModifiers,
		sizeof(struct RlcTypeModifier) * ++this->fTypeModifierCount);

	this->fTypeModifiers[this->fTypeModifierCount-1] = *modifier;
}