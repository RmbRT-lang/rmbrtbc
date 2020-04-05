#include "controllabel.h"
#include "../parser/controllabel.h"
#include "../assert.h"

void rlc_scoped_control_label_create(
	struct RlcScopedControlLabel * this,
	struct RlcSrcFile const * file,
	struct RlcParsedControlLabel const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	if(!(this->exists = parsed->fExists))
		return;

	if(parsed->fLabel.type == kRlcTokStringLiteral)
		rlc_scoped_text_create(&this->string, file, &parsed->fLabel);
	else
		rlc_scoped_identifier_from_token(
			&this->identifier,
			file,
			&parsed->fLabel);
}

void rlc_scoped_control_label_destroy(
	struct RlcScopedControlLabel * this)
{
	if(!this->exists)
		return;

	this->exists = 0;

	if(this->isString)
		rlc_scoped_text_destroy(&this->string);
	else
		rlc_scoped_identifier_destroy(&this->identifier);
}