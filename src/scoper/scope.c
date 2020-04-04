#include "scope.h"
#include "scopeitem.h"
#include "scopeitemgroup.h"
#include "scopeentry.h"
#include "member.h"
#include "statement.h"
#include "symbol.h"
#include "../assert.h"
#include "../malloc.h"

static struct RlcScopedScope * rlc_scoped_scope_new(
	int ownerIsItem)
{
	struct RlcScopedScope * ret = NULL;
	rlc_malloc(
		(void**)&ret,
		sizeof(struct RlcScopedScope));

	ret->ownerIsItem = ownerIsItem;
	ret->siblings = NULL;
	ret->siblingCount = 0;
	ret->groups = NULL;
	ret->groupCount = 0;

	return ret;
}

struct RlcScopedScope * rlc_scoped_scope_new_for_item(
	struct RlcScopedScopeItem * owner)
{
	struct RlcScopedScope * ret = rlc_scoped_scope_new(1);
	ret->ownerItem = owner;

	return ret;
}

struct RlcScopedScope * rlc_scoped_scope_new_for_statement(
	struct RlcScopedStatement * owner)
{
	struct RlcScopedScope * ret = rlc_scoped_scope_new(0);
	ret->ownerStatement = owner;

	return ret;
}

void rlc_scoped_scope_delete(
	struct RlcScopedScope * this)
{
	RLC_DASSERT(this != NULL);

	if(this->siblings)
	{
		rlc_free((void**)&this->siblings);
		this->siblingCount = 0;
	}

	if(this->groups)
	{
		for(RlcSrcIndex i = 0; i < this->groupCount; i++)
		{
			rlc_scoped_scope_item_group_destroy(this->groups[i]);
			rlc_free((void**)&this->groups[i]);
		}
		rlc_free((void**)&this->groups);
		this->groupCount = 0;
	}

	rlc_free((void**)&this);
}

struct RlcScopedScope * rlc_scoped_scope_parent(
	struct RlcScopedScope * this)
{
	RLC_DASSERT(this != NULL);

	if(this->ownerIsItem)
	{
		if(this->ownerItem)
		{
			RLC_DASSERT(this->ownerItem->children != NULL);
			return this->ownerItem->children;
		}
	} else {
		if(this->ownerStatement)
		{
			RLC_DASSERT(this->ownerStatement->scope != NULL);
			return this->ownerStatement->scope;
		}
	}
	return NULL;
}

