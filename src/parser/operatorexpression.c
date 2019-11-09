#include "operatorexpression.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_operator_expression_create(
	struct RlcParsedOperatorExpression * this,
	RlcSrcIndex first,
	RlcSrcIndex last)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this,RlcParsedExpression),
		kRlcParsedOperatorExpression,
		first,
		last);

	this->fExpressions = NULL;
	this->fExpressionCount = 0;
}

void rlc_parsed_operator_expression_destroy(
	struct RlcParsedOperatorExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_destroy_base(
		RLC_BASE_CAST(this,RlcParsedExpression));

	for(size_t i = 0; i < this->fExpressionCount; i++)
	{
		rlc_parsed_expression_destroy_virtual(this->fExpressions[i]);
		rlc_free((void**)&this->fExpressions[i]);
	}

	if(this->fExpressions)
	{
		rlc_free((void**)&this->fExpressions);
	}
	this->fExpressionCount = 0;
}

enum OperatorType { kUnary, kBinary, kNary = kBinary };

static struct {
	enum RlcTokenType fTok;
	enum RlcOperator fOp;
}
// prefix operators.
const k_unary[] = {
	{ kRlcTokMinus, kNeg },
	{ kRlcTokPlus, kPos },
	{ kRlcTokTilde, kBitNot },
	{ kRlcTokExclamationMark, kLogNot },
	{ kRlcTokAnd, kAddress },
	{ kRlcTokAsterisk, kDereference },
	{ kRlcTokDoublePlus, kPreIncrement },
	{ kRlcTokDoubleMinus, kPreDecrement },
	{ kRlcTokDoubleDotExclamationMark, kExpectDynamic },
	{ kRlcTokDoubleDotQuestionMark, kMaybeDynamic },
	{ kRlcTokAt, kAsync },
	{ kRlcTokDoubleAt, kFullAsync }
},
// binary operators.
k_binary[] = {
	// bind operators.
	{ kRlcTokDotAsterisk, kBindReference },
	{ kRlcTokMinusGreaterAsterisk, kBindPointer },

	// multiplicative operators.
	{ kRlcTokPercent, kMod },
	{ kRlcTokForwardSlash, kDiv },
	{ kRlcTokAsterisk, kMul },

	// additive operators.
	{ kRlcTokMinus, kSub },
	{ kRlcTokPlus, kAdd },

	// bit shift operators.
	{ kRlcTokDoubleLess, kShiftLeft },
	{ kRlcTokDoubleGreater, kShiftRight },
	{ kRlcTokTripleLess, kRotateLeft },
	{ kRlcTokTripleGreater, kRotateRight },

	// bit arithmetic operators.
	{ kRlcTokAnd, kBitAnd },
	{ kRlcTokCircumflex, kBitXor },
	{ kRlcTokPipe, kBitOr },

	// numeric comparisons.
	{ kRlcTokLess, kLess },
	{ kRlcTokLessEqual, kLessEquals },
	{ kRlcTokGreater, kGreater },
	{ kRlcTokGreaterEqual, kGreaterEquals },
	{ kRlcTokDoubleEqual, kEquals },
	{ kRlcTokExclamationMarkEqual, kNotEquals },

	// boolean arithmetic.
	{ kRlcTokDoubleAnd, kLogAnd },
	{ kRlcTokDoublePipe, kLogOr },

	// assignments.
	{ kRlcTokColonEqual, kAssign }
};

static size_t const k_binary_groups[] = {
	2, // .* ->*
	3, // % / *
	2, // + -
	4, // << >> <<< >>>
	1, // &
	1, // ^
	1, // |
	6, // < <= > >= == !=
	1, // &&
	1, // ||
	1 // :=
};

static struct RlcParsedOperatorExpression * make_operator_expression(
	enum RlcOperator type,
	RlcSrcIndex first,
	RlcSrcIndex last)
{
	struct RlcParsedOperatorExpression * out = NULL;
	rlc_malloc(
		(void**)&out,
		sizeof(struct RlcParsedOperatorExpression));
	rlc_parsed_operator_expression_create(out, first, last);
	out->fOperator = type;

	return out;
}

