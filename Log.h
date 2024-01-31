#pragma once
#include <format>
#define _PRINT_LOG(level, fmt, ...) \
	do\
	{\
		if (Log::logLevel <= level)\
		{\
			wstring str = ::format(fmt, ##__VA_ARGS__);\
			Log::Print(str);\
		}\
	}\
	while (false)

#define LOG_DEBUG(fmt, ...)		_PRINT_LOG(LogLevel::LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_SYSTEM(fmt, ...)	_PRINT_LOG(LogLevel::LEVEL_SYSTEM, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)		_PRINT_LOG(LogLevel::LEVEL_ERROR, fmt, ##__VA_ARGS__)


enum class LogLevel
{
	LEVEL_DEBUG,
	LEVEL_SYSTEM,
	LEVEL_ERROR,
	LEVEL_OFF,
};

class Log
{
	using enum LogLevel;
public:
	static void Print(wstring_view str);

	inline static LogLevel logLevel = LEVEL_DEBUG;
};