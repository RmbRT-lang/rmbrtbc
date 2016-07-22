/** @file symbol.h
	Defines the symbol structure. */

#ifndef __rlc_parser_symbol_h_defined
#define __rlc_parser_symbol_h_defined

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A single identifier, possibly templated.
@relates RlcParsedSymbol */
struct RlcParsedSymbolChild
{
	/** The name token's index. */
	size_t fNameToken;
	/** The template arguments. */
	enum RlcParsedExpression ** fTemplates;
	/** The template arguments' count. */
	size_t fTemplateCount;
};

/** Adds a template argument to a symbol child.
@memberof RlcParsedSymbolChild

@param[in] this:
	The symbol child to add a template argument to.
	@pass_pointer_ownership
@param[in] template_argument:
	The template argument. */
void rlc_parsed_symbol_child_add_template(
	struct RlcParsedSymbolChild * this,
	enum RlcParsedExpression * template_argument);


/** A (possibly) namespace-qualified symbol. */
struct RlcParsedSymbol
{
	/** The symbol children. */
	struct RlcParsedSymbolChild * fChildren;
	/** The count of symbol children. */
	size_t fChildCount;
	/** Whether the symbol was prefixed with ```::``` to access the root namespace. */
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

@param[in] this:
	The symbol to add a child to.
@param[in] child:
	The symbol child to add. */
void rlc_parsed_symbol_add_child(
	struct RlcParsedSymbol * this,
	struct RlcParsedSymbolChild const * child);

#ifdef __cplusplus
}
#endif

#endif