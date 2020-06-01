#include "Client.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

bool isIp(const string& input);

int main()
{
	string fingerprint;
	cout << "Server on this computer? (yes/no): ";
	getline(cin, fingerprint);
	while (fingerprint != "yes" && fingerprint != "no")
	{
		cout << "(yes/no): ";
		getline(cin, fingerprint);
	}

	string ip;
	if (fingerprint == "yes")
	{
		system("ip.cmd > ips.txt");
		ifstream ipstream("ips.txt");
		char ch;
		ip;
		while (ipstream.get(ch) && ch != '\n') ip += ch;
		ipstream.close();
		if (!isIp(ip)) fingerprint = "no";
		else cout << "Server IP: " << ip << endl;
	}

	if (fingerprint == "no")
	{
		while (!isIp(ip))
		{
			cout << "Server IP: ";
			getline(cin, ip);
		}
	}

	Client client(ip.c_str(), 123);
	if (client.getLastError() == FAIL_CODE_INIT_WSA)
	{
		cout << "WinSock initialization failed\n";
		system("pause");
		return -1;
	}
	else if (client.getLastError() == FAIL_CODE_INIT_SOCKET)
	{
		cout << "Server Socket creation failed with error: " << WSAGetLastError() << endl;
		system("pause");
		return -1;
	}

	cout << "[kill] - turn off the server\n";
	cout << "[shutdown] - exit from the client\n";
	cout << "[any] - get server time\n";
	string input;
	while (true)
	{
		cout << '>';
		getline(cin, input);

		if (input.empty()) continue;
		if (input == "shutdown") break;

		client.sendRequest(input.c_str());
		if (client.getLastError() == FAIL_CODE_BUFFER_OVERFLOW)
		{
			cout << "The response size is larger than the allowed buffer size.\n";
			return -1;
		}
		else if (client.getLastError() == FAIL_CODE_SENDING)
		{
			cout << "\rSending request of time to server [ FAIL ]\n";
			cout << "Error: " << WSAGetLastError() << endl;
			system("pause");
			return -1;
		}

		client.getResponse();
		if (client.getLastError() == FAIL_CODE_LISTENING)
		{
			cout << "Getting response failed with error: " << WSAGetLastError() << endl;
			system("pause");
			return -1;
		}

		cout << "Server: " << client.getSendBuffer() << endl;
	}

	client.cleanUp();
	if (client.getLastError() == FAIL_CODE_CLEANING_SOCKET)
	{
		cout << "socketclose failed with error: " << WSAGetLastError() << endl;
		system("pause");
		return -1;
	}

	system("pause");
	return 0;
}

bool isIp(const string& input) {
	if (input.size() > 15) return false;
	if (input.size() < 7)  return false;

	unsigned short dotCount = 0;
	unsigned short digitsBeforeDot = 0;
	string byte;

	for (char s : input) {
		if (s == '.')
		{
			if (++dotCount > 3) return false;
			if (digitsBeforeDot > 3 || digitsBeforeDot == 0)
				return false;
			else
			{
				byte = "";
				digitsBeforeDot = 0;
			}
		}
		else if (s - '0' < 10)
		{
			byte += s;
			if (stoi(byte) > 255) return false;
			digitsBeforeDot++;
		}
		else return false;
	}

	if (dotCount != 3) return false;

	return true;
}