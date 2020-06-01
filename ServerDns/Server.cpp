#include "Server.h"

Server::Server(unsigned short port)
{
	mClientAddrSize = sizeof(mClientAddr);

	// WinSock initializtion
	mResult = WSAStartup(MAKEWORD(2, 2), &mWsaData);
	if (mResult != NO_ERROR)
	{
		mResult = FAIL_CODE_INIT_WSA;
		return;
	}

	// Create a socket for sending data
	mServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mServerSocket == INVALID_SOCKET)
	{
		mResult = FAIL_CODE_INIT_SOCKET;
		WSACleanup();
		return;
	}

	// Bind the socket to any address and the specified port.
	mServerAddr.sin_family = AF_INET;
	mServerAddr.sin_port = htons(port);
	mServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	mResult = bind(mServerSocket, (SOCKADDR*)&mServerAddr, sizeof(mServerAddr));
	if (mResult != 0) {
		mResult = FAIL_CODE_BINDING;
		return;
	}
}

void Server::listen()
{
	mResult = recvfrom(
		mServerSocket,
		mRecieveBuffer,
		BUFFER_SIZE,
		0,
		(SOCKADDR*)&mClientAddr,
		&mClientAddrSize
	);

	if (mResult == SOCKET_ERROR)
		mResult = FAIL_CODE_LISTENING;
}

void Server::sendResponse(const char* response)
{
	if (sizeof(response) > BUFFER_SIZE)
	{
		mResult = FAIL_CODE_BUFFER_OVERFLOW;
		return;
	}

	mResult = sendto(
		mServerSocket,
		response,
		BUFFER_SIZE, 0,
		(SOCKADDR*)&mClientAddr,
		mClientAddrSize
	);

	if (mResult == SOCKET_ERROR)
		mResult = FAIL_CODE_SENDING;
}

void Server::cleanUp()
{
	mResult = closesocket(mServerSocket);
	if (mResult == SOCKET_ERROR)
		mResult = FAIL_CODE_CLEANING_SOCKET;
	WSACleanup();
}

int Server::getLastError() const
{
	return mResult;
}

char* Server::getRecieveBuffer() const
{
	char bufferCpy[BUFFER_SIZE];
	strcpy_s(bufferCpy, mRecieveBuffer);
	return bufferCpy;
}

tm* Server::getTime() const
{
	time_t now = time(0);
	tm* timeInfo = new tm();
	localtime_s(timeInfo, &now);
	return timeInfo;
}

tm* Server::getTime(int offset) const
{
	time_t now = time(0);
	now += offset;
	tm* timeInfo = new tm();
	localtime_s(timeInfo, &now);
	return timeInfo;
}