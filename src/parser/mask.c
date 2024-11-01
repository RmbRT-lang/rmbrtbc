#include "mask.h"
#include "../malloc.h"

void rlc_parsed_mask_create(
	struct RlcParsedMask * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedMask,
		name);

	if(templates)
		this->fTemplates = *templates;
	else
		rlc_parsed_template_decl_create(&this->fTemplates);

	this->fFunctions = NULL;
	this->fFunctionCount = 0;
}

void rlc_parsed_mask_destroy(
	struct RlcParsedMask * this)
{
	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_parsed_template_decl_destroy(&this->fTemplates);

	for(RlcSrcSize i = 0; i < this->fFunctionCount; i++)
		rlc_parsed_member_function_destroy(&this->fFunctions[i]);

	if(this->fFunctions)
	{
		rlc_free((void**)&this->fFunctions);
		this->fFunctionCount = 0;
	}
}

int rlc_parsed_mask_parse(
	struct RlcParsedMask * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokMask))
		return 0;

	struct RlcParserTracer t;
	rlc_parser_trace(parser, "mask", &t);

	struct RlcToken name;
	rlc_parser_expect(
		parser,
		&name,
		1,
		kRlcTokIdentifier);

	rlc_parsed_mask_create(
		out,
		&name.content,
		templates);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceOpen);

	struct RlcParsedMemberCommon common;
	rlc_parsed_member_common_create(&common, kRlcVisibilityPublic);
	struct RlcParsedMemberFunction memfn;
	int mask_fns = 0; // Number of functions that are not implemented.
	for(int i = 0;; i++)
	{
		int expect = rlc_parsed_member_common_parse(&common, parser);
		if(!rlc_parsed_member_function_parse(&memfn, parser, &common))
		{
			if(!i || expect || !mask_fns)
				rlc_parser_fail(parser, "expected member function");
			else
				break;
		}

		if(memfn.fAbstractness != kRlcMemberFunctionAbstractnessNone)
			rlc_parser_fail(parser, "mask function must be non-virtual, non-override");

		if(!RLC_BASE(&memfn, RlcParsedFunction)->fHasBody)
			++mask_fns;

		rlc_realloc(
			(void**)&out->fFunctions,
			++out->fFunctionCount * sizeof(struct RlcParsedMemberFunction));
		out->fFunctions[out->fFunctionCount-1] = memfn;
	}

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceClose);

	rlc_parser_untrace(parser, &t);
	return 1;
}

