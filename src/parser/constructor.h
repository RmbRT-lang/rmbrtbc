#ifndef __rlc_parser_lifetime_h_defined
#define __rlc_parser_lifetime_h_defined

#include "../src/string.h"
#include "blockstatement.h"
#include "parser.h"
#include "templatedecl.h"
#include "variable.h"

#include "../macros.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Identifies the kind of a constructor. */
enum RlcConstructorType
{
	kRlcDefaultConstructor,
	kRlcCopyConstructor,
	kRlcMoveConstructor,
	kRlcCustomConstructor
};

/** A constructor as used in the parser.
@extends RlcParsedMember */
struct RlcParsedConstructor
{
	RLC_DERIVE(struct,RlcParsedMember);

	/** The template parameters. */
	struct RlcParsedTemplateDecl fTemplates;

	/** The constructor type. */
	enum RlcConstructorType fType;

	union {
		struct {
			int fIsVariant;
			struct RlcToken fVariant;
			/** The arguments. */
			struct RlcParsedVariable * fArguments;
			/** The argument count. */
			size_t fArgumentCount;
		};
		/** For copy and move constructors, the argument's name. */
		struct RlcSrcString fArgName;
	};

	/** Whether the constructor is a definition. */
	int fIsDefinition;

	/** The base initialisers. */
	struct RlcParsedBaseInit * fBaseInits;
	/** The base initialiser count. */
	size_t fBaseInitCount;

	/** The initialisers. */
	struct RlcParsedInitialiser * fInitialisers;
	/** The initialiser count. */
	size_t fInitialiserCount;

	/** The body statement. */
	struct RlcParsedBlockStatement fBody;
	/** Whether the constructor is inline. */
	int fIsInline;
};

/** Creates a constructor.
@memberof RlcParsedConstructor
@param[out] this:
	The constructor to create.
	@dassert @nonnull
@param[in] visibility:
	The constructor's visibility. */
void rlc_parsed_constructor_create(
	struct RlcParsedConstructor * this,
	struct RlcParsedMemberCommon const * member);

/** Destroys a constructor.
@memberof RlcParsedConstructor
@param[in,out] this:
	The constructor to destroy.
	@dassert @nonnull */
void rlc_parsed_constructor_destroy(
	struct RlcParsedConstructor * this);

/** Parses a constructor.
@memberof RlcParsedConstructor
@param[out] out:
	The constructor to parse into.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull
@param[in] member:
	@dassert @nonnull
@return
	Nonzero on success. */
_Nodiscard int rlc_parsed_constructor_parse(
	struct RlcParsedConstructor * out,
	struct RlcParser * parser,
	struct RlcParsedMemberCommon const * member);

/** Adds an argument to a constructor.
@memberof RlcParsedConstructor
@param[in,out] this:
	The constructor to add an argument to.
	@dassert @nonnull
@param[in] argument:
	The argument to add.
	@pass_ownership
	@dassert @nonnull */
void rlc_parsed_constructor_add_argument(
	struct RlcParsedConstructor * this,
	struct RlcParsedVariable * argument);

/** Adds an initialiser to a constructor.
@memberof RlcParsedConstructor
@param[in,out] this:
	The constructor to add an initialiser to.
	@dassert @nonnull
@param[in] initialiser:
	The initialiser to add.
	@pass_ownership
	@dassert @nonnull */
void rlc_parsed_constructor_add_initialiser(
	struct RlcParsedConstructor * this,
	struct RlcParsedInitialiser * initialiser);

void rlc_parsed_constructor_add_base_init(
	struct RlcParsedConstructor * this,
	struct RlcParsedBaseInit * initialiser);

/** A member initaliser inside a constructor as used by the parser.
@related RlcParsedConstructor */
struct RlcParsedInitialiser
{
	/** The meber's name. */
	struct RlcSrcString fMember;

	int fIsNoInit;
	/** The arguments. */
	struct RlcParsedExpression ** fArguments;
	/** The argument count. */
	size_t fArgumentCount;
};

/** A member initaliser inside a constructor as used by the parser.
@related RlcParsedConstructor */
struct RlcParsedBaseInit
{
	/** The base class' name. */
	struct RlcParsedSymbol fBase;

	int fIsNoInit;
	/** The arguments. */
	struct RlcParsedExpression ** fArguments;
	/** The argument count. */
	size_t fArgumentCount;
};

/** Creates an initialiser.
@memberof RlcParsedInitialiser
@param[out] this:
	The initialiser to create.
	@dassert @nonnull */
void rlc_parsed_initialiser_create(
	struct RlcParsedInitialiser * this);

/** Destroys an initialiser.
@memberof RlcParsedInitialiser
@param[in,out] this:
	The initialiser to destroy.
	@dassert @nonnull */
void rlc_parsed_initialiser_destroy(
	struct RlcParsedInitialiser * this);

/** Parses an initialiser.
@memberof RlcParsedInitialiser
@param[out] out:
	The initialiser to parse into.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull */
void rlc_parsed_initialiser_parse(
	struct RlcParsedInitialiser * out,
	struct RlcParser * parser);

/** Adds an argument to an initaliser.
@memberof RlcParsedInitialiser
@param[in,out] this:
	The intialiser to add an argument to.
	@dassert @nonnul
@param[in] argument:
	The argument to add.
	@pass_pointer_ownership
	@dassert @nonnull */
void rlc_parsed_initialiser_add_argument(
	struct RlcParsedInitialiser * this,
	struct RlcParsedExpression * argument);


/** Creates an initialiser.
@memberof RlcParsedBaseInit
@param[out] this:
	The initialiser to create.
	@dassert @nonnull */
void rlc_parsed_base_init_create(
	struct RlcParsedBaseInit * this);

/** Destroys an initialiser.
@memberof RlcParsedBaseInit
@param[in,out] this:
	The initialiser to destroy.
	@dassert @nonnull */
void rlc_parsed_base_init_destroy(
	struct RlcParsedBaseInit * this);

/** Parses an initialiser.
@memberof RlcParsedBaseInit
@param[out] out:
	The initialiser to parse into.
	@dassert @nonnull
@param[in,out] parser:
	The parser data.
	@dassert @nonnull */
void rlc_parsed_base_init_parse(
	struct RlcParsedBaseInit * out,
	struct RlcParser * parser);

/** Adds an argument to an initaliser.
@memberof RlcParsedBaseInit
@param[in,out] this:
	The intialiser to add an argument to.
	@dassert @nonnul
@param[in] argument:
	The argument to add.
	@pass_pointer_ownership
	@dassert @nonnull */
void rlc_parsed_base_init_add_argument(
	struct RlcParsedBaseInit * this,
	struct RlcParsedExpression * argument);

#ifdef __cplusplus
}
#endif

#endif