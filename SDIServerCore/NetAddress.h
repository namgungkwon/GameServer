#pragma once


///////////////////////////////////////////////////////
//	<	NetAddress	>
///////////////////////////////////////////////////////
class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr) {};
	NetAddress(wstring ip, uint16 port);

public:
	/* Get & Set */
	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	String GetIpAddress();
	uint16 GetPort() { return ::ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR Ip2Address(const WCHAR* ip);


private:
	SOCKADDR_IN _sockAddr = {};
};

