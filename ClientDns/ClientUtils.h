#pragma once
#include "Client.h"

#define argIs(x) strcmp(argv[1], x) == 0

using namespace std;

const char* types[] = { 
	"/a", 
	"/ns"
};

int isTypeValid(const char* type) {
	unsigned char tCount = sizeof(types) / sizeof(*types);
	for (unsigned char i = 0; i < tCount; ++i) {
		if (strcmp(types[i], type) == 0)
			return true;
	}
	return false;
}

bool clientInitCheck(int lastErrorCode) {
	if (lastErrorCode == FAIL_CODE_INIT_WSA)
	{
		cout << "WinSock initialization failed\n";
		return false;
	}
	else if (lastErrorCode == FAIL_CODE_INIT_SOCKET)
	{
		cout << "Server Socket creation failed with error: " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

bool clientSendCheck(int lastErrorCode) {
	if (lastErrorCode == FAIL_CODE_BUFFER_OVERFLOW)
	{
		cout << "The response size is larger than the allowed buffer size.\n";
		return false;
	}
	else if (lastErrorCode == FAIL_CODE_SENDING)
	{
		cout << "\rSending request to server [ FAIL ]\n";
		cout << "Error: " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

bool clientResponseCheck(int lastErrorCode) {
	if (lastErrorCode == FAIL_CODE_LISTENING)
	{
		cout << "Getting response failed with error: " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}