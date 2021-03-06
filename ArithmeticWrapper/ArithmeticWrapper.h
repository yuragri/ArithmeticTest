#pragma once
#include "ManagedObject.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

namespace WrapperCLI
{
	public ref class HistoryItem
	{
	public:
		HistoryItem(String^ d, String^ e, String^ r) :
			Date(d), Expression(e), Result(r) {}
		String^ Date;
		String^ Expression;
		String^ Result;
	};
	public ref class ArithmeticParserWrapper : public ManagedObject<ArithmeticCore::ArithmeticParser>
	{
	public:
		ArithmeticParserWrapper();
		int ParseExpression(String^ string);
		List<HistoryItem^>^ GetHistory();
	};
}
