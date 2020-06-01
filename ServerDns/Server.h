#pragma once

#define FAIL_CODE_INIT_WSA			-1
#define FAIL_CODE_INIT_SOCKET		-2
#define FAIL_CODE_SENDING			-3
#define FAIL_CODE_CLEANING_SOCKET	-4
#define FAIL_CODE_BINDING			-5
#define FAIL_CODE_LISTENING			-6
#define FAIL_CODE_BUFFER_OVERFLOW	-7

#define BUFFER_SIZE 1024

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

class Server
{
public:
	Server(unsigned short port);
	void listen();
	void sendResponse(const char* response);
	void cleanUp();

	int getLastError() const;
	char* getRecieveBuffer() const;
	tm* getTime() const;
	tm* getTime(int offset) const;
private:
	int mResult;
	char mRecieveBuffer[BUFFER_SIZE];
	WSADATA mWsaData;
	SOCKET mServerSocket;
	sockaddr_in mServerAddr;
	sockaddr_in mClientAddr;
	int mClientAddrSize;
};