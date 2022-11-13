#ifndef __rlc_parser_symbolconstantexpression_h_defined
#define __rlc_parser_symbolconstantexpression_h_defined

#include "expression.h"
#include "typename.h"
#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

void rlc_parsed_symbol_constant_register(
	struct RlcSrcFile const * file,
	struct RlcSrcString const * name);
void rlc_parsed_symbol_constant_free();
void rlc_parsed_symbol_constant_print(FILE * out);


struct RlcParsedSymbolConstantExpression
{
	RLC_DERIVE(struct,RlcParsedExpression);

	struct RlcToken fName;

	// optionally injected template type (:symbol.T).
	int fHasType;
	struct RlcParsedTypeName fType;
};

void rlc_parsed_symbol_constant_expression_create(
	struct RlcParsedSymbolConstantExpression * this,
	struct RlcToken const * op,
	struct RlcToken const * name,
	struct RlcParsedTypeName const * type);

_Nodiscard int rlc_parsed_symbol_constant_expression_parse(
	struct RlcParsedSymbolConstantExpression * out,
	struct RlcParser * parser);

void rlc_parsed_symbol_constant_expression_destroy(
	struct RlcParsedSymbolConstantExpression * this);

void rlc_parsed_symbol_constant_expression_print(
	struct RlcParsedSymbolConstantExpression * this,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif