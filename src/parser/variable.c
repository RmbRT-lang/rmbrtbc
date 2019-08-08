#include "variable.h"

#include "../malloc.h"
#include "../assert.h"

void rlc_parsed_variable_create(
	struct RlcParsedVariable * this,
	size_t start_index)
{
	RLC_DASSERT(this != NULL);


	rlc_parsed_scope_entry_create(
		RLC_BASE_CAST(this, RlcParsedScopeEntry),
		kRlcParsedVariable,
		start_index);

	rlc_template_decl_create(
		&this->fTemplates);

	rlc_parsed_type_name_create(
		&this->fType);

	this->fInitArgs = NULL;
	this->fInitArgCount = 0;
}

void rlc_parsed_variable_destroy(
	struct RlcParsedVariable * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_scope_entry_destroy_base(
		RLC_BASE_CAST(this, RlcParsedScopeEntry));

	rlc_template_decl_destroy(&this->fTemplates);
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
	struct RlcParserData * parser,
	int needs_name,
	int allow_initialiser,
	int force_initialiser,
	int allow_templates)
{
	RLC_DASSERT(out != NULL);
	RLC_DASSERT(parser != NULL);

	enum RlcParseError error_code;
	size_t const start_index = parser->fIndex;

	rlc_parsed_variable_create(
		out,
		start_index);

	if(!rlc_template_decl_parse(
		&out->fTemplates,
		parser))
	{
		error_code = kRlcParseErrorExpectedTemplateDeclaration;
		goto failure;
	}

	int has_name = 0;
	int is_shortcut = 0;

	// "name: type" style variable?
	if(rlc_parser_data_match(
		parser,
		kRlcTokIdentifier)
	&& (rlc_parser_data_match_ahead(
		parser,
		kRlcTokColon)
	|| (is_shortcut = 0 != rlc_parser_data_match_ahead(
		parser,
		kRlcTokDoubleColonEqual))))
	{
		has_name = 1;

		rlc_parsed_scope_entry_add_name(
			RLC_BASE_CAST(out, RlcParsedScopeEntry),
			rlc_parser_data_matched_index(parser));

		rlc_parser_data_next(parser);
		if(!allow_initialiser && is_shortcut)
		{
			error_code = kRlcParseErrorForbiddenDoubleColonEqual;
			goto failure;
		}
		rlc_parser_data_next(parser);
	} // If !isArgument, "name: type" is expected.
	else if(needs_name)
	{
		goto nonfatal_failure;
	}

	out->fHasType = !is_shortcut;

	if(is_shortcut)
	{
		struct RlcParsedExpression * init = rlc_parsed_expression_parse(
			parser,
			RLC_ALL_FLAGS(RlcParsedExpressionType));
		if(!init)
		{
			error_code = kRlcParseErrorExpectedExpression;
			goto failure;
		}
		rlc_parsed_variable_add_arg(out, init);
	} else
	{
		if(!rlc_parsed_type_name_parse(
			&out->fType,
			parser))
		{
			if(parser->fErrorCount
			|| has_name)
			{
				error_code = kRlcParseErrorExpectedTypeName;
				goto failure;
			} else {
				goto nonfatal_failure;
			}
		}
		if(allow_initialiser)
		{
			int isParenthese = 0;
			if(rlc_parser_data_consume(
				parser,
				kRlcTokColonEqual)
			|| (isParenthese = rlc_parser_data_consume(
				parser,
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
						{
							error_code = kRlcParseErrorExpectedExpression;
							goto failure;
						}

						rlc_parsed_variable_add_arg(out, arg);

					} while(isParenthese && rlc_parser_data_consume(
						parser,
						kRlcTokComma));

					if(isParenthese && !rlc_parser_data_consume(
						parser,
						kRlcTokParentheseClose))
					{
						error_code = kRlcParseErrorExpectedParentheseClose;
						goto failure;
					}
				}
			} else if(force_initialiser)
			{
				error_code = kRlcParseErrorExpectedInitialiser;
				goto failure;
			}
		}
	}

	if(!allow_templates
	&& rlc_template_decl_exists(&out->fTemplates))
	{
		parser->fIndex = start_index;
		error_code = kRlcParseErrorForbiddenTemplateDeclaration;
		goto failure;
	}

success:
	RLC_BASE_CAST(out, RlcParsedScopeEntry)->fLocation.fEnd = 0;
	return 1;
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
nonfatal_failure:
	rlc_parsed_variable_destroy(out);
	parser->fIndex = start_index;
	return 0;
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
		is_static)) // if static, allow template declarations.
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