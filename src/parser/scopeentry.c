#include "scopeentry.h"

#include "class.h"
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

	;
}


void rlc_parsed_scope_entry_destroy_virtual(
	struct RlcParsedScopeEntry * this)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(RLC_DERIVING_TYPE(this), RlcParsedScopeEntryType));

	typedef void (*destructor_t)(
		void * this);
	static destructor_t const k_vtable[] = {
		(destructor_t)&rlc_parsed_class_destroy,
		(destructor_t)&rlc_parsed_rawtype_destroy,
		(destructor_t)&rlc_parsed_union_destroy,
		(destructor_t)&rlc_parsed_namespace_destroy,
		(destructor_t)&rlc_parsed_function_destroy,
		(destructor_t)&rlc_parsed_variable_destroy,
		(destructor_t)&rlc_parsed_enum_destroy,
		(destructor_t)&rlc_parsed_enum_constant_destroy,
		(destructor_t)&rlc_parsed_typedef_destroy,
		(destructor_t)&rlc_parsed_external_symbol_destroy
	};

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedScopeEntryType), "ill-sized vtable.");

	static intptr_t const k_offsets[] = {
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedClass),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedRawtype),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedUnion),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedNamespace),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedFunction),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedVariable),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedEnum),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedEnumConstant),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedTypedef),
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct RlcParsedExternalSymbol)
	};

	static_assert(RLC_COVERS_ENUM(k_offsets, RlcParsedScopeEntryType), "ill-sized offset table.");

	uint8_t * p = (uint8_t*)this + k_offsets[RLC_DERIVING_TYPE(this)];
	k_vtable[RLC_DERIVING_TYPE(this)](p);

	rlc_free((void**)&p);
}

void rlc_parsed_scope_entry_create(
	struct RlcParsedScopeEntry * this,
	enum RlcParsedScopeEntryType derivingType,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);

	this->fName = *name;

	RLC_DERIVING_TYPE(this) = derivingType;
}

static int dummy_rlc_parsed_variable_parse(
	struct RlcParsedVariable * variable,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(variable != NULL);
	RLC_DASSERT(parser != NULL);

	if(rlc_parsed_variable_parse(variable, parser, templates, 1, 1, 0, 1, 0))
	{
		struct RlcParserTracer tracer;
		rlc_parser_trace(parser, "variable statement", &tracer);

		if(rlc_parsed_template_decl_exists(templates))
			rlc_parser_fail(parser, "variables must not have templates");

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokSemicolon);
		rlc_parser_untrace(parser, &tracer);
		return 1;
	}

	return 0;
}

static int dummy_rlc_parsed_function_parse(
	struct RlcParsedFunction * function,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(function != NULL);
	RLC_DASSERT(parser != NULL);

	return rlc_parsed_function_parse(function, parser, templates, 1);
}


struct RlcParsedScopeEntry * rlc_parsed_scope_entry_parse(
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);

	union Pack {
		struct RlcParsedVariable fVariable;
		struct RlcParsedFunction fFunction;
		struct RlcParsedClass fClass;
		struct RlcParsedUnion fUnion;
		struct RlcParsedRawtype fRawtype;
		struct RlcParsedTypedef fTypedef;
		struct RlcParsedNamespace fNamespace;
		struct RlcParsedEnum fEnum;
		struct RlcParsedExternalSymbol fExternalSymbol;
	} pack;

	typedef int (*parse_fn_t)(
		union Pack *,
		struct RlcParser *,
		struct RlcParsedTemplateDecl const * templates);


