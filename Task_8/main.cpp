#include <iostream>
#include "rapidjson/include/rapidjson/rapidjson.h"
#include "rapidjson/include/rapidjson/document.h"
#include <fstream>
#include <map>
#include "User.h"
#include <algorithm>

using namespace std;
using namespace rapidjson;

int main()
{
    enum class State { UserSelecting, UserMenu, Exit };
    enum class Comands { Folowers };

    map<string, Comands> comands;
    comands["folowers"] = Comands::Folowers;

    map<int, string> helps;
    helps[static_cast<int>(State::UserSelecting)] = "\nexit : Exit the program\nhelp : Show help\n[id] : Enter user id\n";
    helps[static_cast<int>(State::UserMenu)] = "\nexit : Exit the program\nhelp : Show help\nback : Back to the User Selecting\n";

    string id = "";
    State state = State::UserSelecting;
    string token;
    string request;
    string response;
    ifstream ifresponse;
    Document d;
    Value val;
    string userName;
    string comand;
    User user;

    ifstream* iftoken = new ifstream("token.txt");
    token =  string(istreambuf_iterator<char>(*iftoken), istreambuf_iterator<char>());
    iftoken->close();
    delete iftoken;


    cout << "\tVK API\n";

    cout << helps[static_cast<int>(State::UserSelecting)];

    while (state != State::Exit) {
        switch (state)
        {
        case State::UserSelecting:
            cout << "\nUser Selecting\n";
            break;
        case State::UserMenu:
            cout << "\nUser Menu: " << user.getName() << endl;
            break;
        }

        cout << '>';
        cin >> comand;
        transform(comand.begin(), comand.end(), comand.begin(), ::tolower);

        if (comand == "exit") {
            state = State::Exit;
            continue;
        }
        else if (comand == "help") {
            cout << helps[static_cast<int>(state)];
            continue;
        }

        if (state == State::UserSelecting) {
            string request = "curl \"https://api.vk.com/method/users.get?user_ids=" + comand + "&fields=bdate&access_token=" + token + "&v=5.107\" > response.txt 2> null";
            system(request.c_str());

            ifstream ifresponse = ifstream("response.txt");
            string response = string(istreambuf_iterator<char>(ifresponse), istreambuf_iterator<char>());
            ifresponse.close();

            Document d;
            d.Parse(response.c_str());
            if (d.HasMember("error")) {
                cout << "\nID doesn't exist" << endl;
                continue;
            }
            else {
                Value val = d["response"].GetArray()[0].GetObject();
                string userName = val["first_name"].GetString();
                userName += ' ';
                userName += val["last_name"].GetString();
                int id = val["id"].GetInt();

                user = User(userName, to_string(id));
                state = State::UserMenu;
            }
        } else if (state == State::UserMenu) {
            if (comand == "back") {
                state = State::UserSelecting;
                continue;
            }
            else {
                cout << helps[static_cast<char>(State::UserMenu)];
            }
        }
    }
}