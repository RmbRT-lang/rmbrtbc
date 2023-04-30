#include "class.h"
#include "constructor.h"
#include "operatorexpression.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_class_create(
	struct RlcParsedClass * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedClass,
		name);

	if(templates)
		this->fTemplateDecl = *templates;
	else
		rlc_parsed_template_decl_create(&this->fTemplateDecl);

	this->fInheritances = NULL;
	this->fInheritanceCount = 0;

	rlc_parsed_member_list_create(&this->fConstructors);
	rlc_parsed_member_list_create(&this->fMembers);

	this->fHasDestructor = 0;
}

void rlc_parsed_class_destroy(
	struct RlcParsedClass * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_parsed_template_decl_destroy(&this->fTemplateDecl);

	rlc_parsed_member_list_destroy(&this->fMembers);
	rlc_parsed_member_list_destroy(&this->fConstructors);

	if(this->fHasDestructor)
	{
		rlc_parsed_destructor_destroy(&this->fDestructor);
		this->fHasDestructor = 0;
	}

	if(this->fInheritanceCount)
	{
		for(RlcSrcSize i = 0; i < this->fInheritanceCount; i++)
			rlc_parsed_symbol_destroy(&this->fInheritances[i].fBase);
		rlc_free((void**)&this->fInheritances);
		this->fInheritanceCount = 0;
	}
}

int rlc_parsed_class_parse(
	struct RlcParsedClass * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(templates != NULL);

	struct RlcToken name;
	if((!rlc_parser_is_ahead(parser, kRlcTokBraceOpen)
		&& !rlc_parser_is_ahead(parser, kRlcTokMinusGreater)
		&& !rlc_parser_is_ahead(parser, kRlcTokVirtual))
	|| !rlc_parser_consume(parser, &name, kRlcTokIdentifier))
	{
		return 0;
	}

	struct RlcParserTracer tracer;
	rlc_parser_trace(parser, "class", &tracer);

	rlc_parsed_class_create(out, &name.content, templates);

	out->fIsVirtual = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokVirtual);

	if(kRlcTokMinusGreater == rlc_parser_expect(
		parser,
		NULL,
		2,
		kRlcTokMinusGreater,
		kRlcTokBraceOpen))
	{
		do
		{
			rlc_realloc(
				(void**)&out->fInheritances,
				++out->fInheritanceCount * sizeof(struct RlcParsedInheritance));
			struct RlcParsedInheritance * in =
				&out->fInheritances[out->fInheritanceCount-1];

			int _ = rlc_visibility_parse(&in->fVisibility, parser, kRlcVisibilityPublic);
			(void) _;

			in->fVirtual = rlc_parser_consume(
				parser,
				NULL,
				kRlcTokVirtual);
			if(!rlc_parsed_symbol_parse(&in->fBase, parser))
				rlc_parser_fail(parser, "expected symbol");
		} while(kRlcTokComma == rlc_parser_expect(
			parser,
			NULL,
			2,
			kRlcTokComma,
			kRlcTokBraceOpen));
	}

	struct RlcParsedMemberCommon common;
	rlc_parsed_member_common_create(&common, kRlcVisibilityPublic);

	struct RlcParsedMember * member;
	while((member = rlc_parsed_member_parse(
		parser,
		&common,
		RLC_ALL_FLAGS(RlcParsedMemberType)
		& (out->fHasDestructor ? ~RLC_FLAG(kRlcParsedDestructor) : ~0))))
	{
		if(RLC_DERIVING_TYPE(member) == kRlcParsedDestructor)
		{
			out->fHasDestructor = 1;
			out->fDestructor = *RLC_DERIVE_CAST(
				member,
				RlcParsedMember,
				struct RlcParsedDestructor);
			rlc_free((void**)&member);
		} else if(RLC_DERIVING_TYPE(member) == kRlcParsedConstructor)
		{
			struct RlcParsedConstructor * ctor = RLC_DERIVE_CAST(
				member,
				RlcParsedMember,
				struct RlcParsedConstructor);

			if(!ctor->fCallsOtherCtor)
			{
				RlcSrcSize init_count = ctor->fInit.fInits.fBaseInitCount;

				if(init_count && init_count != out->fInheritanceCount)
					rlc_parser_fail(parser,
						"constructor has wrong number of base initialisers");
			}

			rlc_parsed_member_list_add(
				&out->fConstructors,
				member);
		} else
		{
			// add the member to the members list.
			rlc_parsed_member_list_add(
				&out->fMembers,
				member);
		}
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceClose);

	rlc_parser_untrace(parser, &tracer);

	return 1;
}

