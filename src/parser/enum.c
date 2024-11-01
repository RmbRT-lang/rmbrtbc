#include "enum.h"
#include "symbolconstantexpression.h"

#include "../tokeniser/tokens.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_enum_constant_create(
	struct RlcParsedEnumConstant * this,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedEnumConstant,
		name);

	this->fAliasTokens = NULL;
	this->fAliasCount = 0;
}

void rlc_parsed_enum_constant_add_name(
	struct RlcParsedEnumConstant * this,
	struct RlcSrcString const * nameToken)
{
	RLC_DASSERT(this != NULL);

	rlc_realloc(
		(void**)&this->fAliasTokens,
		sizeof(*this->fAliasTokens) * ++this->fAliasCount);

	this->fAliasTokens[this->fAliasCount-1] = *nameToken;
}

void rlc_parsed_enum_constant_destroy(
	struct RlcParsedEnumConstant * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	if(this->fAliasTokens)
	{
		rlc_free((void**)&this->fAliasTokens);
		this->fAliasCount = 0;
	}
}

int rlc_parsed_enum_constant_parse(
	struct RlcParsedEnumConstant * out,
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	struct RlcToken name;
	if(!rlc_parser_consume(
		parser,
		&name,
		kRlcTokIdentifier))
		return 0;

	rlc_parsed_enum_constant_create(out, &name.content);
	rlc_parsed_symbol_constant_register(rlc_parser_file(parser), &name.content);

	while(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokColonEqual))
	{
		rlc_parser_expect(
			parser,
			&name,
			1,
			kRlcTokIdentifier);

		rlc_parsed_enum_constant_add_name(
			out,
			&name.content);
		rlc_parsed_symbol_constant_register(rlc_parser_file(parser), &name.content);
	}

	return 1;
}

void rlc_parsed_enum_create(
	struct RlcParsedEnum * this,
	struct RlcSrcString const * name)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedEnum,
		name);

	this->fConstants = NULL;
	this->fConstantCount = 0;
	this->fDefaultConstant = -1;
}

void rlc_parsed_enum_destroy(
	struct RlcParsedEnum * this)
{
	RLC_DASSERT(this != NULL);

	for(size_t i = 0; i < this->fConstantCount; i++)
		rlc_parsed_enum_constant_destroy(
			&this->fConstants[i]);

	if(this->fConstants)
	{
		rlc_free((void**)&this->fConstants);
		this->fConstantCount = 0;
	}
}

void rlc_parsed_enum_add_constant(
	struct RlcParsedEnum * this,
	struct RlcParsedEnumConstant const * constant)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(constant != NULL);

	rlc_realloc(
		(void**)&this->fConstants,
		sizeof(struct RlcParsedEnumConstant) * ++this->fConstantCount);
	this->fConstants[this->fConstantCount-1] = *constant;
}

int rlc_parsed_enum_parse(
	struct RlcParsedEnum * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(out != NULL);

	if(!rlc_parser_consume(
		parser,
		NULL,
		kRlcTokEnum))
		return 0;

	if(rlc_parsed_template_decl_exists(templates))
		rlc_parser_fail(parser, "enums must not have template declarations");

	struct RlcToken name;
	rlc_parser_expect(
		parser,
		&name,
		1,
		kRlcTokIdentifier);

	rlc_parsed_enum_create(
		out,
		&name.content);

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceOpen);

	do {
		if(out->fDefaultConstant == -1
		&& (rlc_parser_consume(parser, NULL, kRlcTokDefault)
			|| rlc_parser_consume(parser, NULL, kRlcTokAsterisk)))
		{
			out->fDefaultConstant = out->fConstantCount;
		}

		struct RlcParsedEnumConstant constant;
		if(!rlc_parsed_enum_constant_parse(
			&constant,
			parser))
			rlc_parser_fail(parser, "expected enum constant");

		rlc_parsed_enum_add_constant(
			out,
			&constant);

	} while(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokComma));

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokBraceClose);

	return 1;
}

