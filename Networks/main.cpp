#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>

using namespace std;

bool isIp(const string& input) {
	if (input.size() > 15) return false;
	if (input.size() < 7)  return false;

	unsigned short dotCount			= 0;
	unsigned short digitsBeforeDot	= 0;
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
bool getIpList(const string& ip, vector<string>& list)
{
	string cmd = "tracert -d -4 -w 60 -h 20 " + ip + " > trace.txt | start /wait \"Tracing\" tracing.cmd";

	ofstream tracing("tracing.cmd");

	tracing << "color 0A";
	tracing << "\ncls";
	tracing << "\ntracert -d -4 -w 60 -h 20 " + ip;
	tracing << "\nexit";

	tracing.close();
	system("attrib +h tracing.cmd");

	system(cmd.c_str());
	system("del /A:h tracing.cmd");

	ifstream ifs("trace.txt");
	char ch;

	// Пропускаем ненужный текст
	ifs.seekg(2, ios::beg);
	while (ifs.get(ch) && ch != '\n');
	while (ifs.get(ch) && ch != '\n');

	// Максимальная длина маршрута 20 узлов
	for (int i = 0; i < 20; ++i)
	{
		string log;
		while (ifs.get(ch) && ch != '\n') log += ch;
		if (log.empty()) break;
		log = log.substr(32, log.length()-1); // Да, если вычесть 2, пробел не уберётся
		log.erase(log.end()-1); // Убираем лишний пробел в конце
		if (isIp(log)) list.push_back(log);
	}

	ifs.close();
	system("del trace.txt");
	if (list[list.size() - 1] != ip) return false;
	return true;
}
void getAsnList(const vector<string>& ips, vector<string>& asns) {
	int i = 0;
	for (string el : ips) {
		i++;

		ifstream ifs;
		string request = "curl https://api.iptoasn.com/v1/as/ip/" + el + " > asn.txt 2> nul";
		system(request.c_str());
		
		ifs.open("asn.txt");
		string answer = string(istream_iterator<char>(ifs), istream_iterator<char>());
		ifs.close();

		string asn;
		// Если первое значение ответа равно false, выводим 'no' и переходим к следующему IP
		if (answer.find("\"announced\":false") != string::npos)
		{
			asns.push_back("");
			cout << ' ' << i << "\tip: " << el;
			if (el.size() < 12) cout.put('\t');
			cout << "\tasn: no" << endl;
			continue;
		}

		// Считываем ASN
		short pos = answer.find("as_number") + 11;
		short count = answer.find(',', pos) - pos;
		asn = answer.substr(
			pos,
			count
		);
		asns.push_back(asn);

		cout << ' ' << i << "\tip: " << el;
		if (el.size() < 12) cout.put('\t');
		cout << "\tasn: " << asn << endl;

		asn = "";
	}
	system("del asn.txt");
}

int main()
{
	// 109.195.104.45 - test
	string ip;
	cout << "IP: ";
	cin >> ip;

	while (!isIp(ip))
	{
		cout << "Invalid IP format" << endl;
		cout << "IP: ";
		cin >> ip;
	}

	cout << endl;

	vector<string> ips;
	vector<string> asns;

	// Tracing
	cout << "Tracing " << ip << " [wait]";
	if (!getIpList(ip, ips))
	{
		cout << '\r';
		cout << "Tracing " << ip << " [ FAILED ] - failed to route\n\n";
		system("pause");
		return 0;
	}
	cout << '\r';
	cout << "Tracing " << ip << " [ OK ]\n\n";
	
	// ASNs finding
	getAsnList(ips, asns);

	cout << endl;
	system("pause");
	return 0;
}