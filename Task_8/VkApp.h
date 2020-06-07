#pragma once

#include <map>
#include <string>
#include <vector>
#include <stack>

enum class State { UserSelecting, UserMenu, FriendsList, Exit };

using namespace std;

struct User {
	string Name;
	string Id;
	vector<User> Friends;
};

class VkApp {
public:
	VkApp();
	void ShowHelp() const;
	void ShowHelp(State state) const;
	State GetState() const;
	void GoTo(State newState);
	void ShowFriends(User* user);
	User* GetCurrentUser() const;
	void Back();
	void SaveUser(User* pUser);

	User* GetUserById(const string& id);
	void GetUsersFriendsList(User* user) const;
	void SetCurrentUser(User* pUser);

private:
	string Request(const string& request) const;

private:
	State mState;
	map<int, string> mHelps;
	string mToken;
	User* mCurrentUser;
	stack<User*> mSavedUsers;
};