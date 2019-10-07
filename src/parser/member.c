#include "member.h"
#include "function.h"
#include "variable.h"
#include "class.h"
#include "rawtype.h"
#include "union.h"
#include "struct.h"
#include "typedef.h"
#include "constructor.h"
#include "destructor.h"

#include "../assert.h"
#include "../malloc.h"

#include <string.h>

void rlc_parsed_member_create(
	struct RlcParsedMember * this,
	enum RlcParsedMemberType type,
	enum RlcVisibility visibility,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);

	RLC_DERIVING_TYPE(this) = type;

	this->fVisibility = visibility;
	this->fLocation.fBegin = start_index;
	this->fLocation.fEnd = 0;
}

void rlc_parsed_member_destroy_virtual(
	struct RlcParsedMember * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcParsedMemberType));

	typedef void (*destructor_t)(
		void * this);

	static destructor_t const k_vtable[] = {
		(destructor_t)&rlc_parsed_member_function_destroy,
		(destructor_t)&rlc_parsed_member_variable_destroy,
		(destructor_t)&rlc_parsed_member_rawtype_destroy,
		(destructor_t)&rlc_parsed_member_struct_destroy,
		(destructor_t)&rlc_parsed_member_union_destroy,
		(destructor_t)&rlc_parsed_member_class_destroy,
		(destructor_t)&rlc_parsed_member_typedef_destroy,
		(destructor_t)&rlc_parsed_constructor_destroy,
		(destructor_t)&rlc_parsed_destructor_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedMemberType), "ill-sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberFunction),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberVariable),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberRawtype),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberStruct),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberUnion),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberClass),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberTypedef),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedConstructor),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedDestructor)
	};

	static_assert(RLC_COVERS_ENUM(k_offsets, RlcParsedMemberType), "ill-sized offset table.");


	k_vtable[RLC_DERIVING_TYPE(this)](
		(uint8_t*)this + k_offsets[RLC_DERIVING_TYPE(this)]);
}

void rlc_parsed_member_destroy_base(
	struct RlcParsedMember * this)
{
	RLC_DASSERT(this != NULL);
}

struct RlcSrcString const * rlc_parsed_member_name(
	struct RlcParsedMember * this)
{
#define WITH_NAME(Type) \
	{ \
		k ## Type, \
		1, \
		RLC_DERIVE_OFFSET(RlcParsedMember, struct Type) \
			+ RLC_BASE_OFFSET(RlcScopeEntry, struct Type) \
			+ RLC_OFFSETOF(struct RlcScopeEntry, fName) \
	}
#define WITHOUT_NAME(Type) { k ## Type, 0, 0 }

	static struct {
		// As a sanity check.
		RlcParsedMemberType type;
		int8_t has_name;
		int16_t name_offset;
	} const k_name_table[] = {
		WITH_NAME(RlcParsedMemberFunction),
		WITH_NAME(RlcParsedMemberVariable),
		WITH_NAME(RlcParsedMemberRawtype),
		WITH_NAME(RlcParsedMemberStruct),
		WITH_NAME(RlcParsedMemberUnion),
		WITH_NAME(RlcParsedMemberClass),
		WITH_NAME(RlcParsedMemberTypedef),
		WITHOUT_NAME(RlcParsedConstructor),
		WITHOUT_NAME(RlcParsedDestructor)
	};

	static_assert(RLC_COVERS_ENUM(k_name_table, RlcParsedMemberType), "ill-sized table");

	RLC_DASSERT(this != NULL);

	RlcParsedMemberType const type = RLC_DERIVING_TYPE(this);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedMemberType));
	RLC_DASSERT(type == k_name_table[type].type);

	if(k_name_table[type].has_name)
		return
			(struct RlcSrcString const *)
				(uintptr_t(this) + k_name_table[type].name_offset);
	else
		return NULL;
}

enum RlcMemberAttribute rlc_member_attribute_parse(
	struct RlcParserData * parser)
{
	if(rlc_parser_data_consume(
		parser,
		kRlcTokIsolated))
	{
		return kRlcMemberAttributeIsolated;
	} else if(rlc_parser_data_consume(
		parser,
		kRlcTokStatic))
	{
		return kRlcMemberAttributeStatic;
	} else
	{
		return kRlcMemberAttributeNone;
	}
}

enum RlcVisibility rlc_visibility_parse(
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser)
{
	RLC_DASSERT(default_visibility != NULL);
	RLC_DASSERT(parser != NULL);


	static struct {
		enum RlcTokenType fToken;
		enum RlcVisibility fVisibility;
	} const k_lookup[] = {
		{kRlcTokPrivate, kRlcVisibilityPrivate},
		{kRlcTokProtected, kRlcVisibilityProtected},
		{kRlcTokPublic, kRlcVisibilityPublic}
	};

	enum RlcVisibility ret = *default_visibility;

	// parse (Modifier ":")* Modifier?
accept_once_more:
	for(int i = 0; i < _countof(k_lookup); i++)
	{
		if(rlc_parser_data_consume(
			parser,
			k_lookup[i].fToken))
		{
			ret = k_lookup[i].fVisibility;
			break;
		}
	}

	if(rlc_parser_data_consume(
		parser,
		kRlcTokColon))
	{
		*default_visibility = ret;
		goto accept_once_more;
	}

	return ret;
}

