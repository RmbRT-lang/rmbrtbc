#include "scope.h"
#include "../assert.h"
#include "../malloc.h"


static int rlc_resolved_scope_filter_impl(
	struct RlcResolvedScope * this,
	struct RlcResolvedScopeEntryName const * name,
	rlc_resolved_scope_filter_fn_t callback,
	void * userdata,
	int relatives,
	int * abort)
{
	int found = 0;

	for(RlcSrcIndex i = 0; i < this->entryCount; i++)
	{
		if(0 == rlc_resolved_scope_entry_name_compare(
			name,
			&this->entries[i]->name))
		{
			found = 1;
			if(!callback(this->entries[i], userdata))
			{
				*abort = 1;
				return 1;
			}
		}
	}

	if(relatives)
	{
		for(RlcSrcIndex i = 0; i < this->siblingCount; i++)
		{
			found |= rlc_resolved_scope_filter_impl(
				this->siblings[i],
				name,
				callback,
				userdata,
				0, // Don't look into siblings' relatives.
				abort);

			if(*abort)
				return 1;
		}

		if(this->parent)
		{
			found |= rlc_resolved_scope_filter_impl(
				this->parent,
				name,
				callback,
				userdata,
				1, // Look into parent's relatives.
				abort);
		}
	}

	return found;
}

int rlc_resolved_scope_filter(
	struct RlcResolvedScope * this,
	struct RlcResolvedScopeEntryName const * name,
	rlc_resolved_scope_filter_fn_t callback,
	void * userdata)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);
	RLC_DASSERT(callback != NULL);

	int abort = 0;
	return rlc_resolved_scope_filter_impl(
		this,
		name,
		callback,
		userdata,
		1,
		&abort);
}

struct RlcResolvedScopeEntry * rlc_resolved_scope_add_entry(
	struct RlcResolvedScope * this,
	struct RlcSrcFile * file,
	struct RlcParsedScopeEntry * entry)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(entry != NULL);

	struct RlcResolvedScopeEntry * res = rlc_resolved_scope_entry_new(file, entry);

	rlc_realloc(
		(void **)&this->entries,
		sizeof(struct RlcResolvedScopeEntry *) * ++this->entryCount);
	this->entries[this->entryCount-1] = res;
	return res;
}