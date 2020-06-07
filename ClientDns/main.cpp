#include <iostream>
#include <string.h>
#include <string>
#include <fstream>
#include "Client.h"
#include "ClientUtils.h"

using namespace std;

bool isIp(const string& input);
void help();
string serverIp();

// A
// AAAA
// CNAME
// MX
// NS
// PTR
// SOA
// SRV

int main(int argc, char* argv[]) {
	if (argc != 3) { help(); return 0; }
	if (!isTypeValid(argv[1])) { help(); return 0; }

	// Type specified and valid [ OK ]

	string serv = serverIp();
	Client client(serv.c_str(), 53);
	if (!clientInitCheck(client.getLastError())) return -1;

	unsigned char len = strlen(argv[1]) + strlen(argv[2]) + 2;
	char* input = new char[len];
	memset(input, 0, len);
	strcat_s(input, len, argv[1]);
	strcat_s(input, len, "|");
	strcat_s(input, len, argv[2]);

	client.sendRequest(input);
	if (!clientSendCheck(client.getLastError())) return -1;

	client.getResponse();
	if (!clientResponseCheck(client.getLastError())) return -1;

	string server = client.getSendBuffer();
	cout << "Server: " << server << endl;
}

void help() {
	cout << "/a : A" << endl;
	cout << "/ns : NS" << endl;
}

string serverIp() {
	string fingerprint;
	string ip;
	system("ip.cmd > ips.txt");
	ifstream ipstream("ips.txt");
	char ch;
	while (ipstream.get(ch) && ch != '\n') ip += ch;
	ipstream.close();
	if (!isIp(ip)) fingerprint = "no";
	else cout << "Server IP: " << ip << endl;

	if (fingerprint == "no")
	{
		while (!isIp(ip))
		{
			cout << "Server IP: ";
			getline(cin, ip);
		}
	}

	return ip;
}

//int main()
//{
//	string fingerprint;
//	cout << "Server on this computer? (yes/no): ";
//	getline(cin, fingerprint);
//	while (fingerprint != "yes" && fingerprint != "no")
//	{
//		cout << "(yes/no): ";
//		getline(cin, fingerprint);
//	}
//
//	string ip;
//	if (fingerprint == "yes")
//	{
//		system("ip.cmd > ips.txt");
//		ifstream ipstream("ips.txt");
//		char ch;
//		ip;
//		while (ipstream.get(ch) && ch != '\n') ip += ch;
//		ipstream.close();
//		if (!isIp(ip)) fingerprint = "no";
//		else cout << "Server IP: " << ip << endl;
//	}
//
//	if (fingerprint == "no")
//	{
//		while (!isIp(ip))
//		{
//			cout << "Server IP: ";
//			getline(cin, ip);
//		}
//	}
//
//	Client client(ip.c_str(), 53);
//	if (client.getLastError() == FAIL_CODE_INIT_WSA)
//	{
//		cout << "WinSock initialization failed\n";
//		system("pause");
//		return -1;
//	}
//	else if (client.getLastError() == FAIL_CODE_INIT_SOCKET)
//	{
//		cout << "Server Socket creation failed with error: " << WSAGetLastError() << endl;
//		system("pause");
//		return -1;
//	}
//
//	cout << "[kill] - turn off the server\n";
//	cout << "[shutdown] - exit from the client\n";
//	cout << "[any] - get server time\n";
//	string input;
//	while (true)
//	{
//		cout << '>';
//		getline(cin, input);
//
//		if (input.empty()) continue;
//		if (input == "shutdown") break;
//
//		client.sendRequest(input.c_str());
//		if (client.getLastError() == FAIL_CODE_BUFFER_OVERFLOW)
//		{
//			cout << "The response size is larger than the allowed buffer size.\n";
//			return -1;
//		}
//		else if (client.getLastError() == FAIL_CODE_SENDING)
//		{
//			cout << "\rSending request of time to server [ FAIL ]\n";
//			cout << "Error: " << WSAGetLastError() << endl;
//			system("pause");
//			return -1;
//		}
//
//		client.getResponse();
//		if (client.getLastError() == FAIL_CODE_LISTENING)
//		{
//			cout << "Getting response failed with error: " << WSAGetLastError() << endl;
//			system("pause");
//			return -1;
//		}
//
//		string server = client.getSendBuffer();
//		cout << "Server: " << server << endl;
//	}
//
//	client.cleanUp();
//	if (client.getLastError() == FAIL_CODE_CLEANING_SOCKET)
//	{
//		cout << "socketclose failed with error: " << WSAGetLastError() << endl;
//		system("pause");
//		return -1;
//	}
//
//	system("pause");
//	return 0;
//}

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