#pragma once

#include <map>
#include <intsafe.h>
#include <winsock2.h>
#include <windns.h>
#include <ws2tcpip.h>
#include <vector>
#include <comdef.h>
#include <vector>

#pragma comment(lib, "Dnsapi.lib")
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class DnsHelper {
public:
    DnsHelper();
    void RequestA(string& dest, const char* url);
    void RequestNs(string& dest, const char* url);
private:
	map<string, string> cache;
    map<string, time_t> times;
    map<string, string> ns;
    map<string, time_t> timesNs;
    DNS_STATUS Request(const char* url, int type, PDNS_RECORD& pQueryResults);
    void NsCacheInit();
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

    NsCacheInit();
}

void DnsHelper::NsCacheInit() {
    ifstream fNs("ns.txt");
    ifstream fTimes("timesNs.txt");

    string nsValue = string(istreambuf_iterator<char>(fNs), istreambuf_iterator<char>());
    string timesValue = string(istreambuf_iterator<char>(fTimes), istreambuf_iterator<char>());

    fNs.close();
    fTimes.close();

    if (nsValue.empty()) return;

    vector<string> lines;
    size_t start = 0;
    size_t cur = nsValue.find("|", start);

    string nsNewValue = "";
    string timesNewValue = "";

    while (cur < nsValue.size()) {
        lines.push_back(nsValue.substr(start, cur - start));
        start = cur + 1;
        cur = nsValue.find("|", start);
    }

    for (auto iter = lines.begin(); iter != lines.end(); ++iter) {
        unsigned short delim = iter->find("=>");
        string url = iter->substr(0, delim);
        string val = iter->substr(delim + 2);
        ns[url] = val;
    }

    start = 0;
    cur = timesValue.find("|", start);
    while (cur < timesValue.size()) {
        string line = timesValue.substr(start, cur - start);

        unsigned short delim = line.find("=>");
        string url = line.substr(0, delim);
        time_t timeVal = stoi(line.substr(delim + 2));

        if (timeVal < time(0)) {
            ns.erase(ns.find(url));
        }
        else {
            nsNewValue += url + "=>" + ns[url] + "|";
            timesNewValue += line + "|";
        }

        start = cur + 1;
        cur = timesValue.find("|", start);
    }

    system("del ns.txt");
    system("del timesNs.txt");

    ofstream oNs("ns.txt");
    ofstream oTimes("timesNs.txt");

    oNs << nsNewValue;
    oTimes << timesNewValue;

    oNs.close();
    oTimes.close();
}

DNS_STATUS DnsHelper::Request(const char* url, int type, PDNS_RECORD& pQueryResults) {
    DNS_STATUS status;

    size_t length = strlen(url);
    wchar_t text_wchar[30];
    mbstowcs_s(&length, text_wchar, url, length);
    CONST WCHAR* owner = text_wchar;

    status = DnsQuery(
        owner,
        type,
        DNS_QUERY_STANDARD,
        NULL,
        &pQueryResults,
        NULL
    );

    return status;
}

void DnsHelper::RequestA(string& dest, const char* url) {
    map<string, string>::iterator iter = cache.find(url);
    bool inCache = (iter != cache.end());
    if (inCache)
        dest = iter->second;
    else
    {
        PDNS_RECORD pQueryResults;
        auto status = Request(url, DNS_TYPE_A, pQueryResults);

        if (status)
            dest.clear();
        else
        {
            ofstream fCache;
            ofstream fTimes;

            fCache.open("cache.txt", ios_base::app);
            fTimes.open("times.txt", ios_base::app);

            IN_ADDR ipaddr;
            ipaddr.S_un.S_addr = (pQueryResults->Data.A.IpAddress);
            char temp[30];
            inet_ntop(AF_INET, &ipaddr, temp, 30);
            dest = temp;

            string ip;
            ip = dest;

            cache[url] = ip;
            times[url] = time(0);

            fCache << url << "=>" << ip << '|';
            fTimes << url << "=>" << time(0) + pQueryResults->dwTtl << '|';

            fCache.close();
            fTimes.close();
        }
    }
}

void DnsHelper::RequestNs(string& dest, const char* url) {
    auto iter = ns.find(url);
    bool inCache = (iter != ns.end());
    if (inCache) {
        dest = iter->second;
    }
    else {
        PDNS_RECORD pQueryResults;
        auto status = Request(url, DNS_TYPE_NS, pQueryResults);

        if (status)
            dest.clear();
        else {
            wstring wdest = pQueryResults->Data.NS.pNameHost;
            _bstr_t b(wdest.c_str());
            dest = b;

            ofstream fNs, fTimes;
            fNs.open("ns.txt", ios_base::app);
            fTimes.open("timesNs.txt", ios_base::app);

            ns[url] = dest;
            timesNs[url] = time(0);

            fNs << url << "=>" << dest << "|";
            fTimes << url << "=>" << time(0) + pQueryResults->dwTtl << "|";

            fNs.close();
            fTimes.close();
        }
    }
}