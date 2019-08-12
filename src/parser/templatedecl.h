/** @file templatedecl.h
	Defines template argument declaration types used by the parser. */
#ifndef __rlc_parser_templatedecl_h_defined
#define __rlc_parser_templatedecl_h_defined

#include "parser.h"
#include "typename.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** The type of a template argument. */
enum RlcParsedTemplateDeclType
{
	/** Argument is a type. */
	kRlcParsedTemplateDeclTypeType,
	/** Argument is a number. */
	kRlcParsedTemplateDeclTypeNumber,
	/** Argument is a value. */
	kRlcParsedTemplateDeclTypeValue,

	RLC_ENUM_END(RlcParsedTemplateDeclType)
};

/** Single template argument declaration. */
struct RlcParsedTemplateDeclChild
{
	/** The template argument's type. */
	enum RlcParsedTemplateDeclType fType;
	/** If the template argument is a value, then this is the value's type. */
	struct RlcParsedTypeName fValueType;
	/** The name of the argument. */
	size_t fNameToken;
};

/** Multiple template arguemnt declaration. */
struct RlcParsedTemplateDecl
{
	/** The arguments. */
	struct RlcParsedTemplateDeclChild * fChildren;
	/** The argument count. */
	size_t fChildCount;
};

/** Initialises a template argument declaraion to be empty.
@memberof RlcParsedTemplateDecl

@param[in] this:
	The template argument declaration to initialise. */
void rlc_parsed_template_decl_create(
	struct RlcParsedTemplateDecl * this);

/** Adds an argument to a template argument declaration.
@memberof RlcParsedTemplateDecl

@param[in] this:
	The template argument declaration to add an argument to.
@param[in] child:
	The template argument to add. */
void rlc_parsed_template_decl_add_child(
	struct RlcParsedTemplateDecl * this,
	struct RlcParsedTemplateDeclChild const * child);

/** Destroys a template argument declaration.
@memberof RlcParsedTemplateDecl

@param[in] this:
	The template argument declaration to destroy. */
void rlc_parsed_template_decl_destroy(
	struct RlcParsedTemplateDecl * this);

/** Parses a template declaration. */
int rlc_parsed_template_decl_parse(
	struct RlcParsedTemplateDecl * decl,
	struct RlcParserData * parser);

/** Checks whether a template declaration has entries.
@memberof RlcParsedTemplateDecl
@param[in] this:
	The template declaration to check.
@return
	Whether the template declaration is empty. */
static inline int rlc_parsed_template_decl_exists(
	struct RlcParsedTemplateDecl const * this);

/** Destroys a template argument declaration child.
@memberof RlcParsedTemplateDeclChild

@param[in] this:
	The template argument declaration child to destroy. */
void rlc_parsed_template_decl_child_destroy(
	struct RlcParsedTemplateDeclChild * this);

#include "templatedecl.inl"


#ifdef __cplusplus
}
#endif

#endif