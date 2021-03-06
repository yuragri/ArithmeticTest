// ArithmeticCore.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"


#include "ArithmeticCore.h"
#include "ComputeExpression.h"
#include <iostream>
#include <string>
#include "mysql.h"
#include <fstream>

namespace ArithmeticCore
{
	ArithmeticParser::ArithmeticParser()
	{
		m_dbManager = new DbManager();		
		m_counter = m_dbManager->GetItemsCount();
		std::cout << "Created the Arithmetic Parser object!" << std::endl;
	}

	ArithmeticParser::~ArithmeticParser()
	{
		delete m_dbManager;
		std::cout << "Deleted the Arithmetic Parser object!" << std::endl;
	}

	ARITHMETICCORE_API std::vector<TableInfo> ArithmeticParser::GetHistory()
	{
		auto values = m_dbManager->GetValues();
		return values;
	}

	int ArithmeticParser::ParseExpression(const char* expression)
	{
		auto express = std::string(expression);
		auto compute = std::make_unique<ComputeExpression>(express);
		int result = compute->GetResult();
		++m_counter;
		std::cout << "Parsing "<< m_counter <<" Expression: " << express << std::endl;

		auto path = m_dbManager->WriteToFile(express, result, m_counter);

		m_dbManager->Insert(std::to_string(m_counter), express, path);

		return result;
	}

	
	
}
