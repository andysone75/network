#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define FAIL_CODE_INIT_WSA			-1
#define FAIL_CODE_INIT_SOCKET		-2
#define FAIL_CODE_SENDING			-3
#define FAIL_CODE_CLEANING_SOCKET	-4
#define FAIL_CODE_LISTENING			-5
#define FAIL_CODE_BUFFER_OVERFLOW	-6

#define BUFFER_SIZE 1024

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
	Client(const char* serverIp, unsigned short port);
	void sendRequest(const char* request);
	void getResponse();
	void cleanUp();

	int getLastError() const;
	char* getSendBuffer() const;
public:
	int mResult;
	char mSendBuffer[BUFFER_SIZE];
	WSADATA mWsaData;
	SOCKET mServerSocket;
	sockaddr_in mServerAddr;
	int mServerAddrSize;
};