#pragma once
#include <string>
namespace ArithmeticCore
{
	class ComputeExpression
	{
	public:
		ComputeExpression(std::string expr);
		int GetResult();
	private:
		std::string m_ExpressionString;
		const char* m_Expression;
		//utils for parsing
		char peek();
		char get();
		int number();
		int factor();
		int term();
		int expression();
	};
}