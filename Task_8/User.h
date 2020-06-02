#pragma once

#include <string>

class User {
public:
	User();
	User(std::string name, std::string id);

	std::string getName() const;
	std::string getId() const;
private:
	std::string mName;
	std::string mId;
};

User::User() :
	mName("noname"), mId("noID")
{}

User::User(std::string name, std::string id) :
	mName(name), mId(id)
{}

std::string User::getName() const {
	return mName;
}

std::string User::getId() const {
	return mId;
}