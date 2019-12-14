#include "scope.h"
#include "../assert.h"
#include "../malloc.h"


struct RlcResolvedScope * rlc_resolved_scope_new(
	struct RlcResolvedScope * parent)
{
	struct RlcResolvedScope * ret = NULL;
	rlc_malloc(
		(void**)&ret,
		sizeof(struct RlcResolvedScope));

	ret->parent = parent;
	ret->siblings = NULL;
	ret->siblingCount = 0;
	ret->entries = NULL;
	ret->entryCount = 0;

	return ret;
}

void rlc_resolved_scope_delete(
	struct RlcResolvedScope * this)
{
	RLC_DASSERT(this != NULL);

	if(this->siblings)
	{
		rlc_free((void**)&this->siblings);
		this->siblingCount = 0;
	}

	if(this->entries)
	{
		for(RlcSrcIndex i = 0; i < this->entryCount; i++)
			rlc_resolved_scope_entry_deref(this->entries[i]);
		rlc_free((void**)&this->entries);
		this->entryCount = 0;
	}
}

static int rlc_resolved_scope_filter_impl(
	struct RlcResolvedScope * this,
	struct RlcResolvedScopeEntryName const * name,
	rlc_resolved_scope_filter_fn_t callback,
	void * userdata,
	int parents,
	int siblings,
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

	if(siblings)
	{
		for(RlcSrcIndex i = 0; i < this->siblingCount; i++)
		{
			found |= rlc_resolved_scope_filter_impl(
				this->siblings[i],
				name,
				callback,
				userdata,
				0, 0, // Don't look into siblings' relatives.
				abort);

			if(*abort)
				return 1;
		}
	}
	if(parents)
	{
		if(this->parent)
		{
			found |= rlc_resolved_scope_filter_impl(
				this->parent,
				name,
				callback,
				userdata,
				1, siblings, // Look into parent's relatives.
				abort);
		}
	}

	return found;
}

int rlc_resolved_scope_filter(
	struct RlcResolvedScope * this,
	struct RlcResolvedScopeEntryName const * name,
	rlc_resolved_scope_filter_fn_t callback,
	void * userdata,
	int check_parents,
	int check_siblings)
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
		check_parents,
		check_siblings,
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