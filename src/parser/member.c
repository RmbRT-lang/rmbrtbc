#include "member.h"
#include "function.h"
#include "variable.h"
#include "class.h"
#include "rawtype.h"
#include "union.h"
#include "typedef.h"
#include "constructor.h"
#include "destructor.h"

#include "../assert.h"
#include "../malloc.h"

#include <string.h>

void rlc_parsed_member_create(
	struct RlcParsedMember * this,
	enum RlcParsedMemberType type,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);

	RLC_DERIVING_TYPE(this) = type;

	this->fVisibility = member->visibility;
	this->fAttribute = member->attribute;
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
		kRlcParsedMember ## Type, \
		1, \
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMember##Type) \
			+ RLC_BASE_OFFSET(RlcParsed##Type, struct RlcParsedMember##Type) \
			+ RLC_BASE_OFFSET(RlcParsedScopeEntry, struct RlcParsed##Type) \
			+ RLC_OFFSETOF(struct RlcParsedScopeEntry, fName) \
	}
#define WITHOUT_NAME(Type) { k ## Type, 0, 0 }

	static struct {
		// As a sanity check.
		enum RlcParsedMemberType type;
		int8_t has_name;
		int16_t name_offset;
	} const k_name_table[] = {
		WITH_NAME(Function),
		WITH_NAME(Variable),
		WITH_NAME(Rawtype),
		WITH_NAME(Union),
		WITH_NAME(Class),
		WITH_NAME(Typedef),
		WITHOUT_NAME(RlcParsedConstructor),
		WITHOUT_NAME(RlcParsedDestructor)
	};

	static_assert(RLC_COVERS_ENUM(k_name_table, RlcParsedMemberType), "ill-sized table");

	RLC_DASSERT(this != NULL);

	enum RlcParsedMemberType const type = RLC_DERIVING_TYPE(this);
	RLC_DASSERT(RLC_IN_ENUM(type, RlcParsedMemberType));
	RLC_DASSERT(type == k_name_table[type].type);

	if(k_name_table[type].has_name)
		return
			(struct RlcSrcString const *)
				(((uintptr_t)this) + k_name_table[type].name_offset);
	else
		return NULL;
}

enum RlcMemberAttribute rlc_member_attribute_parse(
	struct RlcParser * parser)
{
	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokHash))
	{
		return kRlcMemberAttributeIsolated;
	} else if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokStatic))
	{
		return kRlcMemberAttributeStatic;
	} else
	{
		return kRlcMemberAttributeNone;
	}
}

void rlc_parsed_member_common_create(
	struct RlcParsedMemberCommon * this,
	enum RlcVisibility default_visibility)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(default_visibility, RlcVisibility));

	this->visibility = default_visibility;
	this->default_visibility = default_visibility;
	this->attribute = kRlcMemberAttributeNone;
	rlc_parsed_template_decl_create(&this->templates);
}

void rlc_visibility_parse_default(
	enum RlcVisibility * default_visibility,
	struct RlcParser * parser)
{
	RLC_DASSERT(default_visibility != NULL);
	RLC_DASSERT(parser != NULL);

continue_outer:
	while(rlc_parser_is_ahead(parser, kRlcTokColon))
	{
		static struct {
			enum RlcTokenType fToken;
			enum RlcVisibility fVisibility;
		} const k_lookup[] = {
			{kRlcTokPrivate, kRlcVisibilityPrivate},
			{kRlcTokProtected, kRlcVisibilityProtected},
			{kRlcTokPublic, kRlcVisibilityPublic}
		};

		for(size_t i = 0; i < _countof(k_lookup); i++)
		{
			if(rlc_parser_consume(
				parser,
				NULL,
				k_lookup[i].fToken))
			{
				*default_visibility = k_lookup[i].fVisibility;
				rlc_parser_expect(
					parser,
					NULL,
					1,
					kRlcTokColon);
				goto continue_outer;
			}
		}
		return;
	}
}

