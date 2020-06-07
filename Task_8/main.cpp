#include <iostream>
#include <algorithm>
#include <map>
#include "VkApp.h"

using namespace std;

int main()
{
    VkApp app;
    string comand;

    cout << "\tVK API\n";
    app.ShowHelp(State::UserSelecting);
    cout << "\nUser Selecting\n";

    while (app.GetState() != State::Exit) {
        cout << "\n>";
        cin >> comand;
        transform(comand.begin(), comand.end(), comand.begin(), ::tolower);

        if (comand == "exit") {
            app.GoTo(State::Exit);
            continue;
        }
        else if (comand == "help") {
            app.ShowHelp();
            continue;
        }

        if (app.GetState() == State::UserSelecting) {
            User* user = app.GetUserById(comand);
            if (user != nullptr) {
                app.SetCurrentUser(user);
                app.GoTo(State::UserMenu);
            }
        }
        else if (app.GetState() == State::UserMenu) {
            if (comand == "back") {
                app.Back();
                continue;
            }
            else if (comand == "friends") {
                app.ShowFriends(app.GetCurrentUser());
                if (!app.GetCurrentUser()->Friends.empty())
                    app.GoTo(State::FriendsList);
            }
            else {
                app.ShowHelp(State::UserMenu);
            }
        }
        else if (app.GetState() == State::FriendsList) {
            if (comand == "back") {
                app.GoTo(State::UserMenu);
            }
            else {
                int number;
                try {
                    number = stoi(comand);
                }
                catch (invalid_argument ia) {
                    app.ShowHelp();
                    continue;
                }

                if (number < 0 || number > app.GetCurrentUser()->Friends.size()) {
                    cout << "Input must be in friends list range\n";
                    continue;
                }
                else {
                    User* user = &app.GetCurrentUser()->Friends[number - 1];
                    app.SetCurrentUser(user);
                    app.SaveUser(user);
                    app.GoTo(State::UserMenu);
                }
            }
        }
    }
}