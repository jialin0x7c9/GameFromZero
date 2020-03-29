#include <iostream>
#include "TcpServer.h"
#include <string>
#include "InetAddress.h"
#include <memory>


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

void threadInit(EventLoop *loop)
{
    printf("newThread\n");
}

int main()
{
	EventLoop loop;
	std::string ip("192.168.209.134");
	InetAddress addr(ip, 8000);
    TcpServer server(&loop, addr, "kkk");
    server.setThreadInitCallback(std::bind(&threadInit, _1));
    server.setThreadNum(3);
    server.start();
    loop.loop();
    return 0;
}

