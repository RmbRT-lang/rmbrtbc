/** @file struct.h
	Contains the struct type used by the parser. */
#ifndef __rlc_parser_struct_h_defined
#define __rlc_parser_struct_h_defined

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedStruct
{
	struct RlcParsedScopeEntry fScopeEntry;

	struct RlcParsedMember * fMembers;
	size_t fMemberCount;
};

#ifdef __cplusplus
}
#endif