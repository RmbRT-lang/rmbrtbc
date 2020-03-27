/** @file templatedecl.h
	Contains the scoped template declaration type. */
#ifndef __rlc_scoper_templatedecl_h_defined
#define __rlc_scoper_templatedecl_h_defined
#pragma once

#include "identifier.h"
#include "typename.h"

#include "../parser/templatedecl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A single template declaration argument. */
struct RlcScopedTemplateDeclChild
{
	/** The template argument's name. */
	struct RlcScopedIdentifier name;
	/** The template argument's type. */
	enum RlcParsedTemplateDeclType type;
	/** If the template argument is a value, the value's type. */
	struct RlcScopedTypeName valueType;
};

/** A template declaration. */
struct RlcScopedTemplateDecl
{
	/** The template declaration's arguments. */
	struct RlcScopedTemplateDeclChild * children;
	/** The template declaration's argument count. */
	RlcSrcSize childCount;
};

/** Creates a scoped template declaration from a parsed template declaration.
@memberof RlcScopedTemplateDecl
@param[out] this:
	The template declaration to create.
	@dassert @nonnull
@param[in] file:
	The template declaration's source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed template declaration.
	@dassert @nonnull */
void rlc_scoped_template_decl_create(
	struct RlcScopedTemplateDecl * this,
	struct RlcSrcFile const * file,
	struct RlcParsedTemplateDecl const * parsed);

/** Destroys a scoped template declaration.
@memberof RlcScopedTemplateDecl
@param[in,out] this:
	The template declaration to destroy.
	@dassert @nonnull */
void rlc_scoped_template_decl_destroy(
	struct RlcScopedTemplateDecl * this);

#ifdef __cplusplus
}
#endif


#endif