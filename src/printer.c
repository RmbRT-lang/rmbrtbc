#include "printer.h"
#include "assert.h"
#include "parser/templatedecl.h"

void rlc_printer_add_ctx(
	struct RlcPrinter * printer,
	struct RlcPrinterCtx * ctx,
	struct RlcSrcString const * str,
	struct RlcParsedTemplateDecl const * tpl)
{
	ctx->next = NULL;
	ctx->prev = printer->innerCtx;
	if(printer->innerCtx)
		printer->innerCtx->next = ctx;
	printer->innerCtx = ctx;
	if(!printer->outerCtx)
		printer->outerCtx = ctx;
	ctx->str = str;
	ctx->tpl = tpl;
}

void rlc_printer_pop_ctx(
	struct RlcPrinter * printer)
{
	RLC_DASSERT(printer->innerCtx);
	printer->innerCtx = printer->innerCtx->prev;
	if(!printer->innerCtx)
		printer->outerCtx = NULL;
	else
		printer->innerCtx->next = NULL;
}

void rlc_printer_add_ns(
	struct RlcPrinter * printer,
	struct RlcPrinterNamespace * ns,
	struct RlcSrcString const * str)
{
	ns->next = NULL;
	ns->prev = printer->innerNs;
	if(printer->innerNs)
		printer->innerNs->next = ns;
	printer->innerNs = ns;
	if(!printer->outerNs)
		printer->outerNs = ns;
	ns->str = str;
}

void rlc_printer_pop_ns(
	struct RlcPrinter * printer)
{
	RLC_DASSERT(printer->innerNs);
	printer->innerNs = printer->innerNs->prev;
	if(!printer->innerNs)
		printer->outerNs = NULL;
	else
		printer->innerNs->next = NULL;
}

void rlc_printer_print_ctx_tpl(
	struct RlcPrinter const * p,
	struct RlcSrcFile const * file,
	FILE * out)
{
	for(struct RlcPrinterCtx * ctx = p->outerCtx; ctx != NULL; ctx = ctx->next)
	{
		if(ctx->tpl)
			rlc_parsed_template_decl_print(
				ctx->tpl,
				file,
				out);
	}
}

void rlc_printer_print_ctx_symbol(
	struct RlcPrinter const * p,
	struct RlcSrcFile const * file,
	FILE * out)
{
	int tail = 0;
	for(struct RlcPrinterCtx * ctx = p->outerCtx; ctx != NULL; ctx = ctx->next)
	{
		if(tail)
		{
			fputs("::", out);
			if(ctx->tpl && ctx->tpl->fChildCount)
				fputs("template ", out);
		}
		tail = 1;

		rlc_src_string_print(ctx->str, file, out);

		if(ctx->tpl && rlc_parsed_template_decl_exists(ctx->tpl))
		{
			fputs("<", out);
			for(RlcSrcIndex i = 0; i < ctx->tpl->fChildCount; i++)
			{
				if(i)
					fputs(", ", out);

				rlc_src_string_print(&ctx->tpl->fChildren[i].fName, file, out);
			}
			fputs(">", out);
		}
	}
}

void rlc_printer_print_ctx_symbol_with_namespace_rl_flavour(
	struct RlcPrinter const * p,
	struct RlcSrcFile const * file,
	FILE * out)
{
	int tail = 0;

	for(struct RlcPrinterNamespace * ctx = p->outerNs; ctx != NULL; ctx = ctx->next)
	{
		if(tail)
			fputs("::", out);
		tail = 1;
		rlc_src_string_print_noreplace(ctx->str, file, out);
	}

	for(struct RlcPrinterCtx * ctx = p->outerCtx; ctx != NULL; ctx = ctx->next)
	{
		if(tail)
			fputs("::", out);
		tail = 1;

		if(ctx->tpl && rlc_parsed_template_decl_exists(ctx->tpl))
		{
			fputs("[", out);
			for(RlcSrcIndex i = 0; i < ctx->tpl->fChildCount; i++)
			{
				if(i)
					fputs(", ", out);

				rlc_src_string_print_noreplace(&ctx->tpl->fChildren[i].fName, file, out);
			}
			fputs("]", out);
		}

		rlc_src_string_print_noreplace(ctx->str, file, out);
	}
}