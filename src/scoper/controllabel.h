/** @file controllabel.h
	Contains the scoper's control label type. */
#ifndef __rlc_scoper_controllabel_h_defined
#define __rlc_scoper_controllabel_h_defined
#pragma once

#include "string.h"
#include "identifier.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RlcParsedControlLabel;

/** Scoped control label type. */
struct RlcScopedControlLabel
{
	/** Whether the control label exists or is empty. */
	char exists;
	/** Whether the control label is a string or identifier. */
	char isString;
	union {
		/** The control label's string value. */
		struct RlcScopedText string;
		/** The control label's identifier value. */
		struct RlcScopedIdentifier identifier;
	};
};

/** Creates a scoped control label from a parsed control label.
@memberof RlcScopedControlLabel
@param[out] this:
	The scoped control label to create.
	@dassert @nonnull
@param[in] file:
	The source file.
	@dassert @nonnull
@param[in] parsed:
	The parsed control label.
	@dassert @nonnull */
void rlc_scoped_control_label_create(
	struct RlcScopedControlLabel * this,
	struct RlcSrcFile const * file,
	struct RlcParsedControlLabel const * parsed);

/** Destroys a scoped control label.
@memberof RlcScopedControlLabel
@param[in,out] this:
	The control label to destroy.
	@dassert @nonnull */
void rlc_scoped_control_label_destroy(
	struct RlcScopedControlLabel * this);


#ifdef __cplusplus
}
#endif

#endif