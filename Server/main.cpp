#include "Server.h"
#include <iostream>
#include <string>
#include <fstream>

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

		const char* request = server.getRecieveBuffer();
		cout << "Client: " << request << endl;
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

		int offset = 0;
		try
		{
			offset = stoi(configValue);
		}
		catch (exception& e)
		{
			if (!configValue.empty())
				cout << "Config file error: " << e.what() << endl;
		}

		tm* time = server.getTime(offset);
		string strHour	= to_string(time->tm_hour);
		string strMin	= to_string(time->tm_min);
		string str = (strHour.length() == 2 ? strHour : '0' + strHour) + ':' + (strMin.length() == 2 ? strMin : '0' + strMin);
		delete time;

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