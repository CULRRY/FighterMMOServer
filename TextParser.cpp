#include "stdafx.h"
#include <fstream>
#include <string>
#include "TextParser.h"

#define OUT_OF_RANGE()	__debugbreak()
#define INVALID_VALUE() __debugbreak()

void TextParser::LoadFile(const string& fileName)
{
	ifstream file(fileName);

	string line;

	while (getline(file, line))
	{
		int32 idx = 0;
		string key = "", value = "";
		for (; idx < line.size(); idx++)
		{
			if (line[idx] == '=')
				break;
			if (line[idx] == ',' || line[idx] == '"' || line[idx] == 0x20 ||
				line[idx] == 0x08 || line[idx] == 0x09 || line[idx] == 0x0a ||
				line[idx] == 0x0d)
			{
				continue;
			}
			key += line[idx];
		}
		idx++;

		for (; idx < line.size(); idx++)
		{
			if (line[idx] == '=')
				break;
			if (line[idx] == ',' || line[idx] == '"' || line[idx] == 0x20 ||
				line[idx] == 0x08 || line[idx] == 0x09 || line[idx] == 0x0a ||
				line[idx] == 0x0d)
			{
				continue;
			}
			value += line[idx];
		}

		_data[key] = value;
	}
}

void TextParser::GetValue(const string& key, bool& result)
{
	const string& value = _data[key];
	if (value == "true")
	{
		result = true;
	}
	else if (value == "false")
	{
		result = false;
	}
	else
	{
		INVALID_VALUE();
	}
}

void TextParser::GetValue(const string& key, char& result)
{
	const string& value = _data[key];

	const int32 tmp = ::stoi(value);

	if (tmp > INT8_MAX || tmp < INT8_MIN)
	{
		OUT_OF_RANGE();
	}
	else
	{
		result = static_cast<char>(tmp);
	}


}

void TextParser::GetValue(const string& key, BYTE& result)
{
	const string& value = _data[key];

	const int32 tmp = ::stoi(value);

	if (tmp > UINT8_MAX || tmp < 0)
	{
		OUT_OF_RANGE();
	}
	else
	{
		result = static_cast<BYTE>(tmp);
	}
}

void TextParser::GetValue(const string& key, int16& result)
{
	const string& value = _data[key];

	const int32 tmp = ::stoi(value);

	if (tmp > INT16_MAX || tmp < INT16_MIN)
	{
		OUT_OF_RANGE();
	}
	else
	{
		result = static_cast<int16>(tmp);
	}
}

void TextParser::GetValue(const string& key, uint16& result)
{
	const string& value = _data[key];

	const int32 tmp = ::stoi(value);

	if (tmp > INT16_MAX || tmp < 0)
	{
		OUT_OF_RANGE();
	}
	else
	{
		result = static_cast<uint16>(tmp);
	}
}

void TextParser::GetValue(const string& key, int32& result)
{
	const string& value = _data[key];

	result = ::stoi(value);
}

void TextParser::GetValue(const string& key, uint32& result)
{
	const string& value = _data[key];

	result = ::stoul(value);
}

void TextParser::GetValue(const string& key, int64& result)
{
	const string& value = _data[key];

	result = ::stoll(value);
}

void TextParser::GetValue(const string& key, uint64& result)
{
	const string& value = _data[key];

	result = ::stoull(value);
}

void TextParser::GetValue(const string& key, float& result)
{
	const string& value = _data[key];

	result = ::stof(value);
}

void TextParser::GetValue(const string& key, double& result)
{
	const string& value = _data[key];

	result = ::stod(value);
}

void TextParser::GetValue(const string& key, string& result)
{
	result = _data[key];
}