static void rlc_parsed_enum_print_symbol_ctor(
	struct RlcParsedEnum const * this,
	struct RlcSrcString const * name,
	struct RlcSrcFile const * file,
	RlcSrcIndex value,
	FILE * out)
{
	fputs("constexpr ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputc('(', out);
	fputs("::__rl::constant::_t_", out);
	rlc_src_string_print_noreplace(name, file, out);
	fputs("): ", out);
	fputs("::__rl::EnumWrapper<", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(", __rl_enum_", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fprintf(out, ", %zu>(%zu) {}\n",
		(size_t)this->fConstantCount,
		(size_t)value);
}

static void rlc_parsed_enum_print_to_file(
	struct RlcParsedEnum const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter const * printer,
	FILE * out)
{
	fputs("enum class __rl_enum_", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(": ", out);
	fputs(
		this->fConstantCount <= 0xff ? "::uint8_t" :
		this->fConstantCount <= 0xffff ? "::uint16_t" :
		"uint32_t",
		out);
	fputs(" {\n", out);

	for(RlcSrcIndex i = 0; i < this->fConstantCount; i++)
	{
		if(i)
			fprintf(out, ",\n");

		rlc_src_string_print(
			&RLC_BASE_CAST(&this->fConstants[i], RlcParsedScopeEntry)->fName,
			file,
			out);

		for(RlcSrcIndex j = 0; j < this->fConstants[i].fAliasCount; j++)
		{
			fprintf(out, ", ");
			rlc_src_string_print(
				&this->fConstants[i].fAliasTokens[j],
				file,
				out);
			fprintf(out, " = ");
			rlc_src_string_print(
				&RLC_BASE_CAST(&this->fConstants[i], RlcParsedScopeEntry)->fName,
				file,
				out);
		}
	}

	fputs("\n};\n", out);

	fputs("struct ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(": public ::__rl::EnumWrapper<", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(", __rl_enum_", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fprintf(out, ", %zu>\n", (size_t)this->fConstantCount);
	fputs("{\n\t"
		"using ::__rl::EnumWrapper<", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(", __rl_enum_", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fprintf(out, ", %zu>::EnumWrapper;\n\n"
		"constexpr operator char const *() const\n"
		"{\n\t"
		"char const * const names[] = {\n",
		(size_t)this->fConstantCount);

	for(RlcSrcIndex i = 0; i < this->fConstantCount; i++)
	{
		if(i)
			fprintf(out, ",\n");

		fputs("\t\"", out);
		rlc_src_string_print_noreplace(
			&RLC_BASE_CAST(&this->fConstants[i], RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("\"", out);
	}
	fputs("\n};\n"
		"if(this->valid())\n\t"
			"return names[(int)this->value];\n"
		"else\n\t"
			"return \"<unknown ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs(">\";\n"
		"}\n\n", out);

	if(this->fDefaultConstant == -1)
		fputs("private: ", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fprintf(out, "(::__rl::default_init_t): EnumWrapper(%d) {}\npublic:\n",
		this->fDefaultConstant);

	fputs("static constexpr char const * __rl_type_name_v = \"", out);
	rlc_printer_print_ctx_symbol_with_namespace_rl_flavour(printer, file, out);
		fputs("::", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("\";\n", out);

	for(RlcSrcIndex i = 0; i < this->fConstantCount; i++)
	{
		rlc_parsed_enum_print_symbol_ctor(
			this,
			&RLC_BASE_CAST(&this->fConstants[i], RlcParsedScopeEntry)->fName,
			file,
			i,
			out);

		fputs("\t"
			"static constexpr __rl::EnumConstant<", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs(" *, __rl_enum_", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("> const ", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(&this->fConstants[i], RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("{__rl_enum_", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("::", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(&this->fConstants[i], RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs("};\n", out);
		for(RlcSrcIndex j = 0; j < this->fConstants[i].fAliasCount; j++)
		{
			rlc_parsed_enum_print_symbol_ctor(
				this,
				&this->fConstants[i].fAliasTokens[j],
				file,
				i,
				out);

			fputs("\t"
				"static constexpr __rl::EnumConstant<", out);
			rlc_src_string_print(
				&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
				file,
				out);
			fputs(" *, __rl_enum_", out);
			rlc_src_string_print(
				&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
				file,
				out);
			fputs("> const ", out);
			rlc_src_string_print(
				&this->fConstants[i].fAliasTokens[j],
				file,
				out);
			fputs("{__rl_enum_", out);
			rlc_src_string_print(
				&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
				file,
				out);
			fputs("::", out);
			rlc_src_string_print(
				&RLC_BASE_CAST(&this->fConstants[i], RlcParsedScopeEntry)->fName,
				file,
				out);
			fputs("};\n", out);
		}
	}


	// unbox
	fputs("\t"
		"template<class Fn, class ...Args>\n\t"
		"void __rl_unbox(Fn &&fn, Args&&... args) const { switch(value) {\n", out);
	for(RlcSrcIndex i = 0; i < this->fConstantCount; i++)
	{
		fputs("\tcase ", out);
		rlc_src_string_print(
			&RLC_BASE_CAST(&this->fConstants[i], RlcParsedScopeEntry)->fName,
			file,
			out);
		fputs(": fn(::__rl::constant::_t_", out);
		rlc_src_string_print_noreplace(
			&RLC_BASE_CAST(&this->fConstants[i], RlcParsedScopeEntry)->fName,
			file, out);
		fputs("{}, ::std::forward<Args>(args)...); return;\n", out);
	}
	fputs("\tdefault: throw \"", out);
	rlc_src_string_print(
		&RLC_BASE_CAST(this, RlcParsedScopeEntry)->fName,
		file,
		out);
	fputs("::__rl_unbox: unknown value\";\n\t}}", out);


	// end enum class
	fputs("};\n", out);
}

void rlc_parsed_enum_print(
	struct RlcParsedEnum const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer)
{
	rlc_parsed_enum_print_to_file(this, file, printer, printer->fTypes);
}

void rlc_parsed_member_enum_create(
	struct RlcParsedMemberEnum * this,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(member != NULL);

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberEnum,
		member);
}

void rlc_parsed_member_enum_destroy(
	struct RlcParsedMemberEnum * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_enum_destroy(
		RLC_BASE_CAST(this, RlcParsedEnum));
	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
}

int rlc_parsed_member_enum_parse(
	struct RlcParsedMemberEnum * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);
	RLC_DASSERT(member != NULL);

	if(!rlc_parsed_enum_parse(
		RLC_BASE_CAST(out, RlcParsedEnum),
		parser,
		&member->templates))
		return 0;

	rlc_parsed_member_enum_create(
		out,
		member);

	return 1;
}

void rlc_parsed_member_enum_print(
	struct RlcParsedMemberEnum const * this,
	struct RlcSrcFile const * file,
	struct RlcPrinter * printer)
{
	rlc_visibility_print(
		RLC_BASE_CAST(this, RlcParsedMember)->fVisibility,
		1,
		printer->fTypesImpl);
	rlc_parsed_enum_print_to_file(
		RLC_BASE_CAST(this, RlcParsedEnum),
		file,
		printer,
		printer->fTypesImpl);
}