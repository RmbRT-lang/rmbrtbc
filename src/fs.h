#ifndef __rlc_fs_h_defined
#define __rlc_fs_h_defined

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Retrieves the absolute path of a file path.
@return
	NULL if the requested file does not exist. */
char const * to_absolute_path(char const * path);

/** Finds the parent directory name of a file or directory.
@return
	0 on failure, otherwise, the index of the '/' after the parent directory. */
int parent_dir(char const * file);

char const * concat_paths(
	char const * base_file,
	int base_file_length,
	char const * relative_path,
	unsigned relative_path_length);

/** Resolves a relative file path with respect to a base file path.
@param[in] base_file:
	The base file path.
@param[in] base_file_length:
	The string length of base_file to use. 0 if it should be detected automatically.
@param[in] relative_path:
	A path relative to the base file.
@param[in] relative_path_length:
	The string length of relative_path_length to use. 0 if it should be detected automatically.
@return
	The resolved path, or NULL, if the file does not exist. */
char const * resolve_relative_path(
	char const * base_file,
	int base_file_length,
	char const * relative_path,
	unsigned relative_path_length);

#ifdef __cplusplus
}
#endif

#endif