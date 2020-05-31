#include "Server.h"
#include <iostream>
#include <string>
#include <fstream>

#include <iostream>
#include <winsock2.h>

#include "DnsHelper.h"

using namespace std;

int main()
{
	// Initialization
	Server server(123);
	if (server.getLastError() == FAIL_CODE_INIT_WSA)
	{
		cout << "WinSock initialization failed\n";
		system("pause");
		return -1;
	}
	else if (server.getLastError() == FAIL_CODE_INIT_SOCKET)
	{
		cout << "Server Socket creation failed with error: " << WSAGetLastError() << endl;
		system("pause");
		return -1;
	}
	else if (server.getLastError() == FAIL_CODE_BINDING)
	{
		cout << "Binding failed with error: " << WSAGetLastError() << endl;
		system("pause");
		return -1;
	}

	

	cout << "Listening datagrams...\n";
	while (true)
	{
		server.listen();
		if (server.getLastError() == FAIL_CODE_LISTENING)
		{
			cout << "Listening failed with error: " << WSAGetLastError() << endl;
			system("pause");
			return -1;
		}

		cout << "Client: " << server.getRecieveBuffer() << endl;
		if (strcmp(server.getRecieveBuffer(), "kill") == 0) {
			server.sendResponse("server killed. reboot server before input new command");
			if (server.getLastError() == FAIL_CODE_BUFFER_OVERFLOW)
			{
				cout << "The response size is larger than the allowed buffer size.\n";
				return -1;
			}
			else if (server.getLastError() == FAIL_CODE_SENDING)
			{
				cout << "Sending response failed with error: "
					<< WSAGetLastError() << endl;
				system("pause");
				return -1;
			}
			break;
		}

		ifstream ifs;
		ifs.open("config.txt");
		string configValue = string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
		ifs.close();

		string str = "Hello, World!!!";

		server.sendResponse(str.c_str());
		if (server.getLastError() == FAIL_CODE_BUFFER_OVERFLOW)
		{
			cout << "The response size is larger than the allowed buffer size.\n";
			return -1;
		}
		else if (server.getLastError() == FAIL_CODE_SENDING)
		{
			cout << "Sending response failed with error: "
				<< WSAGetLastError() << endl;
			system("pause");
			return -1;
		}
	}

	server.cleanUp();
	if (server.getLastError() == FAIL_CODE_CLEANING_SOCKET)
	{
		cout << "socketclose failed with error: " << WSAGetLastError() << endl;
		system("pause");
		return -1;
	}

	system("pause");
	return 0;
}

void GetIpByUrl() {
	
}