static void rlc_parsed_inheritance_print(
	struct RlcParsedInheritance const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(file != NULL);
	RLC_DASSERT(out != NULL);

	rlc_visibility_print(this->fVisibility, 0, out);

	if(this->fVirtual)
		fprintf(out, " virtual ");

	rlc_parsed_symbol_print_no_template(&this->fBase, file, out);
}

static void rlc_parsed_class_print_decl(
	struct RlcParsedClass const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	rlc_parsed_template_decl_print(&this->fTemplateDecl, file, out);
	fputs("class ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(";\n", out);
}

static void rlc_parsed_class_print_impl(
	struct RlcParsedClass const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer)
{
	struct RlcPrinterCtx ctx;
	rlc_printer_add_ctx(
		printer,
		&ctx,
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		&this->fTemplateDecl);

	FILE* out = printer->fTypesImpl;
	rlc_parsed_template_decl_print(&this->fTemplateDecl, file, out);
	fputs("class ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	if(this->fInheritanceCount)
	{
		fputc(':', out);
		for(RlcSrcIndex i = 0; i < this->fInheritanceCount; i++)
		{
			if(i)
				fputs(", ", out);

			rlc_parsed_inheritance_print(
				&this->fInheritances[i],
				file,
				out);
		}
	}

	fprintf(out, " { ");

	fputs("typedef ", out);
	rlc_printer_print_ctx_symbol(printer, file, out);
	fputs(" __rl_MY_T;\n", out);

	rlc_parsed_member_list_print(&this->fMembers, file, printer);


	// detect ctors
	int hasStructuralCtor = 0;
	int hasBareCtor = 0;
	int hasCopyCtor = 0;
	int hasMoveCtor = 0;
	int hasDefaultCtor = 0;
	enum RlcVisibility structCtorVisibility = kRlcVisibilityPublic;
	enum RlcVisibility copyCtorVisibility = kRlcVisibilityPublic;
	for(RlcSrcIndex i = 0; i < this->fConstructors.fEntryCount; i++)
	{
		struct RlcParsedConstructor * ctor = RLC_DERIVE_CAST(
			this->fConstructors.fEntries[i],
			RlcParsedMember,
			struct RlcParsedConstructor);

		enum RlcVisibility ctor_visibility =
			this->fConstructors.fEntries[i]->fVisibility;

		switch(ctor->fType)
		{
		case kRlcStructuralConstructor:
			{
				hasStructuralCtor = 1;
				structCtorVisibility = ctor_visibility;
			} break;
		case kRlcBareConstructor:
			{
				hasBareCtor = 1;
			} break;
		case kRlcCopyConstructor:
			{
				hasCopyCtor = 1;
				copyCtorVisibility = ctor_visibility;
			} break;
		case kRlcMoveConstructor: hasMoveCtor = 1; break;
		case kRlcDefaultConstructor: hasDefaultCtor = 1; break;
		default: break;
		}
	}

	//////////////////////
	// Get base address //
	//////////////////////

	fputs("public: struct __rl_identifier {};\n", out);

	static unsigned class_id = 1;
	fprintf(out, "enum { __rl_type_number_v = __rl::last_native_type_number + %u };\n", class_id++);

	fputs("static constexpr char const * __rl_type_name_v = \"", out);
	rlc_printer_print_ctx_symbol_with_namespace_rl_flavour(printer, file, out);
	fputs("\";\n", out);

	if(this->fIsVirtual)
	{
		fputs("virtual void const * __rl_get_derived(__rl_identifier const *) const = 0;\n", out);
		fputs("virtual char const * __rl_type_name(__rl_identifier const *) const = 0;\n", out);
		fputs("virtual unsigned __rl_type_number(__rl_identifier const *) const = 0;\n", out);

		fputs("virtual void __rl_virtual_constructor(__rl_identifier const *, ::__rl::default_init_t) = 0;\n", out);
		fputs("virtual void __rl_virtual_constructor(__rl_identifier const *, ::__rl::bare_init_t) = 0;\n", out);
		fputs("virtual void __rl_virtual_constructor(__rl_identifier const *, __rl_MY_T const&) = 0;\n", out);
		fputs("virtual void __rl_virtual_constructor(__rl_identifier const *, __rl_MY_T &&) = 0;\n", out);
	}
	else
	{
		fputs("inline void const * __rl_get_derived(__rl_identifier const *) const { return this; }\n", out);
		fputs("constexpr char const * __rl_type_name(__rl_identifier const *) const { return __rl_type_name_v; }\n", out);
		fputs("constexpr unsigned __rl_type_number(__rl_identifier const *) const { return __rl_type_number_v; }\n", out);

		if(!hasDefaultCtor)
			fputs("void __rl_virtual_constructor(__rl_identifier const *, ::__rl::default_init_t rhs) { throw \"virtual default ctor call: type has no default ctor.\"; }\n", out);
		else
			fputs("void __rl_virtual_constructor(__rl_identifier const *, ::__rl::default_init_t rhs) { new (this) __rl_MY_T(rhs); }\n", out);
		fputs("void __rl_virtual_constructor(__rl_identifier const *, ::__rl::bare_init_t rhs) { new (this) __rl_MY_T(rhs); }\n", out);
		if(!hasCopyCtor && hasMoveCtor)
			fputs("void __rl_virtual_constructor(__rl_identifier const *, __rl_MY_T const& rhs) { throw \"virtual copy ctor call: type has no copy ctor\"; }\n", out);
		else
			fputs("void __rl_virtual_constructor(__rl_identifier const *, __rl_MY_T const& rhs) { new (this) __rl_MY_T(rhs); }\n", out);
		if(!hasMoveCtor && hasCopyCtor)
			fputs("void __rl_virtual_constructor(__rl_identifier const *, __rl_MY_T &&rhs) { throw \"virtual move ctor call: type has no move ctor\"; }\n", out);
		else
			fputs("void __rl_virtual_constructor(__rl_identifier const *, __rl_MY_T &&rhs) { new (this) __rl_MY_T(std::move(rhs)); }\n", out);
	}

	for(RlcSrcIndex i = 0; i < this->fInheritanceCount; i++)
	{
		fputs("void const * __rl_get_derived(typename ", out);
		rlc_parsed_symbol_print_no_template(
			&this->fInheritances[i].fBase,
			file,
			out);

		fputs("::__rl_identifier const *) const { return __rl::real_addr(*this); }\n", out);

		fputs("char const * __rl_type_name(typename ", out);
		rlc_parsed_symbol_print_no_template(
			&this->fInheritances[i].fBase,
			file,
			out);

		fputs("::__rl_identifier const *) const { return __rl::type_name(*this); }\n", out);

		fputs("unsigned __rl_type_number(typename ", out);
		rlc_parsed_symbol_print_no_template(
			&this->fInheritances[i].fBase,
			file,
			out);

		fputs("::__rl_identifier const *) const { return __rl::type_number(*this); }\n", out);


		// virtual ctor
		fputs("void __rl_virtual_constructor(typename ", out);
		rlc_parsed_symbol_print_no_template(
			&this->fInheritances[i].fBase,
			file,
			out);
		fputs("::__rl_identifier const *, ::__rl::default_init_t rhs) { __rl::__rl_virtual_constructor(*this, rhs); }\n", out);


		// virtual BARE ctor
		fputs("void __rl_virtual_constructor(typename ", out);
		rlc_parsed_symbol_print_no_template(
			&this->fInheritances[i].fBase,
			file,
			out);
		fputs("::__rl_identifier const *, ::__rl::bare_init_t rhs) { __rl::__rl_virtual_constructor(*this, rhs); }\n", out);



		// virtual copy ctor
		fputs("void __rl_virtual_constructor(typename ", out);
		rlc_parsed_symbol_print_no_template(
			&this->fInheritances[i].fBase,
			file,
			out);
		fputs("::__rl_identifier const *, ", out);
		rlc_parsed_symbol_print_no_template(
			&this->fInheritances[i].fBase,
			file,
			out);
		fputs(" const& rhs) { __rl::__rl_virtual_constructor(*this, static_cast<__rl_MY_T const&>(rhs)); }\n", out);



		// virtual move ctor
		fputs("void __rl_virtual_constructor(typename ", out);
		rlc_parsed_symbol_print_no_template(
			&this->fInheritances[i].fBase,
			file,
			out);
		fputs("::__rl_identifier const *, ", out);
		rlc_parsed_symbol_print_no_template(
			&this->fInheritances[i].fBase,
			file,
			out);
		fputs(" &&rhs) { __rl::__rl_virtual_constructor(*this, static_cast<__rl_MY_T &&>(rhs)); }\n", out);
	}

	////////////////
	// Destructor //
	////////////////

	if(this->fHasDestructor)
	{
		RLC_ASSERT(this->fDestructor.fIsDefinition);

		rlc_visibility_print(
			RLC_BASE_CAST(&this->fDestructor, RlcParsedMember)->fVisibility,
			1,
			out);

		if(this->fDestructor.fIsInline)
			fputs("inline ", out);

		if(this->fIsVirtual)
			fputs("virtual ", out);
		fputc('~', out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("();\n", out);

		FILE * out = printer->fFuncsImpl;
		rlc_printer_print_ctx_tpl(printer, file, out);
		rlc_printer_print_ctx_symbol(printer, file, out);
		fputs("::~", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("()\n", out);
		fputs("#define _return return\n", out);
		rlc_parsed_block_statement_print(
			&this->fDestructor.fBody,
			file,
			out);
		fputs("#undef _return\n", out);
	}
	else
	{
		fputs("public:", out);
		if(this->fIsVirtual)
		{
			fputs("virtual ",out);
			fputs(" ~", out);
				rlc_src_string_print(
					&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
					file,
					out);
				fputs("() = default;\n", out);
		}
	}
	fputs("inline void __rl_destructor() const { this->~", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("(); }", out);


	////////////////
	// tuple ctor //
	////////////////

	fputs(
		"\n// tuple ctor helper: applies a tuple to any existing ctors.\n"
		"template<class...__RL_Types, std::size_t ...__RL_Indices>\n"
		"inline ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("(::__rl::TupleCtorHelper,"
		"::__rl::Tuple<__RL_Types...> && __rl_tuple,"
		"::std::index_sequence<__RL_Indices...>):\n", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("(::std::forward<__RL_Types>(::std::get<__RL_Indices>(__rl_tuple))...)\n"
		"{\n"
		"}\n", out);


	fputs("// tuple ctor: calls tuple ctor helper to apply tuples to ctors.\n"
		"template<class ...__RL_Types>\n"
		"inline ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("(::__rl::Tuple<__RL_Types...> &&__rl_tuple,\n"
		"::std::enable_if_t<::std::is_constructible_v<", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(", __RL_Types...>,\n"
		"\t::__rl::TupleCtorHelper> = __rl::tupleCtorHelper):\n", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("(::__rl::tupleCtorHelper,"
		"::std::move(__rl_tuple),"
		"::std::make_index_sequence<sizeof...(__RL_Types)>{})\n"
		"{\n"
		"}\n", out);

	// Add manual default ctor.
	for(int type = !!this->fConstructors.fEntryCount; type < 1 + !hasBareCtor; type++)
	{
		char const * default_init_arg = type
			? "::__rl::bare_init"
			: "::__rl::default_init";
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fprintf(out, "(%s_t)\n", default_init_arg);

		int printed = 0;
		for(RlcSrcIndex i = 0; i < this->fInheritanceCount; i++)
		{
			fputs(printed ? ",\n" : ":\n", out);
			printed = 1;
			fputc('\t', out);
			rlc_parsed_symbol_print(&this->fInheritances[i].fBase, file, out);
			fprintf(out, "(%s)", default_init_arg);
		}
		for(RlcSrcIndex i = 0; i < this->fMembers.fEntryCount; i++)
		{
			struct RlcParsedMemberVariable * v;
			if((v = RLC_DYNAMIC_CAST(
				this->fMembers.fEntries[i],
				RlcParsedMember,
				RlcParsedMemberVariable)))
			{
				struct RlcSrcString * name = &RLC_BASE_CAST(
						RLC_BASE_CAST(v, RlcParsedVariable),
						RlcParsedScopeEntry)->fName;
				if(!name->length)
					continue;

				if(RLC_BASE_CAST(v, RlcParsedMember)->fAttribute
				== kRlcMemberAttributeStatic)
					continue;

				fputs(printed ? ",\n" : ":\n", out);
				printed = 1;
				fputc('\t', out);
				rlc_src_string_print(name, file, out);
				fprintf(out, "(%s)", default_init_arg);
			}
		}
		fputs("\n{}\n", out);
	}

	// Make sure that THIS& also calls the copy ctor.
	if(hasCopyCtor)
	{
		rlc_visibility_print(copyCtorVisibility, 1, out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("(__rl_MY_T &__rl_arg): ", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("(const_cast<__rl_MY_T const&>(__rl_arg))\n{}\n", out);
	}

	if(!this->fConstructors.fEntryCount)
		// Add manual memberwise ctor if simple struct.
		if(!this->fInheritanceCount)
			hasStructuralCtor = 1;

	// Add manual memberwise ctor if simple struct.
	if(hasStructuralCtor)
	{
		rlc_visibility_print(
			structCtorVisibility,
			1,
			out);
		for(int init = 0; init <= 1; init++)
		{
			int printed = 0;
			if(init)
				for(RlcSrcIndex j = 0; j < this->fInheritanceCount; j++)
				{
					fputs(printed ? ", " : "): ", out);
					printed = 1;

					rlc_parsed_symbol_print(&this->fInheritances[j].fBase, file, out);
					fputs("(::__rl::default_init)", out);
				}
			for(RlcSrcIndex i = 0; i < this->fMembers.fEntryCount; i++)
			{
				struct RlcParsedMemberVariable * v;
				if((v = RLC_DYNAMIC_CAST(
					this->fMembers.fEntries[i],
					RlcParsedMember,
					RlcParsedMemberVariable)))
				{
					struct RlcSrcString * name = &RLC_BASE_CAST(
							RLC_BASE_CAST(v, RlcParsedVariable),
							RlcParsedScopeEntry)->fName;
					if(!name->length)
						continue;

					if(RLC_BASE_CAST(v, RlcParsedMember)->fAttribute
					== kRlcMemberAttributeStatic)
						continue;

					if(!init)
					{
						if(!printed)
						{
							rlc_src_string_print(
								&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
								file,
								out);
							fputs("(\n", out);
						} else
							fputs(",\n", out);

						RLC_DASSERT(RLC_BASE_CAST(v, RlcParsedVariable)->fHasType);
						rlc_parsed_type_name_print(
							&RLC_BASE_CAST(v, RlcParsedVariable)->fType,
							file,
							out);
						fputs(" __rl_arg_", out);
						rlc_src_string_print(name, file, out);
					} else
					{
						fputs(!printed ? "):\n" : ",\n", out);
						fputc('\t', out);
						rlc_src_string_print(name, file, out);
						fputs("(std::move(__rl_arg_", out);
						rlc_src_string_print(name, file, out);
						fputs("))", out);
					}
					printed = 1;
				}
			}
			if(init && printed)
				fputs("\n{}\n", out);
		}
	}

	////////////////////////
	// NOINIT constructor //
	////////////////////////

	fputs("public: inline ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("() {}\n", out);

	// Default ctors and assignments for virtual classes.
	if(this->fIsVirtual)
	{
		if(!hasMoveCtor)
		{
			rlc_src_string_print(
				&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
				file,
				out);
			fputs("(__rl_MY_T&&) = default;\n", out);
		}
		if(!hasCopyCtor)
		{
			rlc_src_string_print(
				&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
				file,
				out);
			fputs("(__rl_MY_T const&) = default;\n", out);
		}

		fputs("__rl_MY_T& operator=(__rl_MY_T&&) = default;\n", out);
		fputs("__rl_MY_T& operator=(__rl_MY_T const&) = default;\n", out);
	} else {
		struct RlcSrcString clsName = RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName;

		fprintf(out, "template<class _tpl_type_1> __rl_MY_T& operator=(_tpl_type_1 &&_rhs)\n{\n"
			"	if constexpr(std::is_same<__rl_MY_T, std::decay_t<_tpl_type_1>>())\n"
			"		__rl_assert(this != &_rhs, (self-assignments are forbidden!), \"%s\", %u, %u);\n"
			"	__rl_destructor();\n"
			"	__rl::__rl_p_constructor(this, std::forward<_tpl_type_1>(_rhs));\n"
			"	return *this;\n"
			"}\n",
			file->fName,
			clsName.line,
			clsName.column);
	}


	////////////////////
	// tuple ctor end //
	////////////////////

	///////////
	// Visit //
	///////////

	for(int isReflect = 0; isReflect <= 1; isReflect++)
		for(int isConst = 0; isConst <= 1; isConst++)
		{
			fputs("template<class __rl_Fn, class ...__rl_Args>\n"
				"void __rl_visit", out);
			if(isReflect)
				fputs("_reflect", out);
			fputs("(__rl_Fn &&fn, __rl_Args&&... args)", out);
			if(isConst) fputs(" const", out);
			fputs(" {\n", out);

			for(RlcSrcIndex i = 0; i < this->fInheritanceCount; i++)
			{
				struct RlcParsedInheritance const * base = &this->fInheritances[i];
				if(base->fVisibility == kRlcVisibilityPublic)
				{
					fputs("fn(", out);
					if(isReflect)
					{
						rlc_parsed_symbol_print(&base->fBase, file, out);
						fputs("::__rl_type_name_v, ", out);
					}
					fputs("(", out);
					rlc_parsed_symbol_print(&base->fBase, file, out);
					if(isConst)
						fputs(" const", out);
					fputs("&)*this, ::std::forward<__rl_Args>(args)...);\n", out);
				}
			}
			for(RlcSrcIndex i = 0; i < this->fMembers.fEntryCount; i++)
			{
				struct RlcParsedMember const * member = this->fMembers.fEntries[i];
				if(member->fVisibility != kRlcVisibilityPublic)
					continue;
				struct RlcParsedMemberVariable * memVar;
				if(!(memVar = RLC_DYNAMIC_CAST(member, RlcParsedMember, RlcParsedMemberVariable)))
					continue;

				struct RlcParsedScopeEntry const * scopeEntry =
					RLC_BASE_CAST2(memVar, RlcParsedVariable, RlcParsedScopeEntry);
				if(!scopeEntry->fName.length)
					continue;

				if(member->fVisibility == kRlcVisibilityPublic)
				{
					fputs("fn(", out);
					if(isReflect)
					{
						fputc('"', out);
						rlc_src_string_print(&scopeEntry->fName, file, out);
						fputs("\", ", out);
					}
					fputs("this->", out);
					rlc_src_string_print(&scopeEntry->fName, file, out);
					fputs(", ::std::forward<__rl_Args>(args)...);\n", out);
				}
			}
			fputs("}\n", out);
		}

	///////////////
	// Visit end //
	///////////////

	for(RlcSrcIndex i = 0; i < this->fConstructors.fEntryCount; i++)
	{
		struct RlcParsedConstructor * ctor = RLC_DERIVE_CAST(
			this->fConstructors.fEntries[i],
			RlcParsedMember,
			struct RlcParsedConstructor);

		if(ctor->fType == kRlcStructuralConstructor)
			continue;

		rlc_visibility_print(
			this->fConstructors.fEntries[i]->fVisibility,
			1,
			out);

		rlc_parsed_template_decl_print(&ctor->fTemplates, file, out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputc('(', out);

		switch(ctor->fType)
		{
		case kRlcDefaultConstructor: fputs("::__rl::default_init_t", out); break;
		case kRlcBareConstructor: fputs("::__rl::bare_init_t", out); break;
		case kRlcCopyConstructor:
		case kRlcMoveConstructor:
			{
				fputs("__rl_MY_T ", out);
				fputs(ctor->fType == kRlcCopyConstructor ? "const" : "&", out);
				fputc('&', out);

				rlc_src_string_print(&ctor->fArgName, file, out);
			} break;
		case kRlcStructuralConstructor:
			{
				RLC_DASSERT(!"This must never happen");
			} break;
		case kRlcCustomConstructor:
			{
				if(ctor->fIsVariant)
				{
					rlc_parsed_symbol_constant_type_print(
						&ctor->fVariant, file, out);
					if(ctor->fArgumentCount)
						fputs(", ", out);
				}

				for(RlcSrcIndex j = 0; j < ctor->fArgumentCount; j++)
				{
					if(j)
						fputs(", ", out);
					rlc_parsed_variable_print_argument(
						&ctor->fArguments[j],
						file,
						out,
						0);
				}
			} break;
		}

		fputc(')', out);
		if(ctor->fType != kRlcCustomConstructor
		&& !ctor->fIsDefinition
		&& !ctor->fCallsOtherCtor
		&& !ctor->fInit.fInits.fInitialiserCount)
		{
			if(ctor->fType == kRlcDefaultConstructor
			|| ctor->fType == kRlcBareConstructor)
			{
				char const * init_arg = ctor->fType == kRlcBareConstructor
					? "(::__rl::bare_init)"
					: "(::__rl::default_init)";

				int printed = 0;
				for(RlcSrcIndex i = 0; i < this->fInheritanceCount; i++)
				{
					fputs(printed ? ", ": ": ", out);
					printed = 1;

					rlc_parsed_symbol_print(
						&this->fInheritances[i].fBase, file, out);
					fputs(init_arg, out);
				}
				for(RlcSrcIndex i = 0; i < this->fMembers.fEntryCount; i++)
				{
					struct RlcParsedMemberVariable * v;
					if((v = RLC_DYNAMIC_CAST(
						this->fMembers.fEntries[i],
						RlcParsedMember,
						RlcParsedMemberVariable)))
					{
						struct RlcSrcString * field = &RLC_BASE_CAST(
							RLC_BASE_CAST(v, RlcParsedVariable),
							RlcParsedScopeEntry)->fName;

						if(!field->length)
							continue;

						if(RLC_BASE_CAST(v, RlcParsedMember)->fAttribute
						== kRlcMemberAttributeStatic)
							continue;

						fputs(printed ? ", ": ": ", out);
						printed = 1;

						rlc_src_string_print(field, file, out);
						fputs(init_arg, out);
					}
				}
				fputs("{}\n", out);
			} else {
				fputs(" = default;\n", out);
			}
			continue;
		}

		fputs(";\n", out);

		FILE * out = printer->fFuncsImpl;
		rlc_printer_print_ctx_tpl(printer, file, out);
		rlc_parsed_template_decl_print(&ctor->fTemplates, file, out);
		rlc_printer_print_ctx_symbol(printer, file, out);
		fputs("::", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputc('(', out);

		switch(ctor->fType)
		{
		case kRlcStructuralConstructor: break;
		case kRlcDefaultConstructor: fputs("::__rl::default_init_t", out); break;
		case kRlcBareConstructor: fputs("::__rl::bare_init_t", out); break;
		case kRlcCopyConstructor:
		case kRlcMoveConstructor:
			{
				fputs("__rl_MY_T ", out);
				fputs(ctor->fType == kRlcCopyConstructor ? "const" : "&", out);
				fputc('&', out);

				rlc_src_string_print(&ctor->fArgName, file, out);
			} break;
		case kRlcCustomConstructor:
			{
				if(ctor->fIsVariant)
				{
					rlc_parsed_symbol_constant_type_print(
						&ctor->fVariant, file, out);
					if(ctor->fArgumentCount)
						fputs(", ", out);
				}

				for(RlcSrcIndex j = 0; j < ctor->fArgumentCount; j++)
				{
					if(j)
						fputs(", ", out);
					rlc_parsed_variable_print_argument(
						&ctor->fArguments[j],
						file,
						out,
						0);
				}
			} break;
		}

		fputs(")\n", out);

		if(!ctor->fCallsOtherCtor)
		{
			char const * default_init_arg = ctor->fType == kRlcBareConstructor
				? "(::__rl::bare_init)"
				: "(::__rl::default_init)";
			int printed_init = 0;
			for(RlcSrcIndex m = 0; m < this->fMembers.fEntryCount; m++)
			{
				struct RlcParsedMemberVariable * v;
				if((v = RLC_DYNAMIC_CAST(
					this->fMembers.fEntries[m],
					RlcParsedMember,
					RlcParsedMemberVariable)))
				{
					struct RlcSrcString * field = &RLC_BASE_CAST(
						RLC_BASE_CAST(v, RlcParsedVariable),
						RlcParsedScopeEntry)->fName;

					if(!field->length)
						continue;

					if(RLC_BASE_CAST(v, RlcParsedMember)->fAttribute
					== kRlcMemberAttributeStatic)
						continue;

					int has_init = 0;
					for(RlcSrcIndex init = 0; init < ctor->fInit.fInits.fInitialiserCount; init++)
						if(!rlc_src_string_cmp(
							file, field, &ctor->fInit.fInits.fInitialisers[init].fMember))
						{
							has_init = 1;
							break;
						}

					if(!has_init)
					{
						fputs(printed_init ? ",\n" : ":\n", out);
						printed_init = 1;
						fputc('\t', out);

						rlc_src_string_print(field, file, out);
						fputs(default_init_arg, out);
					}
				}
			}

			if(ctor->fInit.fInits.fBaseInitCount)
			{
				RLC_DASSERT(ctor->fInit.fInits.fBaseInitCount == this->fInheritanceCount);
				for(RlcSrcIndex j = 0; j < this->fInheritanceCount; j++)
				{
					struct RlcParsedBaseInit * init = &ctor->fInit.fInits.fBaseInits[j];
					if(init->fInitType == kRlcInitTypeNoInit)
						continue;

					fputs(printed_init ? ", " : ": ", out);
					printed_init = 1;

					rlc_parsed_symbol_print(&this->fInheritances[j].fBase, file, out);
					fputc('(', out);

					switch(init->fArgumentCount)
					{
					case 0:
						switch(init->fInitType)
						{
						case kRlcInitTypeArguments:
							fputs("::__rl::default_init", out); break;
						case kRlcInitTypeBare:
							fputs("::__rl::bare_init", out); break;
						case kRlcInitTypeNoInit: break;
						} break;
					case 1:
						{
							fputs("__rl::single_ctor_arg(", out);
							rlc_parsed_expression_print(init->fArguments[0], file, out);
							fputc(')', out);
						} break;
					default:
						for(RlcSrcIndex k = 0; k < init->fArgumentCount; k++)
						{
							if(k)
								fputs(", ", out);
							rlc_parsed_expression_print(init->fArguments[k], file, out);
						}
					}
					fputs(")\n", out);
				}
			}
			else
				for(RlcSrcIndex j = 0; j < this->fInheritanceCount; j++)
				{
					fputs(printed_init ? ", " : ": ", out);
					printed_init = 1;

					rlc_parsed_symbol_print(&this->fInheritances[j].fBase, file, out);
					fputs(default_init_arg, out);
				}

			for(RlcSrcIndex j = 0; j < ctor->fInit.fInits.fInitialiserCount; j++)
			{
				struct RlcParsedInitialiser * init = &ctor->fInit.fInits.fInitialisers[j];
				if(init->fInitType == kRlcInitTypeNoInit)
					continue;

				fputs(printed_init ? ", " : ": ", out);
				printed_init = 1;
				rlc_printer_adjust_position(out, &init->fMember);
				rlc_src_string_print(&init->fMember, file, out);
				fputc('(', out);
				if(!init->fArgumentCount)
					switch(init->fInitType)
					{
					case kRlcInitTypeArguments:
						fputs("::__rl::default_init", out); break;
					case kRlcInitTypeBare:
						fputs("::__rl::bare_init", out); break;
					case kRlcInitTypeNoInit: break;
					}
				else if(init->fArgumentCount == 1)
				{
					int printClose = 0;
					struct RlcParsedOperatorExpression * exp;
					if((exp = RLC_DYNAMIC_CAST(init->fArguments[0],
						RlcParsedExpression,
						RlcParsedOperatorExpression)))
					{
						if((printClose = exp->fOperator == kVariadicExpand))
							fputs("::__rl::single_ctor_arg(", out);
					}
					rlc_parsed_expression_print(init->fArguments[0], file, out);
					if(printClose)
						fputs(")", out);
				}
				else
					for(RlcSrcIndex k = 0; k < init->fArgumentCount; k++)
					{
						if(k)
							fputs(", ", out);
						rlc_parsed_expression_print(init->fArguments[k], file, out);
					}
				fputs(")\n", out);
			}
		} else
		{
			fputs(": ", out);
			rlc_src_string_print(&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName, file, out);
			fputc('(', out);
			switch(ctor->fInit.fOtherCtorCallArgCount)
			{
			case 0: fputs("__rl::default_init", out); break;
			case 1:
				{
					fputs("__rl::single_ctor_arg(", out);
					rlc_parsed_expression_print(ctor->fInit.fOtherCtorCallArgs[0], file, out);
					fputc(')', out);
				} break;
			default:
				for(RlcSrcIndex i = 0; i < ctor->fInit.fOtherCtorCallArgCount; i++)
				{
					if(i) fputs(", ", out);
					rlc_parsed_expression_print(ctor->fInit.fOtherCtorCallArgs[i], file, out);
				}
			}
			fputc(')', out);
		}

		if(ctor->fIsDefinition)
		{
			fputs("\n#define _return return\n", out);
			rlc_parsed_block_statement_print(&ctor->fBody, file, out);
			fputs("\n#undef _return\n", out);
		} else fputs("{;}\n", out);
	}

	fprintf(out, " };\n");
	rlc_printer_pop_ctx(printer);
}

void rlc_parsed_class_print(
	struct RlcParsedClass const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(printer != NULL);


	rlc_parsed_class_print_decl(
		this,
		file,
		printer->fTypes);

	rlc_parsed_class_print_impl(
		this,
		file,
		printer);
}

void rlc_parsed_member_class_create(
	struct RlcParsedMemberClass * this,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(member != NULL);
	RLC_DASSERT(member->attribute == kRlcMemberAttributeNone);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberClass,
		member);
}

void rlc_parsed_member_class_destroy(
	struct RlcParsedMemberClass * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_class_destroy(
		RLC_BASE_CAST(this, RlcParsedClass));
	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
}

int rlc_parsed_member_class_parse(
	struct RlcParsedMemberClass * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(member != NULL);

	if(member->attribute != kRlcMemberAttributeNone
	|| !rlc_parsed_class_parse(
		RLC_BASE_CAST(out, RlcParsedClass),
		parser,
		&member->templates))
		return 0;

	rlc_parsed_member_class_create(
		out,
		member);

	return 1;
}

void rlc_parsed_member_class_print(
	struct RlcParsedMemberClass const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer)
{
	rlc_visibility_print(
		RLC_BASE_CAST(this, RlcParsedMember)->fVisibility,
		1,
		printer->fTypesImpl);
	rlc_parsed_class_print_impl(
		RLC_BASE_CAST(this, RlcParsedClass),
		file,
		printer);
}