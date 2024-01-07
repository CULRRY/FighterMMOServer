#include "stdafx.h"

#include "Server.h"

int main()
{
	timeBeginPeriod(1);
	Server::Init();
	Server::Run();
}
