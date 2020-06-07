#include "Server.h"
#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include "DnsHelper.h"

using namespace std;

int main()
{
	map<string, int> types;
	types["/a"] = DNS_TYPE_A;
	types["/ns"] = DNS_TYPE_NS;

	// Initialization
	Server server(53);
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

	DnsHelper dns;

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

		string client = server.getRecieveBuffer();

		cout << "Client: " << client << endl;
		if (strcmp(client.c_str(), "kill") == 0) {
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

		USHORT delim = client.find('|');
		string type = client.substr(0, delim);
		string url = client.substr(delim+1);

		string data;
		auto t = types[type];
		if (type == "/ns") dns.RequestNs(data, url.c_str());
		else dns.RequestA(data, url.c_str());

		server.sendResponse(data.c_str());
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