#include "variable.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_variable_create(
	struct RlcParsedVariable * this,
	struct RlcSrcString const * name,
	struct RlcParsedTemplateDecl const * templates)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(name != NULL);
	RLC_DASSERT(templates != NULL);

	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedVariable,
		name);

	this->fTemplates = *templates;

	this->fHasType = 0;
	this->fInitArgs = NULL;
	this->fInitArgCount = 0;
	this->fReferenceType = kRlcReferenceTypeNone;
}

void rlc_parsed_variable_destroy(
	struct RlcParsedVariable * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_parsed_template_decl_destroy(&this->fTemplates);
	if(this->fHasType)
		rlc_parsed_type_name_destroy(&this->fType);

	if(this->fInitArgs)
	{
		for(size_t i = this->fInitArgCount; i--;)
		{
			rlc_parsed_expression_destroy_virtual(this->fInitArgs[i]);
			rlc_free((void**)&this->fInitArgs[i]);
		}
		rlc_free((void**)&this->fInitArgs);
	}
}

static void rlc_parsed_variable_add_arg(
	struct RlcParsedVariable * out,
	struct RlcParsedExpression * arg)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(arg != NULL);

	rlc_realloc(
		(void**)&out->fInitArgs,
		sizeof(struct RlcParsedExpression *) * ++out->fInitArgCount);

	out->fInitArgs[out->fInitArgCount - 1] = arg;
}


int rlc_parsed_variable_parse(
	struct RlcParsedVariable * out,
	struct RlcParser * parser,
	struct RlcParsedTemplateDecl const * templates,
	int needs_name,
	int allow_initialiser,
	int force_initialiser,
	int allow_templates,
	int allow_reference)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	int needs_type = 1;
	int has_name = 0;

	struct RlcToken name;
	if(rlc_parser_is_current(
		parser,
		kRlcTokIdentifier))
	{
		// "name: type" style variable?
		if(rlc_parser_is_ahead(
			parser,
			kRlcTokColon))
		{
			has_name = 1;
			rlc_parser_expect(
				parser,
				&name,
				1,
				kRlcTokIdentifier);
			rlc_parser_skip(parser);
		} else if(allow_initialiser)
		{
			rlc_parser_expect(
				parser,
				&name,
				1,
				kRlcTokIdentifier);
			enum RlcTypeQualifier qualifier;
			int has_qualifier = rlc_type_qualifier_parse(
				&qualifier,
				parser);

			// "name ::=" style variable?
			if(rlc_parser_consume(
				parser,
				NULL,
				kRlcTokDoubleColonEqual))
			{
				has_name = 1;
				needs_type = 0;
				out->fHasType = 0;
				out->fTypeQualifier = qualifier;
			} else if(has_qualifier)
			{
				rlc_parser_fail(parser, "expected '::='");
			}
		}
	} // If !isArgument, "name: type" is expected.
	if(!has_name
	&& needs_name)
	{
		return 0;
	}

	if(!has_name)
		name.content = kRlcSrcStringEmpty;

	rlc_parsed_variable_create(
		out,
		&name.content,
		templates);

	if(!needs_type)
	{
		out->fReferenceType = kRlcReferenceTypeNone;
		struct RlcParsedExpression * init = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType));
		if(!init)
			rlc_parser_fail(parser, "expected expression");
		rlc_parsed_variable_add_arg(out, init);
	} else
	{
		if(!rlc_parsed_type_name_parse(
			&out->fType,
			parser))
		{
			if(needs_name)
				rlc_parser_fail(parser, "expected type name");
			else
				return 0;
		}

		out->fHasType = 1;

		out->fReferenceType = kRlcReferenceTypeNone;
		if(allow_reference)
		{
			if(rlc_parser_consume(
				parser,
				NULL,
				kRlcTokAnd))
			{
				out->fReferenceType = kRlcReferenceTypeReference;
			} else if(rlc_parser_consume(
				parser,
				NULL,
				kRlcTokDoubleAnd))
			{
				out->fReferenceType = kRlcReferenceTypeTempReference;
			}
		}

		if(allow_initialiser)
		{
			int isParenthese = 0;
			if(rlc_parser_consume(
				parser,
				NULL,
				kRlcTokColonEqual)
			|| (isParenthese = rlc_parser_consume(
				parser,
				NULL,
				kRlcTokParentheseOpen)))
			{
				// check for empty initialiser.
				if(!isParenthese
				|| !rlc_parser_data_consume(
					parser,
					kRlcTokParentheseClose))
				{
					do {
						struct RlcParsedExpression * arg = rlc_parsed_expression_parse(
							parser,
							RLC_ALL_FLAGS(RlcParsedExpressionType));

						if(!arg)
							rlc_parser_fail(parser, "expected expression");

						rlc_parsed_variable_add_arg(out, arg);
					} while(isParenthese && rlc_parser_consume(
						parser,
						NULL,
						kRlcTokComma));

					if(isParenthese)
						rlc_parser_expect(
							parser,
							NULL,
							1,
							kRlcTokParentheseClose);
				}
			} else if(force_initialiser)
			{
				rlc_parser_fail(parser, "expected ':=' or '('");
			}
		}
	}

	if(!allow_templates
	&& rlc_parsed_template_decl_exists(&out->fTemplates))
		rlc_parser_fail(parser, "variable had forbidden template declaration");

	return 1;
}

void rlc_parsed_member_variable_create(
	struct RlcParsedMemberVariable * this,
	enum RlcVisibility visibility,
	enum RlcMemberAttribute attribute,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(RLC_IN_ENUM(attribute, RlcMemberAttribute));

	rlc_parsed_member_create(
		RLC_BASE_CAST(this, RlcParsedMember),
		kRlcParsedMemberVariable,
		visibility,
		start_index);

	this->fAttribute = attribute;
}

void rlc_parsed_member_variable_destroy(
	struct RlcParsedMemberVariable * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_variable_destroy(
		RLC_BASE_CAST(this, RlcParsedVariable));

	rlc_parsed_member_destroy_base(
		RLC_BASE_CAST(this, RlcParsedMember));
}

int rlc_parsed_member_variable_parse(
	struct RlcParsedMemberVariable * out,
	enum RlcVisibility * default_visibility,
	struct RlcParserData * parser)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	size_t const start_index = parser->fIndex;
	enum RlcParseError error_code;

	enum RlcVisibility visibility = rlc_visibility_parse(
		default_visibility,
		parser);


	rlc_parsed_member_variable_create(
		out,
		visibility,
		rlc_member_attribute_parse(
			parser),
		start_index);


	int const is_static = out->fAttribute == kRlcMemberAttributeStatic;

	if(!rlc_parsed_variable_parse(
		RLC_BASE_CAST(out, RlcParsedVariable),
		parser,
		is_static, // needs name
		is_static, // allow initialiser
		0, // if static, force name and allow initialiser.
		is_static, // if static, allow template declarations.
		0)) // forbid references.
	{
		// something was parsed already?
		if(parser->fErrorCount)
		{
			rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedVariable);
			goto nonfatal_failure;
		} else goto nonfatal_failure;
	} else if(!rlc_parser_data_consume(
		parser,
		kRlcTokSemicolon))
	{
		error_code = kRlcParseErrorExpectedSemicolon;
		goto failure;
	}

success:
	RLC_BASE_CAST(out, RlcParsedMember)->fLocation.fEnd = parser->fIndex;
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	rlc_parsed_member_variable_destroy(out);
nonfatal_failure:
	parser->fIndex = start_index;
	return 0;
}