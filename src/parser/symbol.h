/** @file symbol.h
	Defines the symbol structure. */

#ifndef __rlc_parser_symbol_h_defined
#define __rlc_parser_symbol_h_defined

#include <stddef.h>

#include "parser.h"
#include "expression.h"


#ifdef __cplusplus
extern "C" {
#endif

enum RlcParsedSymbolChildType
{
	kRlcParsedSymbolChildTypeIdentifier,
	kRlcParsedSymbolChildTypeConstructor,
	kRlcParsedSymbolChildTypeDestructor
};

struct RlcParsedSymbolChildTemplate
{
	int fIsExpression;
	union
	{
		struct RlcParsedExpression * fExpression;
		struct RlcParsedTypeName * fTypeName;
	};
};

/** A single identifier, possibly templated.
@relates RlcParsedSymbol */
struct RlcParsedSymbolChild
{
	/** The name token type. */
	enum RlcParsedSymbolChildType fType;
	/** The name token's index. */
	struct RlcSrcString fName;
	/** The template arguments. */
	struct RlcParsedSymbolChildTemplate * fTemplates;
	/** The template arguments' count. */
	size_t fTemplateCount;
};

/** Creates a symbol child.
@param[out] this:
	The symbol child to create.
	@dassert @nonnull */
void rlc_parsed_symbol_child_create(
	struct RlcParsedSymbolChild * this);
/** Destroys a symbol child.
@memberof RlcParsedSymbolChild
@param[in,out] this:
	The symbol child to destroy.
	@dassert @nonnull */
void rlc_parsed_symbol_child_destroy(
	struct RlcParsedSymbolChild * this);

/** Adds a template argument to a symbol child.
@memberof RlcParsedSymbolChild

@param[in] this:
	The symbol child to add a template argument to.
	@pass_pointer_ownership
	@dassert @nonnull
@param[in] template_argument:
	The template argument.
	@dassert @nonnull */
void rlc_parsed_symbol_child_add_template(
	struct RlcParsedSymbolChild * this,
	struct RlcParsedSymbolChildTemplate * template_argument);

int rlc_parsed_symbol_child_parse(
	struct RlcParsedSymbolChild * out,
	struct RlcParser * parser,
	int allowSpecialIdentifiers);


/** A (possibly) namespace-qualified symbol. */
struct RlcParsedSymbol
{
	/** The symbol children. */
	struct RlcParsedSymbolChild * fChildren;
	/** The count of symbol children. */
	size_t fChildCount;
	/** Whether the symbol was prefixed with `::` to access the root namespace. */
	int fIsRoot;
};
/** Destroys a symbol.
	Releases memory allocated by the given symbol, but not the symbol itself.
@memberof RlcParsedSymbol

@param[in] this:
	The symbol to destroy. */
void rlc_parsed_symbol_destroy(
	struct RlcParsedSymbol * this);

/** Adds a child to a symbol.
@memberof RlcParsedSymbol

@param[in,out] this:
	The symbol to add a child to.
	@dassert @nonnull
@param[in,out] child:
	The symbol child to add.
	@pass_ownership
	@dassert @nonnull */
void rlc_parsed_symbol_add_child(
	struct RlcParsedSymbol * this,
	struct RlcParsedSymbolChild * child);

/** Creates a symbol.
@param[out] this:
	The symbol to create.
	@dassert @nonnull. */
void rlc_parsed_symbol_create(
	struct RlcParsedSymbol * this);

/** Parses a symbol.
@memberof RlcParserData
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[out] out:
	The symbol to parse.
	@dassert @nonnull */
int rlc_parsed_symbol_parse(
	struct RlcParsedSymbol * out,
	struct RlcParser * parser,
	int allowSpecialIdentifiers);

#ifdef __cplusplus
}
#endif

#endif