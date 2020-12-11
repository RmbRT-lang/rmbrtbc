#include "operatorexpression.h"

#include "../assert.h"
#include "../malloc.h"

void rlc_parsed_operator_expression_create(
	struct RlcParsedOperatorExpression * this,
	struct RlcToken first,
	struct RlcToken last)
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
	{ kRlcTokDoubleAt, kFullAsync },
	{ kRlcTokLessMinus, kAwait }
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
	{ kRlcTokColonEqual, kAssign },
	{ kRlcTokPlusEqual, kAddAssign },
	{ kRlcTokMinusEqual, kSubAssign },
	{ kRlcTokAsteriskEqual, kMulAssign },
	{ kRlcTokForwardSlashEqual, kDivAssign },
	{ kRlcTokPercentEqual, kModAssign },
	{ kRlcTokAndEqual, kBitAndAssign },
	{ kRlcTokPipeEqual, kBitOrAssign },
	{ kRlcTokCircumflexEqual, kBitXorAssign },
	{ kRlcTokDoubleLessEqual, kShiftLeftAssign },
	{ kRlcTokDoubleGreaterEqual, kShiftRightAssign }
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
	0, // ?:
	11 // :=
};

static struct RlcParsedOperatorExpression * make_operator_expression(
	enum RlcOperator type,
	struct RlcToken first,
	struct RlcToken last)
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
		lhs->fStart,
		rhs->fEnd);
	rlc_parsed_operator_expression_add(out, lhs);
	rlc_parsed_operator_expression_add(out, rhs);

	return out;
}