static void rlc_parsed_mask_print_decl(
	struct RlcParsedMask const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	rlc_parsed_template_decl_print(&this->fTemplates, file, out);
	fputs("class ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(";\n", out);

	fputs("template<class __rl_mask_type", out);
	for(RlcSrcSize i = 0; i < this->fTemplates.fChildCount; i++)
	{
		fputs(", ", out);
		rlc_parsed_template_decl_child_print(
			&this->fTemplates.fChildren[i], file, out);
	}
	fputs("> class ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("_wrapper;\n", out);
}

static void rlc_parsed_mask_print_impl(
	struct RlcParsedMask const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer)
{
	struct RlcPrinterCtx ctx;
	rlc_printer_add_ctx(
		printer,
		&ctx,
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		&this->fTemplates);

	FILE * out = printer->fTypesImpl;
	rlc_parsed_template_decl_print(&this->fTemplates, file, out);
	fputs("class ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(" { public:\n", out);

	fputs("static constexpr char const * __rl_type_name_v = \"", out);
	rlc_printer_print_ctx_symbol_with_namespace_rl_flavour(printer, file, out);
	fputs("\";\n", out);

	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("() = default;\n", out);

	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("(::__rl::default_init_t) {}\n", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("(::__rl::bare_init_t) {}\n", out);

	fputs("struct __rl_identifier {};\n", out);
	fputs("virtual void const * __rl_get_derived(__rl_identifier const *) const = 0;\n", out);
	fputs("virtual unsigned __rl_real_sizeof(__rl_identifier const *) const = 0;", out);

	fputs("typedef ", out);
	rlc_printer_print_ctx_symbol(printer, file, out);
	fputs(" __rl_MY_T;\n", out);

	fputs("virtual void __rl_copy_rtti(__rl_identifier const *, void *) const = 0;\n", out);
	fputs("virtual void __rl_virtual_constructor(__rl_identifier const *, __rl_MY_T const&) = 0;\n", out);
	fputs("virtual void __rl_virtual_constructor(__rl_identifier const *, __rl_MY_T &&) = 0;\n", out);

	fputs("static inline ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(" &__rl_factory(", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(" &ref) { return ref; }\n", out);

	fputs("static inline ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(" &__rl_factory(", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(" &&ref) { return *&ref; }\n", out);

	fputs("template<class __rl_mask_type> static inline ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("_wrapper<__rl_mask_type",out);
	for(RlcSrcSize i = 0; i < this->fTemplates.fChildCount; i++)
	{
		fputs(", ", out);
		rlc_src_string_print(
			&this->fTemplates.fChildren[i].fName,
			file,
			out);
	}
	fputs("> __rl_factory(__rl_mask_type &&v);\n", out);

	rlc_printer_print_ctx_tpl(printer, file, printer->fFuncsImpl);
	fputs("template<class __rl_mask_type> ", printer->fFuncsImpl);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		printer->fFuncsImpl);
	fputs("_wrapper<__rl_mask_type", printer->fFuncsImpl);
	for(RlcSrcSize i = 0; i < this->fTemplates.fChildCount; i++)
	{
		fputs(", ", printer->fFuncsImpl);
		rlc_src_string_print(
			&this->fTemplates.fChildren[i].fName,
			file,
			printer->fFuncsImpl);
	}
	fputs("> ", printer->fFuncsImpl);
	rlc_printer_print_ctx_symbol(printer, file, printer->fFuncsImpl);
	fputs("::__rl_factory(__rl_mask_type &&v) { return (__rl_mask_type &&)(v); }\n", printer->fFuncsImpl);


	for(RlcSrcSize i = 0; i < this->fFunctionCount; i++)
	{
		if(RLC_BASE_CAST(&this->fFunctions[i], RlcParsedFunction)->fHasBody) {
			rlc_parsed_member_function_print(&this->fFunctions[i], file, printer);
		} else {
			struct RlcParsedMemberFunction fn = this->fFunctions[i];
			fn.fAbstractness = kRlcMemberFunctionAbstractnessAbstract;
			rlc_parsed_member_function_print(&fn, file, printer);
		}
	}

	rlc_printer_pop_ctx(printer);
	fputs("};\n", out);

	fputs("template<class __rl_mask_type", out);
	for(RlcSrcSize i = 0; i < this->fTemplates.fChildCount; i++)
	{
		fputs(", ", out);
		rlc_parsed_template_decl_child_print(
			&this->fTemplates.fChildren[i], file, out);
	}
	fputs("> class ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("_wrapper : public ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	if(this->fTemplates.fChildCount)
	{
		fputs("<", out);
		for(RlcSrcSize i = 0; i < this->fTemplates.fChildCount; i++)
		{
			if(i)
				fputs(", ", out);
			rlc_src_string_print(
				&this->fTemplates.fChildren[i].fName,
				file,
				out);
		}
		fputs(">", out);
	}

	fputs("{\n\tstd::decay_t<__rl_mask_type> __rl_mask_ptr;\n\tpublic:\n", out);
	{
		fputs("typedef ", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		if(this->fTemplates.fChildCount)
		{
			fputs("<", out);
			for(RlcSrcSize i = 0; i < this->fTemplates.fChildCount; i++)
			{
				if(i)
					fputs(", ", out);
				rlc_src_string_print(
					&this->fTemplates.fChildren[i].fName,
					file,
					out);
			}
			fputs(">", out);
		}
		fputs(" __rl_PARENT_T;\n", out);
	}

	{
		fputs("typedef ", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("_wrapper<__rl_mask_type", out);
		if(this->fTemplates.fChildCount)
		{
			for(RlcSrcSize i = 0; i < this->fTemplates.fChildCount; i++)
			{
				fputs(", ", out);
				rlc_src_string_print(
					&this->fTemplates.fChildren[i].fName,
					file,
					out);
			}
		}
		fputs(">", out);
		fputs(" __rl_MY_T;\n", out);
	}

	fputs("using typename __rl_PARENT_T::__rl_identifier;\n", out);

	fputs("inline void __rl_copy_rtti(__rl_identifier const *, void * dst) const final { new (dst) __rl_MY_T(); }\n", out);
	fputs("void __rl_virtual_constructor(__rl_identifier const *, __rl_PARENT_T const& rhs) { new (this) __rl_MY_T(static_cast<__rl_MY_T const&>(rhs)); }\n", out);
	fputs("void __rl_virtual_constructor(__rl_identifier const *, __rl_PARENT_T && rhs) { new (this) __rl_MY_T(static_cast<__rl_MY_T &&>(rhs)); }\n", out);

	fputs("void const * __rl_get_derived(__rl_identifier const *) const final { return this; }\n", out);
	fputs("constexpr unsigned __rl_real_sizeof(__rl_identifier const *) const { return sizeof(*this); }\n", out);

	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("_wrapper() = default;\n", out);

	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("_wrapper(__rl_mask_type p): __rl_mask_ptr(std::move(p)) {}\n", out);

	for(RlcSrcSize i = 0; i < this->fFunctionCount; i++)
	{
		if(!RLC_BASE_CAST(&this->fFunctions[i], RlcParsedFunction)->fHasBody) {
			struct RlcParsedMemberFunction const * memfn = &this->fFunctions[i];
			struct RlcParsedMember const * member = RLC_BASE_CAST(memfn, RlcParsedMember);
			struct RlcParsedFunction const * fn = RLC_BASE_CAST(memfn, RlcParsedFunction);

			rlc_visibility_print(member->fVisibility, 1, out);
			RLC_DASSERT(fn->fHasReturnType);
			rlc_parsed_template_decl_print(&fn->fTemplates, file, out);
			fputs("inline ", out);
			rlc_parsed_type_name_print(&fn->fReturnType, file, out);
			fputs(" ", out);
			rlc_src_string_print(
				&RLC_BASE_CAST(fn, RlcParsedScopeEntry)->fName,
				file,
				out);
			fputs("(", out);
			for(RlcSrcSize i = 0; i < fn->fArgumentCount; i++)
			{
				if(i)
					fputs(", ", out);
				rlc_parsed_variable_print_argument(&fn->fArguments[i], file, out, 0);
				fprintf(out, "__rlc_arg_%u", i);
			}
			fputs(") ", out);
			if(member->fAttribute == kRlcMemberAttributeIsolated)
				fputs("const ", out);
			fputs("final { ", out);

			for(int i = 0; i <= 1; i++)
			{
				if(!i)
					fputs("if constexpr(requires{{", out);
				else if(fn->fReturnType.fValue != kRlcParsedTypeNameValueVoid
				|| fn->fReturnType.fTypeModifierCount)
					fputs("return ", out);

				fputs("__rl::deref(__rl_mask_ptr).", out);
				rlc_src_string_print(
					&RLC_BASE_CAST(fn, RlcParsedScopeEntry)->fName,
					file,
					out);
				fputs("(", out);
				for(RlcSrcSize i = 0; i < fn->fArgumentCount; i++)
				{
					if(i) fputs(", ", out);
					fputs("::__cpp_std::forward<", out);
					RLC_DASSERT(fn->fArguments[i].fHasType);
					rlc_parsed_type_name_print(&fn->fArguments[i].fType, file, out);
					fputs(">(", out);
					rlc_src_string_print(
						&RLC_BASE_CAST(&fn->fArguments[i], RlcParsedScopeEntry)->fName,
						file,
						out);
					fprintf(out, "__rlc_arg_%u", i);
					fputs(")", out);
				}
				fputs(")", out);
				if(!i) fputs("};}) ", out);
				else fputs(";\n", out);
			}
			fputs("else throw ::__rl::Tuple<char const*, char const*>{::__rl::createTuple,\"unsupported MASK call: ", out);
			rlc_src_string_print(
					&RLC_BASE_CAST(fn, RlcParsedScopeEntry)->fName,
					file,
					out);
			fputs("()\", ::__rl::type_name<>(__rl::deref(__rl_mask_ptr))}; }", out);
		}
	}

	fputs("};\n", out);
}

void rlc_parsed_mask_print(
	struct RlcParsedMask const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer)
{
	rlc_parsed_mask_print_decl(this, file, printer->fTypes);
	rlc_parsed_mask_print_impl(this, file, printer);
}