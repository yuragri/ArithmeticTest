//#include "stdafx.h"
#pragma unmanaged
#include "ArithmeticCore.h"
#pragma managed
#include "ArithmeticWrapper.h"

namespace WrapperCLI
{
	ArithmeticParserWrapper::ArithmeticParserWrapper()
		: ManagedObject(new ArithmeticCore::ArithmeticParser())
	{
		Console::WriteLine("Creating a new ArithmeticParserWrapper object!");
	}

	int ArithmeticParserWrapper::ParseExpression(String ^ Expression)
	{
		Console::WriteLine("The ParseExpression method from the Wrapper was called!");
		return m_Instance->ParseExpression(string_to_char_array(Expression));
	}

	List<HistoryItem^>^ ArithmeticParserWrapper::GetHistory()
	{
		auto history = gcnew List<HistoryItem^>();
		auto historycore = m_Instance->GetHistory();
		for each(auto obj in historycore)
		{
			String^ date = gcnew String(obj.date.c_str());
			String^ expr = gcnew String(obj.expression.c_str());
			String^ res = gcnew String(obj.result.c_str());
			auto hitem = gcnew HistoryItem( date , expr, res );
			history->Add(hitem);
		}
		return history;
	}
}

