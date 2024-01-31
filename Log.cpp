#include "stdafx.h"
#include "Log.h"
#include <fstream>
#include "DateTime.h"

void Log::Print(wstring_view str)
{
	DateTime time;

	wcout << "[" << time.to_string() << "]" << str << "\n";
	wofstream fout("Log.txt", ios::app);

	if (fout.is_open())
	{
		fout << "[" << time.to_string() << "]" << str << "\n";
	}
}
