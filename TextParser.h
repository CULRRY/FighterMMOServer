#pragma once
#include <unordered_map>

class TextParser
{
	enum : char
	{
		COMMA = ',',
		DOT = ',',
		SPACE = 0x20,
		BACKSPACE = 0x08,
		TAP = 0x09,
		LINEFEED = 0x0a,
		CARRAGE_RETURN = 0x0d,
	};

public:
	TextParser(const string& fileName)
	{
		LoadFile(fileName);
	}


	void LoadFile(const string& fileName);

	void GetValue(const string& key, bool& result);
	void GetValue(const string& key, char& result);
	void GetValue(const string& key, BYTE& result);
	void GetValue(const string& key, int16& result);
	void GetValue(const string& key, uint16& result);
	void GetValue(const string& key, int32& result);
	void GetValue(const string& key, uint32& result);
	void GetValue(const string& key, int64& result);
	void GetValue(const string& key, uint64& result);
	void GetValue(const string& key, float& result);
	void GetValue(const string& key, double& result);
	void GetValue(const string& key, string& result);

private:
	using DataMap = std::unordered_map<string, string>;

	DataMap _data;
};

