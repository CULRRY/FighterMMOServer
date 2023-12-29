#pragma once
class NetworkAddress
{
public:
	static IN_ADDR	Ip2Address(const wstring_view ip);

public:
	NetworkAddress() = default;
	NetworkAddress(SOCKADDR_IN sockAddr);
	NetworkAddress(const wstring_view ip, uint16 port);
	NetworkAddress(uint32 ipType, uint16 port);


	wstring			GetIpAddress();
	uint16			GetPort() { return ::ntohs(_sockAddr.sin_port); }
	SOCKADDR_IN&	GetSockAddr() { return _sockAddr; }


private:
	SOCKADDR_IN _sockAddr;
};

