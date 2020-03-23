#include <iostream>
#include "TcpServer.h"
#include <string>
#include "InetAddress.h"

void onConnection(const TcpConnectionPtr &conn)
{
    printf("New Connection Complete\n");
}


int main()
{
	EventLoop loop;
	std::string ip("192.168.209.133");

	InetAddress addr(ip, 8000);

    TcpServer server(&loop, addr, "kkk");
    server.setConnectionCallback(std::bind(&onConnection, _1));
    loop.loop();

    return 0;
}

