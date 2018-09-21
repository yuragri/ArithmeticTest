#include "DbManager.h"
#include <iostream>
#include <fstream>

namespace ArithmeticCore
{
	DbManager::DbManager()
	{
		//db info
		m_dbInfo = { "localhost", "root", "admin", "expressiondb", "expressions", 3310 };
		m_conn = mysql_init(0);
		m_conn = mysql_real_connect(m_conn, m_dbInfo.host, m_dbInfo.user, m_dbInfo.passwd, m_dbInfo.dbname, m_dbInfo.port, NULL, 0);
		if (m_conn) {
			std::cout << "Successful connection to database!" << std::endl;

		}
		else {
			std::cout << "Connection to database has failed!" << std::endl;
		}
	}

	DbManager::~DbManager()
	{
		mysql_close(m_conn);
		std::cout << "Successfuly disconnected from database!" << std::endl;
	}
	int DbManager::GetItemsCount()
	{
		std::string query = "SELECT * FROM " + std::string(m_dbInfo.tablename);
		const char* q = query.c_str();
		int qstate = mysql_query(m_conn, q);
		if (!qstate)
		{
			m_res = mysql_store_result(m_conn);
			auto rows = mysql_num_rows(m_res);
			mysql_free_result(m_res);
			return rows;
		}
		else
		{
			std::cout << "Query failed: " << mysql_error(m_conn) << std::endl;
		}
		return 0;
	}
	void DbManager::Insert(std::string id, std::string express, std::string path)
	{
		std::string query = "INSERT INTO " + std::string(m_dbInfo.tablename) + " (id, date, expression, path) VALUES (" + id + ", NOW(), \"" + express.substr(0, 10) + "\", \"" + path + "\")";
		const char* q = query.c_str();
		int qstate = mysql_query(m_conn, q);
		if (!qstate)
		{
			std::cout << "Item added: " << std::endl;
		}
		else
		{
			std::cout << "Query failed: " << mysql_error(m_conn) << std::endl;
		}
	}
	std::vector<TableInfo> DbManager::GetValues()
	{
		std::vector<TableInfo> values;
		std::string query = "SELECT * FROM " + std::string(m_dbInfo.tablename);
		const char* q = query.c_str();
		int qstate = mysql_query(m_conn, q);
		if (!qstate)
		{
			m_res = mysql_store_result(m_conn);
			while (m_row = mysql_fetch_row(m_res))
			{
				TableInfo ti{ m_row[0], m_row[1] ,"","" };
				std::string path = m_row[3];
				ReadFile(path, ti);
				values.push_back(ti);
				/*std::cout << "ID: " << row[0]
					<< ", Date: " << row[1]
					<< ", Expression: " << row[2]
					<< ", Path: " << row[3] << std::endl;*/
			}
			mysql_free_result(m_res);
		}
		else
		{
			std::cout << "Query failed: " << mysql_error(m_conn) << std::endl;
		}
		return values;
	}

	std::string DbManager::WriteToFile(std::string expr, int res, size_t counter)
	{
		std::ofstream fout;
		fout.exceptions(std::ofstream::badbit | std::ofstream::failbit);
		try
		{
			std::string path = "expression" + std::to_string(counter) + ".txt";
			fout.open(path);
			fout << expr << "\n";
			fout << res;
			fout.close();
			return path;
		}
		catch (const std::ofstream::failure & ex)
		{
			std::cout << ex.what() << std::endl;
			std::cout << ex.code() << std::endl;
			return "";
		}
	}

	void DbManager::ReadFile(std::string path, TableInfo& hi)
	{
		std::ifstream fin;
		fin.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		std::string exp, res;
		try
		{
			fin.open(path);
			std::getline(fin, exp);
			std::getline(fin, res);
			hi.expression = exp;
			hi.result = res;
			fin.close();
		}
		catch (const std::ifstream::failure & ex)
		{
			std::cout << ex.what() << std::endl;
			std::cout << ex.code() << std::endl;
		}
	}
}