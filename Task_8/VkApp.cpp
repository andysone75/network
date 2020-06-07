#include "VkApp.h"

#include <iostream>
#include <fstream>
#include <string>

#include "rapidjson/include/rapidjson/rapidjson.h"
#include "rapidjson/include/rapidjson/document.h"

using namespace std;
using namespace rapidjson;

VkApp::VkApp() {
	mState = State::UserSelecting;

	mHelps[static_cast<int>(State::UserSelecting)] = "\nexit : Exit the program\nhelp : Show help\n[id] : Enter user id\n";
	mHelps[static_cast<int>(State::UserMenu)] = "\nexit : Exit the program\nhelp : Show help\nfriends : Get friends list of the user\nback : Back to the previous user\n";
	mHelps[static_cast<int>(State::FriendsList)] = "\nexit : Exit the program\nhelp : Show help\n[number] : Select friend by order number in the list\nback : Back to the User Menu\n";

	ifstream iftoken = ifstream("token.txt");
	mToken = string(istreambuf_iterator<char>(iftoken), istreambuf_iterator<char>());
	iftoken.close();
}

void VkApp::ShowHelp() const {
	if (mState == State::Exit) {
		cout << "There is no help for Exit state\n";
		return;
	}

	cout << mHelps.at(static_cast<int>(mState));
}

void VkApp::ShowHelp(State state) const {
	if (state == State::Exit) {
		cout << "There is no help for Exit state\n";
		return;
	}

	cout << mHelps.at(static_cast<int>(state));
}

State VkApp::GetState() const {
	return mState;
}

void VkApp::GoTo(State newState) {
	switch (newState)
	{
	case State::UserSelecting:
		cout << "\nUser Selecting\n";
		break;
	case State::UserMenu:
		cout << "\nUser Menu: " << mCurrentUser->Name << endl;
		break;
	case State::Exit:
		cout << "\nGood Bye!\n";
		break;
	}

	mState = newState;
}

void VkApp::ShowFriends(User* user) {
	if (user->Friends.empty())
		GetUsersFriendsList(user);

	int size = user->Friends.size();
	for (int i = 0; i < size; ++i) {
		User frnd = user->Friends[i];
		cout << to_string(i + 1) << ". " << frnd.Name << endl;
	}
}

User* VkApp::GetCurrentUser() const {
	return mCurrentUser;
}

void VkApp::Back() {
	if (mSavedUsers.size() == 1) {
		mSavedUsers.pop();
		GoTo(State::UserSelecting);
		return;
	}
	else {
		mSavedUsers.pop();
		SetCurrentUser(mSavedUsers.top());
		GoTo(State::UserMenu);
	}
}

void VkApp::SaveUser(User* pUser) {
	mSavedUsers.push(pUser);
}

string VkApp::Request(const string& request) const {
	string curl = "curl \"";
	curl += request;
	curl += "\" > response.txt 2> null";
	system(curl.c_str());

	ifstream ifresponse = ifstream("response.txt");
	string response = string(istreambuf_iterator<char>(ifresponse), istreambuf_iterator<char>());
	ifresponse.close();
	system("del response.txt");

	return response;
}

User* VkApp::GetUserById(const string& id) {
	string request = "https://api.vk.com/method/users.get?user_ids=" + id + "&fields=bdate&access_token=" + mToken + "&v=5.107";
	string response = Request(request);

	User* user;
	Document d;
	d.Parse(response.c_str());
	if (d.HasMember("error")) {
		cout << "User doesn't exist" << endl;
		return nullptr;
	}
	else {
		Value val = d["response"].GetArray()[0].GetObject();
		string userName = val["first_name"].GetString();
		userName += ' ';
		userName += val["last_name"].GetString();
		int id = val["id"].GetInt();

		user = new User({ userName, to_string(id) });
		SaveUser(user);

		return user;
	}
}

void VkApp::GetUsersFriendsList(User* user) const {
	string request = "";
	request = "https://api.vk.com/method/friends.get?";
	request += "user_id=" + user->Id;
	request += "&order=name";
	request += "&fields=nickname";
	request += "&access_token=" + mToken;
	request += "&v=5.107";

	string response = Request(request);
	Document d;
	d.Parse(response.c_str());
	if (!d.HasMember("error")) {
		int count = d["response"].GetObject()["count"].GetInt();
		for (int i = 0; i < count; ++i) {
			Value frnd = d["response"].GetObject()["items"].GetArray()[i].GetObject();

			string name = frnd["first_name"].GetString();
			name += ' ';
			name += frnd["last_name"].GetString();

			int id = frnd["id"].GetInt();
			user->Friends.push_back({ name, to_string(id) });
		}
	}
}

void VkApp::SetCurrentUser(User* pUser) {
	mCurrentUser = pUser;
}