static struct RlcParsedOperatorExpression * make_binary_expression(
	enum RlcOperator type,
	struct RlcParsedExpression * lhs,
	struct RlcParsedExpression * rhs)
{
	if(!lhs)
	{
		if(rhs)
		{
			rlc_parsed_expression_destroy_virtual(rhs);
			rlc_free((void**)&rhs);
		}

		return NULL;
	}
	else if(!rhs)
	{
		if(lhs)
		{
			rlc_parsed_expression_destroy_virtual(lhs);
			rlc_free((void**)&lhs);
		}

		return NULL;
	}

	struct RlcParsedOperatorExpression * out = make_operator_expression(
		type,
		lhs->fFirst,
		rhs->fLast);
	rlc_parsed_operator_expression_add(out, lhs);
	rlc_parsed_operator_expression_add(out, rhs);

	return out;
}

static struct RlcParsedOperatorExpression * make_unary_expression(
	enum RlcOperator type,
	struct RlcParsedExpression * operand,
	size_t first,
	size_t last)
{
	if(!operand)
		return NULL;

	struct RlcParsedOperatorExpression * out =
		make_operator_expression(type, first, last);
	rlc_parsed_operator_expression_add(out, operand);

	return out;
}

static _Nodiscard struct RlcParsedExpression * parse_postfix(
	struct RlcParser * parser)
{
	struct RlcParsedExpression * out = NULL;

	// get elementary expression.
	out = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType)
		&~RLC_FLAG(kRlcParsedOperatorExpression));

	// without an elementary expression, fail.
	if(!out)
		return NULL;

	// get all postfix operators, if any.
	for(int postfix = 1; postfix--;)
	{
		static struct {
			enum RlcTokenType fTok;
			enum RlcOperator fOp;
		} const k_unary_postfix[] = {
			{ kRlcTokDoublePlus, kPostIncrement },
			{ kRlcTokDoubleMinus, kPostDecrement }
		};

		{
			int found = 0;
			struct RlcToken token;
			for(size_t i = _countof(k_unary_postfix); i--;)
				if(rlc_parser_consume(
					parser,
					&token,
					k_unary_postfix[i].fTok))
				{
					struct RlcParsedOperatorExpression * temp =
						make_unary_expression(
							k_unary_postfix[i].fOp,
							out,
							out->fFirst,
							rlc_src_string_end(&token.content));
					out = RLC_BASE_CAST(
						temp,
						RlcParsedExpression);

					found = 1;
					break;
				}
			if(found)
			{
				++postfix;
				continue;
			}
		}
		// Subscript operator?
		if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokBracketOpen))
		{
			struct RlcParsedOperatorExpression * binary =
				make_binary_expression(
					kSubscript,
					out,
					rlc_parsed_expression_parse(
						parser,
						RLC_ALL_FLAGS(RlcParsedExpressionType)));
			// index expression.
			if(!(out = RLC_BASE_CAST(
				binary,
				RlcParsedExpression)))
			{
				rlc_parser_fail(parser, "expected an expression");
			}

			rlc_parser_expect(
				parser,
				NULL,
				1,
				kRlcTokBracketClose);

			++postfix;
			continue;
		}
		// function call operator?
		else if(rlc_parser_consume(
			parser,
			NULL,
			kRlcTokParentheseOpen))
		{
			struct RlcParsedOperatorExpression * temp =
				make_unary_expression(
					kCall,
					out,
					out->fFirst,
					0);

			out = RLC_BASE_CAST(temp, RlcParsedExpression);

			struct RlcToken parenthese;
			int arguments = 0;
			while(!rlc_parser_consume(
				parser,
				&parenthese,
				kRlcTokParentheseClose))
			{
				// parse comma, if necessary.
				if(arguments++)
					rlc_parser_expect(
						parser,
						NULL,
						1,
						kRlcTokComma);

				// parse the argument.
				struct RlcParsedExpression * arg = rlc_parsed_expression_parse(
					parser,
					RLC_ALL_FLAGS(RlcParsedExpressionType));

				if(!arg)
					rlc_parser_fail(parser, "expected argument");

				rlc_parsed_operator_expression_add(
					RLC_DERIVE_CAST(
						out,
						RlcParsedExpression,
						struct RlcParsedOperatorExpression),
					arg);
			}

			out->fLast = rlc_src_string_end(&parenthese.content);

			++postfix;
			continue;
		}
		// member access operator?
		else
		{
			static struct {
				enum RlcTokenType fToken;
				enum RlcOperator fOperator;
			} const k_ops[] = {
				{ kRlcTokDot, kMemberReference },
				{ kRlcTokMinusGreater, kMemberPointer }
			};

			struct RlcToken token;
			for(size_t i = _countof(k_ops); i--;)
			{
				if(rlc_parser_consume(
					parser,
					&token,
					k_ops[i].fToken))
				{
					struct RlcParsedOperatorExpression * temp =
						make_binary_expression(
							k_ops[i].fOperator,
							out,
							rlc_parsed_expression_parse(
								parser,
								RLC_FLAG(kRlcParsedSymbolChildExpression)));

					if(!(out = RLC_BASE_CAST(temp, RlcParsedExpression)))
					{
						rlc_parser_fail(parser, "expected symbol child expression");
					}

					++postfix;
					break;
				}
			}
		}
	}

	return out;
}

