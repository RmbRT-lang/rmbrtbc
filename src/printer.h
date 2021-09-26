#ifndef __rlc_scoper_printer_h_defined
#define __rlc_scoper_printer_h_defined
#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RlcSrcString;
struct RlcSrcFile;
struct RlcParsedTemplateDecl;

struct RlcPrinterCtx
{
	struct RlcSrcString const * str;
	struct RlcParsedTemplateDecl const * tpl;
	struct RlcPrinterCtx * next, * prev;
};

struct RlcPrinterNamespace
{
	struct RlcSrcString const * str;
	struct RlcPrinterNamespace * next, * prev;
};

struct RlcPrinter
{
	/** The number of the current compilation unit. */
	unsigned fCompilationUnit;
	int fIsTest;
	FILE * fSymbolConstants;
	FILE * fTypes;
	FILE * fVars;
	FILE * fFuncs;
	FILE * fTypesImpl;
	FILE * fVarsImpl;
	FILE * fFuncsImpl;

	struct RlcPrinterCtx * outerCtx;
	struct RlcPrinterCtx * innerCtx;

	struct RlcPrinterNamespace * outerNs;
	struct RlcPrinterNamespace * innerNs;
};

void rlc_printer_add_ctx(
	struct RlcPrinter * printer,
	struct RlcPrinterCtx * ctx,
	struct RlcSrcString const * str,
	struct RlcParsedTemplateDecl const * tpl);
void rlc_printer_pop_ctx(
	struct RlcPrinter * printer);

void rlc_printer_add_ns(
	struct RlcPrinter * printer,
	struct RlcPrinterNamespace * ns,
	struct RlcSrcString const * str);
void rlc_printer_pop_ns(
	struct RlcPrinter * printer);

void rlc_printer_print_ctx_tpl(
	struct RlcPrinter const * p,
	struct RlcSrcFile const * file,
	FILE * out);

void rlc_printer_print_ctx_symbol(
	struct RlcPrinter const * p,
	struct RlcSrcFile const * file,
	FILE * out);

void rlc_printer_print_ctx_symbol_with_namespace_rl_flavour(
	struct RlcPrinter const * p,
	struct RlcSrcFile const * file,
	FILE * out);

#ifdef __cplusplus
}
#endif

#endif