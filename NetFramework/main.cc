#include <iostream>
#include "TcpServer.h"
#include <string>
#include "InetAddress.h"

int main()
{
	EventLoop loop;
	std::string ip("192.168.209.131");

	InetAddress addr(ip, 8000);

    TcpServer server(&loop, addr, "kkk");
    loop.loop();

    return 0;
}