static struct RlcParsedExpression * parse_prefix(
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);

	// track the beginning index of the expression.
	size_t const first = rlc_parser_index(parser);

	for(size_t i = _countof(k_unary); i--;)
	{
		if(rlc_parser_consume(
			parser,
			NULL,
			k_unary[i].fTok))
		{
			struct RlcParsedExpression * operand = parse_prefix(parser);
			struct RlcParsedOperatorExpression * unary = NULL;
			if(!(unary = make_unary_expression(
					k_unary[i].fOp,
					operand,
					first,
					0)))
			{
				rlc_parser_fail(parser, "expected expression");
			}

			struct RlcParsedExpression * out = RLC_BASE_CAST(unary, RlcParsedExpression);
			out->fLast = operand->fLast;

			return out;
		}
	}

	// no prefixes?
	return parse_postfix(parser);
}

static struct RlcParsedExpression * parse_binary(
	struct RlcParser * parser,
	size_t group)
{
	RLC_DASSERT(parser != NULL);

	struct RlcParsedExpression * lhs = group
		? parse_binary(parser, group-1)
		: parse_prefix(parser);

	if(!lhs)
		return NULL;

	size_t start = 0;
	for(size_t i = 0; i < group; i++)
		start += k_binary_groups[i];

	for(size_t i = start + k_binary_groups[group]; i-->start;)
	{
		if(rlc_parser_consume(
			parser,
			NULL,
			k_binary[i].fTok))
		{
			struct RlcParsedOperatorExpression * op =
				make_binary_expression(
					k_binary[i].fOp,
					lhs,
					group
						? parse_binary(parser, group)
						: parse_prefix(parser));
			if(!op)
				rlc_parser_fail(parser, "expeected expression");
			return RLC_BASE_CAST(op, RlcParsedExpression);
		}
	}

	return lhs;
}

struct RlcParsedExpression * rlc_parsed_operator_expression_parse(
	struct RlcParser * parser)
{
	RLC_DASSERT(parser != NULL);

	// parse the expression.
	struct RlcParsedExpression * left = parse_binary(parser, _countof(k_binary_groups)-1);

	if(!left)
	{
		return NULL;
	}

	if(rlc_parser_consume(
		parser,
		NULL,
		kRlcTokQuestionMark))
	{
		struct RlcParsedExpression * then = rlc_parsed_operator_expression_parse(parser);
		if(!then)
		{
			rlc_parser_fail(parser, "expected expression");
		}

		rlc_parser_expect(
			parser,
			NULL,
			1,
			kRlcTokColon);

		struct RlcParsedExpression * other = rlc_parsed_operator_expression_parse(parser);
		if(!other)
		{
			rlc_parser_fail(parser, "expected expression");
		}

		struct RlcParsedOperatorExpression * cond = make_binary_expression(
			kConditional,
			left,
			then);
		rlc_parsed_operator_expression_add(cond, other);

		return RLC_BASE_CAST(cond, RlcParsedExpression);
	}

	return left;
}

void rlc_parsed_operator_expression_add(
	struct RlcParsedOperatorExpression * this,
	struct RlcParsedExpression * expression)
{
	RLC_DASSERT(this != NULL);
	RLC_DASSERT(expression != NULL);

	rlc_realloc(
		(void**)&this->fExpressions,
		sizeof(struct RlcParsedExpression*) * ++this->fExpressionCount);

	this->fExpressions[this->fExpressionCount-1] = expression;
	RLC_BASE_CAST(this, RlcParsedExpression)->fLast = expression->fLast;
}