static struct RlcParsedOperatorExpression * make_unary_expression(
	enum RlcOperator type,
	struct RlcParsedExpression * operand,
	struct RlcToken first,
	struct RlcToken last)
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
							out->fStart,
							token);
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
				&out->fEnd,
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
					out->fStart,
					out->fStart);

			out = RLC_BASE_CAST(temp, RlcParsedExpression);

			int arguments = 0;
			while(!rlc_parser_consume(
				parser,
				&out->fEnd,
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

			for(size_t i = _countof(k_ops); i--;)
			{
				if(rlc_parser_consume(
					parser,
					NULL,
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

	struct RlcToken start;
	for(size_t i = _countof(k_unary); i--;)
	{
		if(rlc_parser_consume(
			parser,
			&start,
			k_unary[i].fTok))
		{
			int isAsync = k_unary[i].fOp == kAsync || k_unary[i].fOp == kFullAsync;
			struct RlcParserTracer trace;
			if(isAsync)
				rlc_parser_trace(parser, "asynchronous call", &trace);

			struct RlcParsedExpression * operand = parse_prefix(parser);

			if(k_unary[i].fOp == kAsync || k_unary[i].fOp == kFullAsync)
			{
				struct RlcParsedOperatorExpression * op;
				if((op = RLC_DYNAMIC_CAST(operand, RlcParsedExpression, RlcParsedOperatorExpression)))
				{
					if(op->fOperator != kCall)
						rlc_parser_fail(parser, "expected call expression");
					rlc_parser_untrace(parser, &trace);
					op->fOperator = k_unary[i].fOp;
					return operand;
				}
			}

			struct RlcParsedOperatorExpression * unary = NULL;
			if(!(unary = make_unary_expression(
					k_unary[i].fOp,
					operand,
					start,
					operand->fEnd)))
			{
				rlc_parser_fail(parser, "expected expression");
			}

			return RLC_BASE_CAST(unary, RlcParsedExpression);
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

	// Special ?: group?
	if(k_binary_groups[group] == 0)
	{
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
				lhs,
				then);
			rlc_parsed_operator_expression_add(cond, other);

			return RLC_BASE_CAST(cond, RlcParsedExpression);
		}
	}

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
				rlc_parser_fail(parser, "expected expression");
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
	RLC_BASE_CAST(this, RlcParsedExpression)->fEnd = expression->fEnd;
}

void rlc_parsed_operator_expression_print(
	struct RlcParsedOperatorExpression const * this,
	struct RlcSrcFile const * file,
	FILE * out)
{
	static struct {
		enum RlcOperator op;
		int position;
		char const * str;
		int needsParentheses;
	} const k_position[] = {
		{kAdd, 1, "+",1}, {kSub, 1, "-",1}, {kMul, 1, "*",1}, {kDiv, 1, "/",1}, {kMod, 1, "%",1},
		{kEquals, 1, "==",1}, {kNotEquals, 1, "!=",1}, {kLess, 1, "<",1},
		{kLessEquals, 1, "<=",1}, {kGreater, 1, ">",1}, {kGreaterEquals, 1, ">=",1},

		{kBitAnd, 1, "&",1}, {kBitOr, 1, "|",1}, {kBitXor, 1, "^",1}, {kBitNot, 0, "~",1},
		{kLogAnd, 1, "&&",1}, {kLogOr, 1, "||",1}, {kLogNot, 0, "!",1},
		{kShiftLeft, 1, "<<",1}, {kShiftRight, 1, ">>",1},
		{kRotateLeft, -1, NULL,1}, {kRotateRight, -1, NULL,1},
		{kNeg, 0, "-",1}, {kPos, 0, "+",1},
		{kSubscript, -1, NULL,0}, {kCall, -1, NULL,0}, {kConditional, -1, NULL,1},
		{kMemberReference, -1, ".",0}, {kMemberPointer, -1, "->",0},
		{kBindReference, 1, ".*",1}, {kBindPointer, 1, "->*",1},
		{kDereference, 0, "*",1}, {kAddress, 0, "&",1},
		{kPreIncrement, 0, "++",1}, {kPreDecrement, 0, "--",1},
		{kPostIncrement, 2, "++",1}, {kPostDecrement, 2, "--",1},

		{kAsync, -1, NULL,1},
		{kFullAsync, -1, NULL,1},
		{kExpectDynamic, -1, NULL,1},
		{kMaybeDynamic, -1, NULL,1},
		{kAwait, 0, " co_await ", 1},

		{kAssign, 1, "=", 1},
		{kAddAssign, 1, "+=", 1}, {kSubAssign, 1, "-=", 1},
		{kMulAssign, 1, "*=", 1}, {kDivAssign, 1, "/=", 1},
		{kModAssign, 1, "%=", 1},

		{kBitAndAssign, 1, "&=", 1}, {kBitOrAssign, 1, "|=", 1},
		{kBitXorAssign, 1, "^=", 1},

		{kShiftLeftAssign, 1, "<<=", 1}, {kShiftRightAssign, 1, ">>=", 1}
	};

	RLC_DASSERT(k_position[this->fOperator].op == this->fOperator);

	if(k_position[this->fOperator].needsParentheses)
		fputc('(', out);

	switch(k_position[this->fOperator].position)
	{
	case 0:
		{
			RLC_DASSERT(this->fExpressionCount == 1);
			fprintf(out, " %s", k_position[this->fOperator].str);
		} break;
	default:;
	}

	if(this->fOperator == kAsync || this->fOperator == kFullAsync)
		fputs("::std::async([&]{ return ", out);

	rlc_parsed_expression_print(
		this->fExpressions[0],
		file,
		out);

	switch(k_position[this->fOperator].position)
	{
	case 1:
		{
			RLC_DASSERT(this->fExpressionCount == 2);
			fprintf(out, " %s ", k_position[this->fOperator].str);
			rlc_parsed_expression_print(
				this->fExpressions[1],
				file,
				out);
		} break;
	case 2:
		{
			RLC_DASSERT(this->fExpressionCount == 1);
			fprintf(out, "%s", k_position[this->fOperator].str);
		} break;
	case -1:
		{
			switch(this->fOperator)
			{
			case kSubscript:
				{
					RLC_DASSERT(this->fExpressionCount == 2);
					fputc('[', out);
					rlc_parsed_expression_print(this->fExpressions[1], file, out);
					fputc(']', out);
				} break;
			case kCall:
			case kAsync:
			case kFullAsync:
				{
					fputc('(', out);
					for(RlcSrcIndex i = 1; i < this->fExpressionCount; i++)
					{
						if(i>1)
							fprintf(out, ", ");
						rlc_parsed_expression_print(this->fExpressions[i], file, out);
					}
					fputc(')', out);
					if(this->fOperator != kCall)
						fputs("; })", out);
				} break;
			case kConditional:
				{
					RLC_DASSERT(this->fExpressionCount == 3);
					fprintf(out, "\n\t? ");
					rlc_parsed_expression_print(this->fExpressions[1], file, out);
					fprintf(out, "\n\t: ");
					rlc_parsed_expression_print(this->fExpressions[2], file, out);
				} break;
			case kMemberReference:
				{
					RLC_DASSERT(this->fExpressionCount == 2);
					fprintf(out, ".");
					rlc_parsed_expression_print(this->fExpressions[1], file, out);
				} break;
			case kMemberPointer:
				{
					RLC_DASSERT(this->fExpressionCount == 2);
					fprintf(out, "->");
					rlc_parsed_expression_print(this->fExpressions[1], file, out);
				} break;
			default:
				RLC_ASSERT(!"not implemented");
			}
		} break;
	default:;
	}

	if(k_position[this->fOperator].needsParentheses)
		fputc(')', out);
}