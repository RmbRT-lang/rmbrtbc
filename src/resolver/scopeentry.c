#include "scopeentry.h"
#include "../assert.h"
#include "../malloc.h"

#include "../parser/member.h"
#include <string.h>

const struct RlcResolvedScopeEntryName kRlcResolvedScopeEntryNameConstructor = {
	kRlcResolvedScopeEntryNameTypeConstructor,
	NULL
};
const struct RlcResolvedScopeEntryName kRlcResolvedScopeEntryNameDestructor = {
	kRlcResolvedScopeEntryNameTypeDestructor,
	NULL
};

void rlc_resolved_scope_entry_name_create(
	struct RlcResolvedScopeEntryName * this,
	struct RlcSrcFile * file,
	struct RlcParsedScopeEntry * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(parsed != NULL);

	switch(RLC_DERIVING_TYPE(parsed))
	{
	case kRlcParsedConstructor:
		this->type = kRlcResolvedScopeEntryNameTypeConstructor;
		this->name = NULL;
		break;
	case kRlcParsedDestructor:
		this->type = kRlcResolvedScopeEntryNameTypeDestructor;
		this->name = NULL;
		break;
	default:
		RLC_DASSERT(parsed->fName.length != 0);
		this->type = kRlcResolvedScopeEntryNameTypeIdentifier;
		this->name = rlc_src_string_cstr(&parsed->fName, file);
	}
}

void rlc_resolved_scope_entry_name_destroy(
	struct RlcResolvedScopeEntryName * this)
{
	RLC_DASSERT(this != NULL);

	if(this->name)
		rlc_free((void**)&this->name);
}

int rlc_resolved_scope_entry_name_compare(
	struct RlcResolvedScopeEntryName const * a,
	struct RlcResolvedScopeEntryName const * b)
{
	int diff = a->type - b->type;
	if(diff)
		return diff;
	else if(a->type == kRlcResolvedScopeEntryNameTypeIdentifier)
	{
		return strcmp(a->name, b->name);
	} else
		return 0;
}

struct RlcResolvedScopeEntry * rlc_resolved_scope_entry_new(
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry * parsed)
{
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	struct RlcResolvedScopeEntry * ret = NULL;
	rlc_malloc((void**)&ret, sizeof(struct RlcResolvedScopeEntry));
	ret->file = file;
	ret->parsed = parsed;
	ret->references = 1;

	return ret;
}

static void rlc_resolved_scope_entry_destroy(
	struct RlcResolvedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->references == 0);

	rlc_resolved_scope_entry_name_destroy(&this->name);
	rlc_free((void**)&this->parsed);
	rlc_free((void**)&this);
}

void rlc_resolved_scope_entry_reference(
	struct RlcResolvedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	++this->references;
}

void rlc_resolved_scope_entry_deref(
	struct RlcResolvedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(this->references != 0);

	if(--this->references)
		rlc_resolved_scope_entry_destroy(this);
}