#define ENTRY(Type, parse) { \
		(parse_fn_t)parse, \
		sizeof(struct Type), \
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct Type) }

	static struct {
		parse_fn_t fParseFn;
		size_t fTypeSize;
		size_t fOffset;
	} const k_parse_lookup[] = {
		ENTRY(RlcParsedVariable, &dummy_rlc_parsed_variable_parse),
		ENTRY(RlcParsedFunction, &dummy_rlc_parsed_function_parse),
		ENTRY(RlcParsedClass, &rlc_parsed_class_parse),
		ENTRY(RlcParsedUnion, &rlc_parsed_union_parse),
		ENTRY(RlcParsedRawtype, &rlc_parsed_rawtype_parse),
		ENTRY(RlcParsedTypedef, &rlc_parsed_typedef_parse),
		ENTRY(RlcParsedNamespace, &rlc_parsed_namespace_parse),
		ENTRY(RlcParsedEnum, &rlc_parsed_enum_parse),
		ENTRY(RlcParsedEnumConstant, NULL), // Must not be called.
		ENTRY(RlcParsedExternalSymbol, &rlc_parsed_external_symbol_parse)
	};
#undef ENTRY

	static_assert(RLC_COVERS_ENUM(k_parse_lookup, RlcParsedScopeEntryType), "ill-sized parse table.");

	struct RlcParsedTemplateDecl templates;
	rlc_parsed_template_decl_parse(
		&templates,
		parser);

	for(size_t i = 0; i < _countof(k_parse_lookup); i++)
	{
		if(k_parse_lookup[i].fParseFn
		&& k_parse_lookup[i].fParseFn(
			&pack,
			parser,
			&templates))
		{
			void * temp = NULL;
			rlc_malloc(&temp, k_parse_lookup[i].fTypeSize);

			memcpy(temp, &pack, k_parse_lookup[i].fTypeSize);

			struct RlcParsedScopeEntry * ret;
			ret = (void*) ((uint8_t*)temp + k_parse_lookup[i].fOffset);
			return ret;
		}
	}

	return NULL;
}

void rlc_parsed_scope_entry_print(
	struct RlcParsedScopeEntry const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * printer)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(printer != NULL);

	typedef void (*print_fn_t)(
		void const *,
		struct RlcSrcFile const *,
		struct RlcPrinter const *);


#define ENTRY(Type, print) { \
		(print_fn_t)print, \
		RLC_DERIVE_OFFSET(RlcParsedScopeEntry, struct Type), \
		k##Type }

	static struct {
		print_fn_t fPrintFn;
		size_t fOffset;
		enum RlcParsedScopeEntryType type;
	} const k_vtable[] = {
		ENTRY(RlcParsedClass, &rlc_parsed_class_print),
		ENTRY(RlcParsedRawtype, &rlc_parsed_rawtype_print),
		ENTRY(RlcParsedUnion, &rlc_parsed_union_print),
		ENTRY(RlcParsedNamespace, &rlc_parsed_namespace_print),
		ENTRY(RlcParsedFunction, &rlc_parsed_function_print),
		ENTRY(RlcParsedVariable, &rlc_parsed_variable_print),
		ENTRY(RlcParsedEnum, &rlc_parsed_enum_print),
		ENTRY(RlcParsedEnumConstant, NULL), // Must not be called.
		ENTRY(RlcParsedTypedef, &rlc_parsed_typedef_print),
		ENTRY(RlcParsedExternalSymbol, &rlc_parsed_external_symbol_print)
	};
#undef ENTRY

	static_assert(RLC_COVERS_ENUM(k_vtable, RlcParsedScopeEntryType), "ill-sized parse table.");
	RLC_DASSERT(k_vtable[RLC_DERIVING_TYPE(this)].type == RLC_DERIVING_TYPE(this));


	k_vtable[RLC_DERIVING_TYPE(this)].fPrintFn(
		((char*)this) + k_vtable[RLC_DERIVING_TYPE(this)].fOffset,
		file,
		printer);
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
		rlc_parsed_scope_entry_destroy_virtual(this->fEntries[i]);
	this->fEntryCount = 0;
	if(this->fEntries)
		rlc_free((void**)&this->fEntries);
}

void rlc_parsed_scope_entry_list_print(
	struct RlcParsedScopeEntryList const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * printer)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(printer != NULL);

	for(RlcSrcIndex i = 0; i < this->fEntryCount; i++)
	{
		rlc_parsed_scope_entry_print(
			this->fEntries[i],
			file,
			printer);
	}
}