#include "member.h"
#include "function.h"
#include "variable.h"
#include "class.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_member_create(
	struct RlcParsedMember * this,
	enum RlcParsedMemberType type,
	enum RlcVisibility visibility)
{
	RLC_DASSERT(this != NULL);

	RLC_DERIVING_TYPE(this) = type;

	this->fVisibility = visibility;
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
		(destructor_t)&rlc_parsed_member_class_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedMemberType), "ill-sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberFunction),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberVariable),
		RLC_DERIVE_OFFSET(RlcParsedMember, struct RlcParsedMemberClass)
	};

	static_assert(RLC_COVERS_ENUM(k_offsets, RlcParsedMemberType), "ill-sized offset table.");


	k_vtable[RLC_DERIVING_TYPE(this)](
		(uint8_t*)this + k_offsets[RLC_DERIVING_TYPE(this)]);
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

	for(int i = 0; i < _countof(k_lookup); i++)
	{
		if(rlc_parser_data_consume(
			k_lookup[i].fToken,
			parser))
		{
			ret = k_lookup[i].fVisibility;

			if(rlc_parser_data_consume(
				kRlcTokColon,
				parser))
			{
				*default_visibility = ret;
			}

			break;
		}
	}

	return ret;
}

struct RlcParsedMember * rlc_parsed_member_parse(
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser)
{
	RLC_DASSERT(default_visibility != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const parser_index = parser->fIndex;
	enum RlcVisibility const visibility = rlc_visibility_parse(
		default_visibility,
		parser);

	union {
		struct RlcParsedMemberVariable fVariable;
		struct RlcParsedMemberFunction fFunction;
		struct RlcParsedMemberClass fClass;
		struct RlcParsedMemberUnion fUnion;
		struct RlcParsedMemberStruct fStruct;
		struct RlcParsedMemberRawtype fRawtype;
	} pack;


	struct RlcParsedMember * ret = NULL;

	// member variable
	if(rlc_parsed_member_variable_parse(
		&pack.fVariable,
		parser))
	{
		struct RlcParsedMemberVariable * var = NULL;
		rlc_malloc((void**)&var, sizeof(struct RlcParsedMemberVariable));

		*var = pack.fVariable;
		ret = RLC_BASE_CAST(var, RlcParsedMember);
	} else if(parser->fErrorCount)
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedMemberVariable), 0;
	}
	// member function
	else if(rlc_parsed_member_function_parse(
		&pack.fFunction,
		parser))
	{
		struct RlcParsedMemberFunction * fun = NULL;
		rlc_malloc((void**)&fun, sizeof(struct RlcParsedMemberFunction));

		*fun = pack.fFunction;
		ret = RLC_BASE_CAST(fun, RlcParsedMember);
	} else if(parser->fErrorCount)
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedMemberFunction), 0;
	}
	// member class
	else if(rlc_parsed_member_class_parse(
		&pack.fClass,
		parser))
	{
		struct RlcParsedMemberClass * cls = NULL;
		rlc_malloc((void**)&fun, sizeof(struct RlcParsedMemberClass));

		*fun = pack.fClass;
		ret = RLC_BASE_CAST(fun, RlcParsedMember);
	} else if(parser->fErrorCount)
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedMemberClass), 0;
	}
	// member union
	else if(rlc_parsed_member_union_parse(
		&pack.fUnion,
		parser))
	{
		struct RlcParsedMemberUnion * un = NULL;
		rlc_malloc((void**)&un, sizeof(struct RlcParsedMemberUnion));

		*un = pack.fUnion;
		ret = RLC_BASE_CAST(fun, RlcParsedMember);
	} else if(parser->fErrorCount)
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedMemberUnion), 0;
	}
	// member struct
	else if(rlc_parsed_member_struct_parse(
		&pack.fStruct,
		parser))
	{
		struct RlcParsedMemberStruct * struc = NULL;
		rlc_malloc((void**)&struc, sizeof(struct RlcParsedMemberStruct));

		*struc = pack.fStruct;
		ret = RLC_BASE_CAST(struc, RlcParsedMember);
	} else if(parser->fErrorCount)
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedMemberStruct), 0;
	}
	// member rawtype
	else if(rlc_parsed_member_rawtype_parse(
		&pack.fRawtype,
		parser))
	{
		struct RlcParsedMemberRawtype * rawtype = NULL;
		rlc_malloc((void**)&rawtype, sizeof(struct RlcParsedMemberRawtype));

		*rawtype = pack.fRawtype;
		ret = RLC_BASE_CAST(rawtype, RlcParsedMember);
	} else if(parser->fErrorCount)
	{
		return rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedMemberRawtype), 0;
	}
	// no success
	else
	{
		if(parser->fIndex != parser_index)
		{
			rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedMember);
		}
		return 0;
	}

	ret->fVisibility = visibility;
}