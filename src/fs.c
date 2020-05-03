#include "fs.h"
#include "assert.h"
#include "malloc.h"

#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


char const * to_absolute_path(char const * path)
{
	char buffer[PATH_MAX+1];
	char const * abs_path = realpath(path, buffer);
	if(!abs_path)
		return NULL;

	int abs_path_len = strlen(abs_path);
	char * ret = NULL;
	rlc_malloc((void**)&ret, abs_path_len + 1);
	memcpy(ret, abs_path, abs_path_len + 1);

	return ret;
}

int parent_dir(char const * file)
{
	RLC_DASSERT(file != NULL);

	int len = strlen(file);
	while(len >= 0 && file[len] != '/')
		--len;

	return len;
}
char const * concat_paths(
	char const * base_file,
	int base_file_length,
	char const * relative_path,
	unsigned relative_path_length)
{
	RLC_DASSERT(base_file != NULL);
	RLC_DASSERT(relative_path != NULL);


	if(!base_file_length)
		base_file_length = strlen(base_file);
	if(base_file_length == -1)
		base_file_length = 0;

	int need_slash = base_file_length != 0;

	if(!relative_path_length)
		relative_path_length = strlen(relative_path);

	char * concat = NULL;
	rlc_malloc((void**)&concat, base_file_length + need_slash + relative_path_length + 1);

	memcpy(concat, base_file, base_file_length);
	if(need_slash)
		concat[base_file_length] = '/';
	memcpy(
		concat + base_file_length + need_slash,
		relative_path,
		relative_path_length);

	concat[base_file_length + need_slash + relative_path_length] = '\0';

	return concat;
}

char const * resolve_relative_path(
	char const * base_file,
	int base_file_length,
	char const * relative_path,
	unsigned relative_path_length)
{

	char const * concat = concat_paths(
		base_file, base_file_length,
		relative_path, relative_path_length);
	char const * ret = to_absolute_path(concat);
	rlc_free((void**)&concat);
	return ret;
}