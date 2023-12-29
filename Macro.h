#pragma once

#define CRASH(str) __debugbreak();

#define CRASH_EXPR(expr) if ((expr) == false) __debugbreak();