#include "file.h"
#include "fileregistry.h"
#include "../malloc.h"
#include "../printer.h"

void rlc_scoped_file_create(
	struct RlcScopedFile * this,
	char const * path,
	struct RlcParsedFile * parsed)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(path != NULL);

	this->path = path;
	this->parsed = parsed;
	this->lastPrinted = 0;
	rlc_scoped_include_list_create(&this->includes);
	rlc_scoped_include_list_create(&this->includedBy);
}

void rlc_scoped_file_destroy(
	struct RlcScopedFile * this)
{
	RLC_DASSERT(this != NULL);

	rlc_free((void**)&this->path);
	rlc_scoped_include_list_destroy(&this->includes);
	rlc_scoped_include_list_destroy(&this->includedBy);
}

void rlc_scoped_file_populate_includes(
	struct RlcScopedFile * this,
	struct RlcScopedFileRegistry * registry,
	struct RlcParsedFile const * parsed)
{
	struct RlcScopedIncludeStatement inc_stmt;
	for(size_t i = 0; i < parsed->fIncludeCount; i++)
	{
		struct RlcScopedFile * inc_file = rlc_scoped_file_registry_get(
			registry,
			rlc_scope_include_statement(
				&inc_stmt,
				&parsed->fIncludes[i],
				&parsed->fSource,
				registry));
		rlc_scoped_include_statement_destroy(&inc_stmt);
		RLC_DASSERT(inc_file);

		// Ignore direct cycles.
		if(inc_file != this)
		{
			rlc_scoped_include_list_add(&this->includes, inc_file, 0);
			rlc_scoped_include_list_add(&inc_file->includedBy, this, 0);
		}

		for(RlcSrcIndex i = 0; i < inc_file->includes.fPathCount; i++)
		{
			struct RlcScopedFile * transitive_inc = inc_file->includes.fPaths[i].fFile;
			// Ignore indirect cycles.
			if(transitive_inc != this)
			{
				rlc_scoped_include_list_add(&this->includes, transitive_inc, 0);
				rlc_scoped_include_list_add(&transitive_inc->includedBy, this, 0);
			}
		}
	}
}

void rlc_scoped_file_print(
	struct RlcScopedFile * this,
	struct RlcScopedFileRegistry * registry,
	struct RlcPrinter const * printer)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(registry != NULL);
	RLC_DASSERT(printer != NULL);

	if(this->lastPrinted > printer->fCompilationUnit)
		return;


	this->lastPrinted = printer->fCompilationUnit + 1;

	for(RlcSrcIndex i = 0; i < this->includes.fPathCount; i++)
		rlc_scoped_file_print(
			this->includes.fPaths[i].fFile,
			registry,
			printer);

	fprintf(printer->fTypes, "////// %s:Types\n", this->path);
	fprintf(printer->fVars, "////// %s:Vars\n", this->path);
	fprintf(printer->fFuncs, "////// %s:Funcs\n", this->path);
	fprintf(printer->fTypesImpl, "////// %s:TypesImpl\n", this->path);
	fprintf(printer->fVarsImpl, "////// %s:VarsImpl\n", this->path);
	fprintf(printer->fFuncsImpl, "////// %s:FuncsImpl\n", this->path);
	rlc_parsed_file_print(
		this->parsed,
		printer);
}