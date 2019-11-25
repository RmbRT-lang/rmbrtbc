#include "file.h"
#include "../malloc.h"

void rlc_resolved_file_destroy(
	struct RlcResolvedFile * this)
{
	rlc_free((void**)&this->path);
	rlc_include_path_list_destroy(&this->includes);
}