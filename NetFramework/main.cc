#include <iostream>
#include "TcpServer.h"
#include <string>
#include "InetAddress.h"


void onConnection(const TcpConnectionPtr &conn)
{
    printf("New Connection Complete\n");
}

void onMessage(const TcpConnectionPtr &conn, Buffer *pBuffer, int ts)
{
	printf("New Message arrive\n");
	std::string message = pBuffer->retrieveAllAsString();
	printf(":%s\n", message.c_str());
}


int main()
{
	EventLoop loop;
	std::string ip("192.168.209.133");

	InetAddress addr(ip, 8000);

    TcpServer server(&loop, addr, "kkk");
    server.setConnectionCallback(std::bind(&onConnection, _1));
	server.setMessageCallback(std::bind(&onMessage, _1, _2, _3));
    loop.loop();

    return 0;
}

