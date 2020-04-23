/** @file templateargs.h
	Contains the resolved template arguments type. */
#ifndef __rlc_resolver_templateargs_h_defined
#define __rlc_resolver_templateargs_h_defined
#pragma once

#include "value.h"
#include "typename.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcScopedTemplateArgument;
struct RlcScopedSymbolChild;

struct RlcResolvedTemplateArg
{
	/** Whether the template argument is an expression or a type. */
	int isExpression;
	union
	{
		struct RlcValue value;
		struct RlcResolvedTypeName type;
	};
};

struct RlcResolvedTemplateArgs
{
	struct RlcResolvedTemplateArg * args;
	RlcSrcSize argCount;
};

void rlc_resolved_template_args_create(
	struct RlcResolvedTemplateArgs * this,
	struct RlcScopedSymbolChild const * symbol,
	struct RlcScopedScope * scope);

void rlc_resolved_template_args_destroy(
	struct RlcResolvedTemplateArgs * this);

void rlc_resolved_template_arg_resolve(
	struct RlcResolvedTemplateArg * this,
	struct RlcScopedTemplateArgument const * scoped,
	struct RlcScopedScope * scope);

#ifdef __cplusplus
}
#endif

#endif