#pragma once

#include <map>
#include <intsafe.h>
#include <winsock2.h>
#include <windns.h>
#include <ws2tcpip.h>
#include <vector>

#pragma comment(lib, "Dnsapi.lib")
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class DnsHelper {
public:
    DnsHelper();
	void GetIp(string& dest, const char* url);
private:
	map<string, string> cache;
    map<string, time_t> times;
};

DnsHelper::DnsHelper() {
    fstream fCache;
    fstream fTimes;
    
    fCache.open("cache.txt");
    fTimes.open("times.txt");

    string cacheValue = string(istreambuf_iterator<char>(fCache), istreambuf_iterator<char>());
    string timesValue = string(istreambuf_iterator<char>(fTimes), istreambuf_iterator<char>());

    if (cacheValue.size() > 0) {
        string newCacheValue;
        string newTimesValue;

        for (int i = 0; i < cacheValue.size() - 1;) {
            int start = i == 0 ? i : i + 1;
            i = cacheValue.find('|', start);

            string entry = cacheValue.substr(start, i - start);
            int j = entry.find("=>");
            if (j == -1) continue;
            string url = entry.substr(0, j);
            string ip = entry.substr(j + 2, entry.size());

            cache[url] = ip;
        }

        for (int i = 0; i < timesValue.size() - 1;) {
            int start = i == 0 ? i : i + 1;
            i = timesValue.find('|', start);

            if (i == -1) break;

            string entry = timesValue.substr(start, i - start);
            int j = entry.find("=>");
            string url = entry.substr(0, j);
            int t = stoi(entry.substr(j + 2, entry.size()));

            if (t - time(0) > 0)
            {
                times[url] = t;
                newCacheValue += url + "=>" + cache[url] + '|';
                newTimesValue += url + "=>" + to_string(t) + '|';
            }
            else
            {
                cache.erase(cache.find(url));
            }
        }

        fCache.close();
        fTimes.close();

        system("del cache.txt times.txt");

        ofstream ofCache;
        ofstream ofTimes;

        ofCache.open("cache.txt");
        ofTimes.open("times.txt");

        ofCache << newCacheValue;
        ofTimes << newTimesValue;

        ofCache.close();
        ofTimes.close();
    }
}

void DnsHelper::GetIp(string& dest, const char* url) {
	map<string, string>::iterator iter = cache.find(url);
	bool inCache = (iter != cache.end());
    if (inCache)
        dest = iter->second;
	else
	{
        DNS_STATUS status;
        PDNS_RECORD pQueryResults;
        IN_ADDR ipaddr;
        
        size_t length = strlen(url);
        wchar_t text_wchar[30];
        mbstowcs_s(&length, text_wchar, url, length);
        CONST WCHAR* owner = text_wchar;

        status = DnsQuery(
            owner,
            DNS_TYPE_A,
            DNS_QUERY_STANDARD,
            NULL,
            &pQueryResults,
            NULL
        );

        if (status)
            dest.clear();
        else
        {
            ofstream fCache;
            ofstream fTimes;

            fCache.open("cache.txt", ios_base::app);
            fTimes.open("times.txt", ios_base::app);

            ipaddr.S_un.S_addr = (pQueryResults->Data.A.IpAddress);
            char temp[30];
            inet_ntop(AF_INET, &ipaddr, temp, 30);
            dest = temp;

            string ip;
            ip = dest;

            cache[url] = ip.c_str();
            times[url] = time(0);

            fCache << url << "=>" << ip << '|';
            fTimes << url << "=>" << time(0) + pQueryResults->dwTtl << '|';

            fCache.close();
            fTimes.close();
        }
	}
}