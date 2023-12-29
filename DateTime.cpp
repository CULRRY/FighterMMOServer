#include "stdafx.h"
#include "DateTime.h"

DateTime::DateTime()
{
	const time_t timer = ::time(nullptr);
	errno_t err = ::localtime_s(&_time, &timer);
}

wstring DateTime::to_string(wstring_view fmt)
{
	WCHAR str[100];

	size_t size = wcsftime(str, 100, fmt.data(), &_time);

	wstring ret(str);
	return str;
}
