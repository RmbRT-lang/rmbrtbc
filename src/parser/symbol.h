/** @file symbol.h
	Defines the symbol structure. */

#ifndef __rlc_parser_symbol_h_defined
#define __rlc_parser_symbol_h_defined

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A single identifier, possibly templated.
@relates RlcSymbol */
struct RlcSymbolChild
{
	/** The name token's index. */
	size_t fNameToken;
	/** The template arguments. */
	enum RlcExpression ** fTemplates;
	/** The template arguments' count. */
	size_t fTemplateCount;
};

/** Adds a template argument to a symbol child.
@memberof RlcSymbolChild

@param[in] this:
	The symbol child to add a template argument to.
	@pass_pointer_ownership
@param[in] template_argument:
	The template argument. */
void rlc_symbol_child_add_template(
	struct RlcSymbolChild * this,
	enum RlcExpression * template_argument);


/** A (possibly) namespace-qualified symbol. */
struct RlcSymbol
{
	/** The symbol children. */
	struct RlcSymbolChild * fChildren;
	/** The count of symbol children. */
	size_t fChildCount;
	/** Whether the symbol was prefixed with ```::``` to access the root namespace. */
	int fIsRoot;
};
/** Destroys a symbol.
	Releases memory allocated by the given symbol, but not the symbol itself.
@memberof RlcSymbol

@param[in] this:
	The symbol to destroy. */
void rlc_symbol_destroy(
	struct RlcSymbol * this);

/** Adds a child to a symbol.
@memberof RlcSymbol

@param[in] this:
	The symbol to add a child to.
@param[in] child:
	The symbol child to add. */
void rlc_symbol_add_child(
	struct RlcSymbol * this,
	struct RlcSymbolChild const * child);

#ifdef __cplusplus
}
#endif

#endif