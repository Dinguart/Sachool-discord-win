#pragma once
#include <dpp/dpp.h>

#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <mysql_driver.h>

#include <print>
#include <string>
#include <format>
#include <optional>
#include <expected>

#include "FindString.h"

using str = std::string;
using constStr = const std::string;
using constStrRef = const std::string&;

struct Assignment {
	str name, subject, url, duedate;
	short importance;
};

class SachoolDB {
private:
	sql::Connection* m_Con;
	str m_Host, m_User, m_Password, m_Database;
	bool m_Connected;

	bool assignmentExists(constStrRef discordID, constStrRef assignmentName) const;
public:
	SachoolDB(constStrRef host, constStrRef user,
		constStrRef password, constStrRef database);
	~SachoolDB();

	bool connect();
	void disconnect();
	bool isConnected() const;
	void setDatabase(constStrRef database);

	// attachment methods

	/*
	adds an assignment object to the mysql database associated with the discordID of the user
	*/
	bool addAssignment(constStrRef discordID, const Assignment& assignment) const;
	bool removeAssignment(constStrRef discordID, constStrRef assignmentName) const;
};