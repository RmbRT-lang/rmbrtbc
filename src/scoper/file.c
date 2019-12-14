#include "file.h"
#include "../malloc.h"

void rlc_scoped_file_destroy(
	struct RlcScopedFile * this)
{
	rlc_free((void**)&this->path);
	rlc_include_path_list_destroy(&this->includes);
}