struct RlcParsedMember * rlc_parsed_member_parse(
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser,
	int flags)
{
	RLC_DASSERT(default_visibility != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start_index = parser->fIndex;

	union Pack {
		struct RlcParsedMemberVariable fVariable;
		struct RlcParsedMemberFunction fFunction;
		struct RlcParsedMemberClass fClass;
		struct RlcParsedMemberUnion fUnion;
		struct RlcParsedMemberStruct fStruct;
		struct RlcParsedMemberRawtype fRawtype;
		struct RlcParsedMemberTypedef fTypedef;
		struct RlcParsedConstructor fConstructor;
		struct RlcParsedDestructor fDestructor;
	} pack;

	typedef int (*parse_fn_t)(
		union Pack *,
		enum RlcVisibility *,
		struct RlcParserData *);


#define ENTRY(Type, parse, error) { \
		k ## Type,\
		(parse_fn_t)parse, \
		error, \
		sizeof(struct Type), \
		RLC_DERIVE_OFFSET(RlcParsedMember, struct Type) }

	static struct {
		enum RlcParsedMemberType fType;
		parse_fn_t fParseFn;
		enum RlcParseError fErrorCode;
		size_t fTypeSize;
		size_t fOffset;
	} const k_parse_lookup[] = {
		ENTRY(RlcParsedConstructor, &rlc_parsed_constructor_parse, kRlcParseErrorExpectedConstructor),
		ENTRY(RlcParsedDestructor, &rlc_parsed_destructor_parse,
			kRlcParseErrorExpectedDestructor),
		ENTRY(RlcParsedMemberFunction, &rlc_parsed_member_function_parse, kRlcParseErrorExpectedMemberFunction),
		ENTRY(RlcParsedMemberVariable, &rlc_parsed_member_variable_parse, kRlcParseErrorExpectedMemberVariable),
		ENTRY(RlcParsedMemberClass, &rlc_parsed_member_class_parse, kRlcParseErrorExpectedMemberClass),
		ENTRY(RlcParsedMemberUnion, &rlc_parsed_member_union_parse, kRlcParseErrorExpectedMemberUnion),
		ENTRY(RlcParsedMemberStruct, &rlc_parsed_member_struct_parse, kRlcParseErrorExpectedMemberStruct),
		ENTRY(RlcParsedMemberRawtype, &rlc_parsed_member_rawtype_parse, kRlcParseErrorExpectedMemberRawtype),
		ENTRY(RlcParsedMemberTypedef, &rlc_parsed_member_typedef_parse, kRlcParseErrorExpectedMemberTypedef)
	};

	static_assert(RLC_COVERS_ENUM(k_parse_lookup, RlcParsedMemberType), "ill-sized parse table.");

	for(int i = 0; i < _countof(k_parse_lookup); i++)
	{
		if(flags & RLC_FLAG(k_parse_lookup[i].fType))
		{
			if(k_parse_lookup[i].fParseFn(
				&pack,
				default_visibility,
				parser))
			{
				void * temp = NULL;
				rlc_malloc(&temp, k_parse_lookup[i].fTypeSize);

				memcpy(temp, &pack, k_parse_lookup[i].fTypeSize);

				struct RlcParsedMember * ret;
				ret = (void*) ((uint8_t*)temp + k_parse_lookup[i].fOffset);

				return ret;
			} else if(parser->fErrorCount)
			{
				rlc_parser_data_add_error(
					parser,
					k_parse_lookup[i].fErrorCode);
				break;
			}
		}
	}

	parser->fIndex = start_index;
	return NULL;
}

void rlc_parsed_member_list_create(
	struct RlcParsedMemberList * this)
{
	RLC_DASSERT(this != NULL);

	this->fEntries = NULL;
	this->fEntryCount = 0;
}

void rlc_parsed_member_list_add(
	struct RlcParsedMemberList * this,
	struct RlcParsedMember * member)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(member != NULL);

	rlc_realloc(
		(void**)&this->fEntries,
		++this->fEntryCount * sizeof(struct RlcParsedMember *));

	this->fEntries[this->fEntryCount-1] = member;
}

void rlc_parsed_member_list_destroy(
	struct RlcParsedMemberList * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fEntries)
	{
		for(size_t i = 0; i < this->fEntryCount; i++)
		{
			rlc_parsed_member_destroy_virtual(this->fEntries[i]);
			rlc_free((void**)&this->fEntries[i]);
		}
		rlc_free((void**)&this->fEntries);
	}

	this->fEntryCount = 0;
}