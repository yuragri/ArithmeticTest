#pragma once
#include "mysql.h"
#include <string>
#include <vector>

namespace ArithmeticCore
{
	struct DBInfo
	{
		const char* host;
		const char* user;
		const char* passwd;
		const char* dbname;
		const char* tablename;
		size_t		port;
	};

	struct TableInfo
	{
		std::string counter;
		std::string date;
		std::string expression;
		std::string result;
	};

	class DbManager
	{
	public:
		DbManager();
		~DbManager();
		int GetItemsCount();
		void Insert(std::string id, std::string express, std::string path);
		std::vector<TableInfo> GetValues();
		void ReadFile(std::string expr, TableInfo& hi);
		std::string	WriteToFile(std::string expr, int res, size_t m_counter);
	private:
		DBInfo		m_dbInfo;
		MYSQL*		m_conn;
		MYSQL_ROW	m_row;
		MYSQL_RES*  m_res;

	};
}