int rlc_parsed_member_common_parse(
	struct RlcParsedMemberCommon * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	rlc_visibility_parse_default(&out->default_visibility, parser);
	rlc_parsed_template_decl_parse(&out->templates, parser);
	int vis = rlc_visibility_parse(&out->visibility, parser, out->default_visibility);
	out->attribute = rlc_member_attribute_parse(parser);

	return rlc_parsed_template_decl_exists(&out->templates)
		|| vis
		|| out->attribute != kRlcMemberAttributeNone;
}


int rlc_visibility_parse(
	enum RlcVisibility * visibility,
	struct RlcParser * parser,
	enum RlcVisibility default_visibility)
{
	RLC_DASSERT(visibility != NULL);
	RLC_DASSERT(parser != NULL);

	static struct {
		enum RlcTokenType fToken;
		enum RlcVisibility fVisibility;
	} const k_lookup[] = {
		{kRlcTokPrivate, kRlcVisibilityPrivate},
		{kRlcTokProtected, kRlcVisibilityProtected},
		{kRlcTokPublic, kRlcVisibilityPublic}
	};

	for(size_t i = 0; i < _countof(k_lookup); i++)
	{
		if(rlc_parser_consume(
			parser,
			NULL,
			k_lookup[i].fToken))
		{
			*visibility = k_lookup[i].fVisibility;
			return 1;
		}
	}

	*visibility = default_visibility;

	return 0;
}

struct RlcParsedMember * rlc_parsed_member_parse(
	struct RlcParser * parser,
	struct RlcParsedMemberCommon * member,
	int flags)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(member != NULL);

	union Pack {
		struct RlcParsedMemberVariable fVariable;
		struct RlcParsedMemberFunction fFunction;
		struct RlcParsedMemberClass fClass;
		struct RlcParsedMemberUnion fUnion;
		struct RlcParsedMemberRawtype fRawtype;
		struct RlcParsedMemberTypedef fTypedef;
		struct RlcParsedConstructor fConstructor;
		struct RlcParsedDestructor fDestructor;
	} pack;

	typedef int (*parse_fn_t)(
		union Pack *,
		struct RlcParser *,
		struct RlcParsedMemberCommon const *);


#define ENTRY(Type, parse) { \
		k ## Type,\
		(parse_fn_t)parse, \
		sizeof(struct Type), \
		RLC_DERIVE_OFFSET(RlcParsedMember, struct Type) }

	static struct {
		enum RlcParsedMemberType fType;
		parse_fn_t fParseFn;
		size_t fTypeSize;
		size_t fOffset;
	} const k_parse_lookup[] = {
		ENTRY(RlcParsedConstructor, &rlc_parsed_constructor_parse),
		ENTRY(RlcParsedDestructor, &rlc_parsed_destructor_parse),
		ENTRY(RlcParsedMemberFunction, &rlc_parsed_member_function_parse),
		ENTRY(RlcParsedMemberVariable, &rlc_parsed_member_variable_parse),
		ENTRY(RlcParsedMemberClass, &rlc_parsed_member_class_parse),
		ENTRY(RlcParsedMemberUnion, &rlc_parsed_member_union_parse),
		ENTRY(RlcParsedMemberRawtype, &rlc_parsed_member_rawtype_parse),
		ENTRY(RlcParsedMemberTypedef, &rlc_parsed_member_typedef_parse)
	};

	static_assert(RLC_COVERS_ENUM(k_parse_lookup, RlcParsedMemberType), "ill-sized parse table.");

	int expect_member = rlc_parsed_member_common_parse(member, parser);

	for(size_t i = 0; i < _countof(k_parse_lookup); i++)
	{
		if(RLC_FLAG(k_parse_lookup[i].fType) & flags)
		{
			if(k_parse_lookup[i].fParseFn(
				&pack,
				parser,
				member))
			{
				void * temp = NULL;
				rlc_malloc(&temp, k_parse_lookup[i].fTypeSize);

				memcpy(temp, &pack, k_parse_lookup[i].fTypeSize);

				struct RlcParsedMember * ret;
				ret = (void*) ((uint8_t*)temp + k_parse_lookup[i].fOffset);

				return ret;
			}
		}
	}

	if(expect_member)
		rlc_parser_fail(parser, "expected member");

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