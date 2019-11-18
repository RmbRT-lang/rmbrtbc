#ifndef __rlc_resolver_fileregistry_h_defined
#define __rlc_resolver_fileregistry_h_defined


#ifdef __cplusplus
extern "C" {
#endif

struct RlcResolvedFileRegistry
{
	char const * fBaseDir;
	struct RlcResolvedFile * fFiles;
	size_t fFileCount;
};

void rlc_resolved_file_registry_create(
	struct RlcResolvedFileRegistry * this);

void rlc_resolved_file_registry_destroy(
	struct RlcResolvedFileRegistry * this);

/** Queries a file from the registry.
	If the file was not registered, tries to parse and resolve it. */
struct RlcResolvedFile const * rlc_resolved_file_registry_get(
	struct RlcResolvedFileRegistry * this,
	char const * file);

#ifdef __cplusplus
}
#endif

#endif