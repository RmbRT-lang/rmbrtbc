#include "scopeentry.h"

#include "class.h"
#include "struct.h"
#include "rawtype.h"
#include "union.h"
#include "namespace.h"
#include "function.h"
#include "variable.h"
#include "enum.h"
#include "typedef.h"
#include "externalsymbol.h"

#include "../macros.h"
#include "../assert.h"

#include "../malloc.h"

#include <string.h>

void rlc_parsed_scope_entry_destroy_base(
	struct RlcParsedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);

	if(this->fNames)
	{
		rlc_free((void**)&this->fNames);
		this->fNameCount = 0;
	}
}


void rlc_parsed_scope_entry_destroy_virtual(
	struct RlcParsedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcParsedScopeEntryType));

	typedef void const * (*destructor_t)(
		void * this);
	static destructor_t const k_vtable[] = {
		(destructor_t)&rlc_parsed_class_destroy,
		(destructor_t)&rlc_parsed_struct_destroy,
		(destructor_t)&rlc_parsed_rawtype_destroy,
		(destructor_t)&rlc_parsed_union_destroy,
		(destructor_t)&rlc_parsed_namespace_destroy,
		(destructor_t)&rlc_parsed_function_destroy,
		(destructor_t)&rlc_parsed_variable_destroy,
		(destructor_t)&rlc_parsed_enum_destroy,
		(destructor_t)&rlc_parsed_typedef_destroy,
		(destructor_t)&rlc_parsed_external_symbol_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedScopeEntryType), "ill-sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedClass),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedStruct),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedRawtype),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedUnion),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedNamespace),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedFunction),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedVariable),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedEnum),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedTypedef),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedExternalSymbol)
	};

	static_assert(RLC_COVERS_ENUM(k_offsets, RlcParsedScopeEntryType), "ill-sized offset table.");

	k_vtable[RLC_DERIVING_TYPE(this)]((uint8_t*)this + k_offsets[RLC_DERIVING_TYPE(this)]);
}

void rlc_parsed_scope_entry_create(
	struct RlcParsedScopeEntry * this,
	enum RlcParsedScopeEntryType derivingType,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);

	this->fLocation.fBegin = start_index;
	this->fLocation.fEnd = 0;
	this->fName = *name;

	RLC_DERIVING_TYPE(this) = derivingType;
}

static int dummy_rlc_parsed_variable_parse(
	struct RlcParsedVariable * variable,
	struct RlcParserData * parser)
{
	RLC_DASSERT(variable != NULL);
	RLC_DASSERT(parser != NULL);

	if(rlc_parsed_variable_parse(variable, parser, 1, 0, 0, 1, 0))
	{
		if(!rlc_parser_data_consume(
			parser,
			kRlcTokSemicolon))
		{
			rlc_parsed_variable_destroy(variable);
			rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedSemicolon);
			return 0;
		} else return 1;
	} else if(parser->fErrorCount)
	{
		rlc_parser_data_add_error(
			parser,
			kRlcParseErrorExpectedVariable);
		return 0;
	}
}


struct RlcParsedScopeEntry * rlc_parsed_scope_entry_parse(
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);

	union Pack {
		struct RlcParsedVariable fVariable;
		struct RlcParsedFunction fFunction;
		struct RlcParsedClass fClass;
		struct RlcParsedUnion fUnion;
		struct RlcParsedStruct fStruct;
		struct RlcParsedRawtype fRawtype;
		struct RlcParsedTypedef fTypedef;
		struct RlcParsedNamespace fNamespace;
		struct RlcParsedEnum fEnum;
	} pack;

	typedef int (*parse_fn_t)(
		union Pack *,
		struct RlcParserData *);


#define ENTRY(Type, parse, error) { \
		(parse_fn_t)parse, \
		error, \
		sizeof(struct Type), \
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct Type) }

	static struct {
		parse_fn_t fParseFn;
		enum RlcParseError fErrorCode;
		size_t fTypeSize;
		size_t fOffset;
	} const k_parse_lookup[] = {
		ENTRY(RlcParsedVariable, &dummy_rlc_parsed_variable_parse, kRlcParseErrorExpectedVariable),
		ENTRY(RlcParsedFunction, &rlc_parsed_function_parse, kRlcParseErrorExpectedFunction),
		ENTRY(RlcParsedClass, &rlc_parsed_class_parse, kRlcParseErrorExpectedClass),
		ENTRY(RlcParsedUnion, &rlc_parsed_union_parse, kRlcParseErrorExpectedUnion),
		ENTRY(RlcParsedStruct, &rlc_parsed_struct_parse, kRlcParseErrorExpectedStruct),
		ENTRY(RlcParsedRawtype, &rlc_parsed_rawtype_parse, kRlcParseErrorExpectedRawtype),
		ENTRY(RlcParsedTypedef, &rlc_parsed_typedef_parse, kRlcParseErrorExpectedTypedef),
		ENTRY(RlcParsedNamespace, &rlc_parsed_namespace_parse, kRlcParseErrorExpectedNamespace),
		ENTRY(RlcParsedEnum, &rlc_parsed_enum_parse, kRlcParseErrorExpectedEnum),
		ENTRY(RlcParsedExternalSymbol, &rlc_parsed_external_symbol_parse, kRlcParseErrorExpectedExternalSymbol)
	};

	static_assert(RLC_COVERS_ENUM(k_parse_lookup, RlcParsedScopeEntryType), "ill-sized parse table.");

	for(int i = 0; i < _countof(k_parse_lookup); i++)
	{
		if(k_parse_lookup[i].fParseFn(
			&pack,
			parser))
		{
			void * temp = NULL;
			rlc_malloc(&temp, k_parse_lookup[i].fTypeSize);

			memcpy(temp, &pack, k_parse_lookup[i].fTypeSize);

			struct RlcParsedScopeEntry * ret;
			ret = (void*) ((uint8_t*)temp + k_parse_lookup[i].fOffset);
			return ret;
		} else if(parser->fErrorCount)
		{
			rlc_parser_data_add_error(
				parser,
				k_parse_lookup[i].fErrorCode);
			return NULL;
		}
	}

	return NULL;
}

void rlc_parsed_scope_entry_list_create(
	struct RlcParsedScopeEntryList * this)
{
	RLC_DASSERT(this != NULL);

	this->fEntries = NULL;
	this->fEntryCount = 0;
}

void rlc_parsed_scope_entry_list_add(
	struct RlcParsedScopeEntryList * this,
	struct RlcParsedScopeEntry * entry)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(entry != NULL);

	rlc_realloc(
		(void**)&this->fEntries,
		sizeof(struct RlcParsedScopeEntry *) * ++this->fEntryCount);

	this->fEntries[this->fEntryCount -1] = entry;
}

void rlc_parsed_scope_entry_list_destroy(
	struct RlcParsedScopeEntryList * this)
{
	RLC_DASSERT(this != NULL);

	for(size_t i = 0; i < this->fEntryCount; i++)
	{
		rlc_parsed_scope_entry_destroy_virtual(
			this->fEntries[i]);
		rlc_free((void**)&this->fEntries[i]);
	}
	this->fEntryCount = 0;
	if(this->fEntries)
		rlc_free((void**)&this->fEntries);
}