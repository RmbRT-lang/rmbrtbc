/** @file templatedecl.h
	Defines template argument declaration types used by the parser. */
#ifndef __rlc_parser_templatedecl_h_defined
#define __rlc_parser_templatedecl_h_defined

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** The type of a template argument. */
enum RlcTemplateDeclType
{
	/** Argument is a type. */
	Type,
	/** Argument is a number. */
	Number
};

/** Single template argument declaration. */
struct RlcTemplateDeclChild
{
	/** Argument type. */
	enum RlcTemplateDeclType fType;
	/** The name of the argument. */
	size_t fNameToken;
};

/** Multiple template arguemnt declaration. */
struct RlcTemplateDecl
{
	/** The arguments. */
	struct RlcTemplateDeclChild * fChildren;
	/** The argument count. */
	size_t fChildCount;
};

/** Initialises a template argument declaraion to be empty.
@memberof RlcTemplateDecl

@param[in] this:
	The template argument declaration to initialise. */
void rlc_template_decl_create(
	struct RlcTemplateDecl * this);

/** Adds an argument to a template argument declaration.
@memberof RlcTemplateDecl

@param[in] this:
	The template argument declaration to add an argument to.
@param[in] child:
	The template argument to add. */
void rlc_template_decl_add_child(
	struct RlcTemplateDecl * this,
	struct RlcTemplateDeclChild const * child);

/** Destroys a template argument declaration.
@memberof RlcTemplateDecl

@param[in] this:
	The template argument declaration to destroy. */
void rlc_template_decl_destroy(
	struct RlcTemplateDecl * this);

#ifdef __cplusplus
}
#endif




#endif