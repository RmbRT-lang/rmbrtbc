/** @file member.h
	Contains the type that stores member information used by the parser. */
#ifndef __rlc_parser_member_h_defined
#define __rlc_parser_member_h_defined

#include "../macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The protection level of a member. */
enum RlcVisibility
{
	kRlcVisibilityPublic,
	kRlcVisibilityProtected,
	kRlcVisibilityPrivate,

	RLC_ENUM_END(RlcVisibility)
};

enum Rlc
{

};

/** Contains information of member declarations. */
struct RlcParsedMember
{
	enum RlcAccess fAccess;
};


#ifdef __cplusplus
}
#endif

#endif