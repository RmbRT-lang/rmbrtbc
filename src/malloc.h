/** @file malloc.h
	Contains memory management functions to enable safer memory management.
	rlc_malloc and rlc_free count the number of allocated objects, so that memory
	leaks are easier to be found out. */

#ifndef __rlc_malloc_h_defined
#define __rlc_malloc_h_defined


#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Allocates memory with size bytes and stores the address in ``*ptr``.
	If no memory could be allocated, terminates the program.
@assert ptr must not be null.
@assert ``*ptr`` must be null.
@param[out] ptr:
	The address of a pointer that should be set
	to the allocated memory address.
@param[in] size: The size of the allocated memory, in bytes. */
void rlc_malloc(void ** ptr, size_t size);

/** Reallocates the memory pointed to by ``*ptr`` to fit the given size.
	If a pointer to a null pointer is passed, acts the same way as rlc_malloc.
@assert ptr must not be null.
@param[in,out] ptr:
	The address of a pointer that should be set
	to the reallocated memory address.
@param[in] newsz: The byte size the reallocated memory should have. */
void rlc_realloc(void ** ptr, size_t newsz);

/** Frees the pointer pointed to by ptr and sets it to null.
@assert ptr must not be null.
@assert ``*ptr`` must not be null.
@assert rlc_allocations must not be 0.
@param[in,out] ptr: A pointer to the pointer to be freed. */
void rlc_free(void ** ptr);

/** Returns the bilance of current allocations. */
size_t rlc_allocations();

#ifdef __cplusplus
}
#endif

#endif