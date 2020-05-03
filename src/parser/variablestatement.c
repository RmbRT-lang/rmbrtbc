#include "variablestatement.h"


#include "../assert.h"

void rlc_parsed_variable_statement_create(
	struct RlcParsedVariableStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_statement_create(
		RLC_BASE_CAST(this, RlcParsedStatement),
		kRlcParsedVariableStatement);
}

void rlc_parsed_variable_statement_destroy(
	struct RlcParsedVariableStatement * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_variable_destroy(&this->fVariable);
	rlc_parsed_statement_destroy_base(
		RLC_BASE_CAST(this, RlcParsedStatement));
}

int rlc_parsed_variable_statement_parse(
	struct RlcParsedVariableStatement * out,
	struct RlcParser * parser)
{
	int isStatic = rlc_parser_consume(
		parser,
		NULL,
		kRlcTokStatic);

	if(!rlc_parsed_variable_parse(
		&out->fVariable,
		parser,
		NULL,
		1,
		1,
		0,
		0,
		1))
	{
		return 0;
	}
	rlc_parsed_variable_statement_create(out);
	out->fIsStatic = isStatic;

	rlc_parser_expect(
		parser,
		NULL,
		1,
		kRlcTokSemicolon);

	return 1;
}

void rlc_parsed_variable_statement_print(
	struct RlcParsedVariableStatement const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	if(this->fIsStatic)
		fputs("static ", out);
	rlc_parsed_variable_print_argument(&this->fVariable, file, out, 1);
	fputs(";\n", out);
}