static int rlc_scoped_scope_filter_impl(
	struct RlcScopedScope * this,
	struct RlcScopedSymbolChild const * name,
	rlc_scoped_scope_filter_fn_t callback,
	void * userdata,
	int parents,
	int siblings,
	int * abort)
{
	int found = 0;

	signed left = 0, right = this->groupCount;
	while(left < right)
	{
		RlcSrcIndex i = (left + right) >> 1;
		signed sign =  rlc_scoped_identifier_compare(
			&this->groups[i]->name,
			&name->name);

		if(sign == 0)
		{
			found = 1;
			for(RlcSrcIndex j = 0; j < this->groups[i]->itemCount; j++)
				if(!callback(this->groups[i]->items[j], userdata))
				{
					*abort = 1;
					return 1;
				}
			break;
		}
	}

	if(siblings)
	{
		for(RlcSrcIndex i = 0; i < this->siblingCount; i++)
		{
			found |= rlc_scoped_scope_filter_impl(
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

	if(!found && parents)
	{
		struct RlcScopedScope * parent = rlc_scoped_scope_parent(this);
		if(parent)
		{
			found |= rlc_scoped_scope_filter_impl(
				parent,
				name,
				callback,
				userdata,
				1, siblings, // Look into parent's relatives.
				abort);
		}
	}

	return found;
}

int rlc_scoped_scope_filter(
	struct RlcScopedScope * this,
	struct RlcScopedSymbolChild const * name,
	rlc_scoped_scope_filter_fn_t callback,
	void * userdata,
	int check_parents,
	int check_siblings)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);
	RLC_DASSERT(callback != NULL);

	int abort = 0;
	return rlc_scoped_scope_filter_impl(
		this,
		name,
		callback,
		userdata,
		check_parents,
		check_siblings,
		&abort);
}


struct RlcScopedScopeItemGroup * rlc_scoped_scope_group(
	struct RlcScopedScope * this,
	struct RlcToken const * name,
	struct RlcSrcFile const * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);
	RLC_DASSERT(file != NULL);

	signed left = 0, right = this->groupCount;
	while(left < right)
	{
		RlcSrcIndex i = (left + right) >> 1;
		signed sign;
		switch(name->type)
		{
		case kRlcTokConstructor:
			{
				sign = rlc_scoped_identifier_compare(
					&kRlcScopedIdentifierConstructor,
					&this->groups[i]->name);
			} break;
		case kRlcTokDestructor:
			{
				sign = rlc_scoped_identifier_compare(
					&kRlcScopedIdentifierDestructor,
					&this->groups[i]->name);
			} break;
		default:
			{
				RLC_DASSERT(name->type == kRlcTokIdentifier);

				sign = kRlcScopedIdentifierTypeIdentifier - this->groups[i]->name.type;
				if(sign == 0)
					sign = rlc_src_string_cmp_cstr(
						file,
						&name->content,
						this->groups[i]->name.name);
			} break;
		}

		if(sign == 0)
			return this->groups[i];

		if(sign < 0)
			right = i-1;
		else
			left = i+1;
	}

	// `left` is the index where it should be.
	rlc_realloc(
		(void**)&this->groups,
		++this->groupCount * sizeof(struct RlcScopedScopeItemGroup *));

	for(RlcSrcIndex i = this->groupCount; i --> left+1;)
		this->groups[i] = this->groups[i-1];

	this->groups[left] = NULL;
	rlc_malloc(
		(void**)&this->groups[left],
		sizeof(struct RlcScopedScopeItemGroup));

	struct RlcScopedIdentifier groupName;
	rlc_scoped_identifier_from_token(&groupName, file, name);
	rlc_scoped_scope_item_group_create(
		this->groups[left],
		&groupName,
		this);
	return this->groups[left];
}

struct RlcScopedScopeEntry * rlc_scoped_scope_add_entry(
	struct RlcScopedScope * this,
	struct RlcSrcFile const * file,
	struct RlcParsedScopeEntry const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	// Retrieve the scope entry name as a token.
	struct RlcToken nameToken = { parsed->fName, kRlcTokIdentifier };

	// Retrieve the scope item group.
	struct RlcScopedScopeItemGroup * group;
	group = rlc_scoped_scope_group(this, &nameToken, file);

	// Create and insert the scoped scope entry.
	struct RlcScopedScopeEntry * entry = rlc_scoped_scope_entry_new(
		file,
		parsed,
		group);

	rlc_scoped_scope_item_group_add(
		group,
		RLC_BASE_CAST(entry, RlcScopedScopeItem));

	return entry;
}

struct RlcScopedMember * rlc_scoped_scope_add_member(
	struct RlcScopedScope * this,
	struct RlcSrcFile const * file,
	struct RlcParsedMember const * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(parsed != NULL);

	// Retrieve the member name as a token.
	struct RlcSrcString const * name = rlc_parsed_member_name(parsed);
	struct RlcToken nameToken;
	if(name)
	{
		nameToken.type = kRlcTokIdentifier;
		nameToken.content = *name;
	} else switch(RLC_DERIVING_TYPE(parsed))
	{
	case kRlcParsedConstructor: nameToken.type = kRlcTokConstructor; break;
	case kRlcParsedDestructor: nameToken.type = kRlcTokDestructor; break;
	default:
		RLC_DASSERT(!"unhandled member type");
	}

	// Retrieve the scope item group.
	struct RlcScopedScopeItemGroup * group;
	group = rlc_scoped_scope_group(this, &nameToken, file);

	// Create and insert the scoped member.
	struct RlcScopedMember * member = rlc_scoped_member_new(
		file,
		parsed,
		group);

	rlc_scoped_scope_item_group_add(
		group,
		RLC_BASE_CAST(member, RlcScopedScopeItem));

	return member;
}

void rlc_scoped_scope_populate(
	struct RlcScopedScope * this,
	struct RlcParsedFile const * file)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);

	for(RlcSrcIndex i = 0; i < file->fScopeEntries.fEntryCount; i++)
	{
		rlc_scoped_scope_add_entry(
			this,
			&file->fSource,
			file->fScopeEntries.fEntries[i]);
	}
}