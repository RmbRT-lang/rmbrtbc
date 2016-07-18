#include "typename.h"
#include "../malloc.h"
#include <assert.h>

void rlc_type_name_destroy(
	struct RlcTypeName * this)
{
	assert(this != NULL);

	if(this->fTypeModifiers)
	{
		rlc_free((void**)&this->fTypeModifiers);
		this->fTypeModifierCount = 0;
	}

	if(!this->fIsVoid)
		rlc_symbol_destroy(&this->fName);
}


void rlc_type_name_add_modifier(
	struct RlcTypeName * this,
	struct RlcTypeModifier const * modifier)
{
	assert(this != NULL);
	assert(modifier != NULL);

	rlc_realloc(
		(void**)&this->fTypeModifiers,
		sizeof(struct RlcTypeModifier) * ++this->fTypeModifierCount);

	this->fTypeModifiers[this->fTypeModifierCount-1] = *modifier;
}