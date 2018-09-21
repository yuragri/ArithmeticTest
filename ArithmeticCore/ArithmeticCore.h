#pragma once
//#include "stdafx.h"
#ifdef ARITHMETICCORE_EXPORTS
#define ARITHMETICCORE_API __declspec(dllexport)
#else
#define ARITHMETICCORE_API __declspec(dllimport)
#endif

#include "DbManager.h"
//#include "mysql.h"
#include <string>
#include <vector>
namespace ArithmeticCore
{
	class ArithmeticParser
	{
	public:
		ARITHMETICCORE_API ArithmeticParser();
		ARITHMETICCORE_API ~ArithmeticParser();
		ARITHMETICCORE_API int ParseExpression(const char* expression);
		ARITHMETICCORE_API std::vector<TableInfo> GetHistory();
	private:
		size_t		m_counter;
		DbManager*	m_dbManager;
	};
}