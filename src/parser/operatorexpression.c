#include "operatorexpression.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_operator_expression_create(
	struct RlcParsedOperatorExpression * this)
{
	RLC_DASSERT(this != NULL);

	rlc_parsed_expression_create(
		RLC_BASE_CAST(this,RlcParsedExpression),
		kRlcParsedOperatorExpression);

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
	{ kRlcTokAsterisk, kDereference }
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
	enum RlcOperator type)
{
	struct RlcParsedOperatorExpression * out = NULL;
	rlc_malloc(
		(void**)&out,
		sizeof(struct RlcParsedOperatorExpression));
	rlc_parsed_operator_expression_create(out);
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

	struct RlcParsedOperatorExpression * out = make_operator_expression(type);
	rlc_parsed_operator_expression_add(out, lhs);
	rlc_parsed_operator_expression_add(out, rhs);

	return out;
}

static struct RlcParsedOperatorExpression * make_unary_expression(
	enum RlcOperator type,
	struct RlcParsedExpression * operand)
{
	if(!operand)
		return NULL;

	struct RlcParsedOperatorExpression * out = make_operator_expression(type);
	rlc_parsed_operator_expression_add(out, operand);

	return out;
}

static struct RlcParsedExpression * parse_postfix(
	struct RlcParserData * parser)
{
	struct RlcParsedExpression * out = NULL;

	size_t const start = parser->fIndex;

	// get elementary expression.
	out = rlc_parsed_expression_parse(
		parser,
		RLC_ALL_FLAGS(RlcParsedExpressionType)
		&~RLC_FLAG(kRlcParsedOperatorExpression)
		&~RLC_FLAG(kRlcParsedTypeNameExpression));

	// without an elementary expression, fail.
	if(!out)
		return NULL;

	enum RlcParseError error_code = kRlcParseErrorExpectedExpression;

	// get all postfix operators, if any.
	for(int postfix = 1; postfix--;)
	{
		// Subscript operator?
		if(rlc_parser_data_consume(
			parser,
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
				goto failure;
			}

			if(!rlc_parser_data_consume(
				parser,
				kRlcTokBracketClose))
			{
				error_code = kRlcParseErrorExpectedBracketClose;
				goto delete_out_failure;
			}

			++postfix;
			continue;
		}
		// function call operator?
		else if(rlc_parser_data_consume(
			parser,
			kRlcTokParentheseOpen))
		{
			struct RlcParsedOperatorExpression * temp =
				make_unary_expression(
					kCall,
					out);

			out = RLC_BASE_CAST(temp, RlcParsedExpression);

			int arguments = 0;
			while(!rlc_parser_data_consume(
				parser,
				kRlcTokParentheseClose))
			{
				// parse comma, if necessary.
				if(arguments++
				&& !rlc_parser_data_consume(
						parser,
						kRlcTokComma))
				{
					error_code = kRlcParseErrorExpectedComma;
					goto delete_out_failure;
				}

				// parse the argument.
				struct RlcParsedExpression * arg = rlc_parsed_expression_parse(
					parser,
					RLC_ALL_FLAGS(RlcParsedExpressionType));

				if(!arg)
					goto delete_out_failure;

				rlc_parsed_operator_expression_add(
					RLC_DERIVE_CAST(
						out,
						RlcParsedExpression,
						struct RlcParsedOperatorExpression),
					arg);
			}

			++postfix;
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

			for(size_t i = _countof(k_ops); i--;)
			{
				if(rlc_parser_data_consume(
					parser,
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
						error_code = kRlcParseErrorExpectedSymbolChildExpression;
						goto delete_out_failure;
					}

					++postfix;
					break;
				}
			}
		}
	}

	return out;
delete_out_failure:
	rlc_parsed_expression_destroy_virtual(out);
	rlc_free((void**)&out);
failure:
	rlc_parser_data_add_error(
		parser,
		error_code);
	return NULL;
}

static struct RlcParsedExpression * parse_prefix(
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);

	// track the beginning index of the expression.
	size_t const first = rlc_parser_data_matched_index(parser);

	struct RlcParsedExpression * out = NULL;
	for(size_t i = _countof(k_unary); i--;)
	{
		if(rlc_parser_data_consume(
			parser,
			k_unary[i].fTok))
		{
			struct RlcParsedOperatorExpression * unary = NULL;
			if(!(unary = make_unary_expression(
					k_unary[i].fOp,
					parse_prefix(parser))))
			{
				rlc_parser_data_add_error(
					parser,
					kRlcParseErrorExpectedExpression);
				return NULL;
			}

			out = RLC_BASE_CAST(unary, RlcParsedExpression);
			out->fFirst = first;
			out->fLast = rlc_parser_data_consumed_index(parser);
			return out;
		}
	}

	// no prefixes?
	return parse_postfix(parser);
}

static struct RlcParsedExpression * parse_binary(
	struct RlcParserData * parser,
	size_t group)
{
	RLC_DASSERT(parser != NULL);

	struct RlcParsedExpression * lhs = group
		? parse_binary(parser, group-1)
		: parse_prefix(parser);
	if(!lhs)
	{
		if(parser->fErrorCount)
			rlc_parser_data_add_error(parser, kRlcParseErrorExpectedExpression);
		return NULL;
	}

	size_t start = 0;
	for(size_t i = 0; i < group; i++)
		start += k_binary_groups[i];

	for(size_t i = start + k_binary_groups[group]; i-->start;)
	{
		if(rlc_parser_data_consume(
			parser,
			k_binary[i].fTok))
		{
			struct RlcParsedOperatorExpression * op =
				make_binary_expression(
					k_binary[i].fOp,
					lhs,
					group
						? parse_binary(parser, group)
						: parse_prefix(parser));
			return RLC_BASE_CAST(op, RlcParsedExpression);
		}
	}

	return lhs;
}

struct RlcParsedExpression * rlc_parsed_operator_expression_parse(
	struct RlcParserData * parser)
{
	RLC_DASSERT(parser != NULL);

	// parse the expression.
	struct RlcParsedExpression * left = parse_binary(parser, _countof(k_binary_groups)-1);

	if(!left)
	{
		return NULL;
	}

	if(rlc_parser_data_consume(
		parser,
		kRlcTokQuestionMark))
	{
		struct RlcParsedExpression * then = rlc_parsed_operator_expression_parse(parser);
		if(!then)
		{
			rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedExpression);
			rlc_parsed_expression_destroy_virtual(left);
			rlc_free((void**)&left);
			return NULL;
		}

		if(!rlc_parser_data_consume(
			parser,
			kRlcTokColon))
		{
			rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedColon);
			rlc_parsed_expression_destroy_virtual(then);
			rlc_parsed_expression_destroy_virtual(left);
			rlc_free((void**)&then);
			rlc_free((void**)&left);
			return NULL;
		}

		struct RlcParsedExpression * other = rlc_parsed_operator_expression_parse(parser);
		if(!other)
		{
			rlc_parser_data_add_error(
				parser,
				kRlcParseErrorExpectedExpression);
			rlc_parsed_expression_destroy_virtual(then);
			rlc_parsed_expression_destroy_virtual(left);
			rlc_free((void**)&then);
			rlc_free((void**)&left);
			return NULL;
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
}