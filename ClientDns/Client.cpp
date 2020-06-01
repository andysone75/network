#include "Client.h"

Client::Client(const char* serverIp, unsigned short port)
{
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

	// Set up the RecvAddr structure with the IP address of
	// the server and the specified port number.
	mServerAddr.sin_family = AF_INET;
	mServerAddr.sin_port = htons(port);
	mServerAddr.sin_addr.s_addr = inet_addr(serverIp);

	mServerAddrSize = sizeof(mServerAddr);
}

void Client::sendRequest(const char* request)
{
	if (sizeof(request) > BUFFER_SIZE)
	{
		mResult = FAIL_CODE_BUFFER_OVERFLOW;
		return;
	}
	strcpy_s(mSendBuffer, request);
	mResult = sendto(
		mServerSocket,
		mSendBuffer,
		BUFFER_SIZE, 0,
		(SOCKADDR*)&mServerAddr,
		sizeof(mServerAddr)
	);

	if (mResult == SOCKET_ERROR)
		mResult = FAIL_CODE_SENDING;
}

void Client::getResponse()
{
	mResult = recvfrom(
		mServerSocket,
		mSendBuffer,
		BUFFER_SIZE,
		0,
		(SOCKADDR*)&mServerAddr,
		&mServerAddrSize
	);

	if (mResult == SOCKET_ERROR)
		mResult = FAIL_CODE_LISTENING;
}

void Client::cleanUp()
{
	mResult = closesocket(mServerSocket);
	if (mResult == SOCKET_ERROR)
		mResult = FAIL_CODE_CLEANING_SOCKET;
	WSACleanup();
}

int Client::getLastError() const
{
	return mResult;
}

char* Client::getSendBuffer() const
{
	char bufferCpy[BUFFER_SIZE];
	strcpy_s(bufferCpy, mSendBuffer);
	return bufferCpy;
}