#include "ComputeExpression.h"

namespace ArithmeticCore
{
	ComputeExpression::ComputeExpression(std::string expr):
		m_ExpressionString(expr)
	{
		m_Expression = m_ExpressionString.c_str();
	}

	int ComputeExpression::GetResult()
	{
		return expression();
	}

	char ComputeExpression::peek()
	{
		return *m_Expression;
	}

	char ComputeExpression::get()
	{
		return *m_Expression++;
	}

	//int expression();

	int ComputeExpression::number()
	{
		int result = get() - '0';
		while (peek() >= '0' && peek() <= '9')
		{
			result = 10 * result + get() - '0';
		}
		return result;
	}

	int ComputeExpression::factor()
	{
		if (peek() >= '0' && peek() <= '9')
			return number();
		else if (peek() == '(')
		{
			get(); // '('
			int result = expression();
			get(); // ')'
			return result;
		}
		else if (peek() == '-')
		{
			get();
			return -factor();
		}
		return 0; // error
	}

	int ComputeExpression::term()
	{
		int result = factor();
		while (peek() == '*' || peek() == '/')
			if (get() == '*')
				result *= factor();
			else
				result /= factor();
		return result;
	}

	int ComputeExpression::expression()
	{
		int result = term();
		while (peek() == '+' || peek() == '-')
			if (get() == '+')
				result += term();
			else
				result -= term();
		return result;
	}
}