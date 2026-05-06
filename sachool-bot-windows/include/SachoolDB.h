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
#include <chrono>

#include "FindString.h"
#include "WhenDue.h"

using str = std::string;
using constStr = const std::string;
using constStrRef = const std::string&;

constexpr uint8_t TOKEN_LIMIT = 5;

/*
in this case, context refers to the context in which a function returns, this is usually for bool functions that require
more context, as both an error and a failure refer to a false return value. the following contexts are as follows:
NORMAL -> regular (no error)
EXCEPTION -> refers to when exception is thrown.
INTERNAL -> refers to an internal check failing, such as the db being disconnected.
*/
enum class Context {
	NORMAL,
	EXCEPTION,
	INTERNAL
};

struct StateContext {
	std::optional<Context> context;
	bool state;
};

struct Assignment {
	str name, subject, url, duedate;
	short importance;
};

namespace Database {
	class SachoolDB {
	private:
		SachoolDB(constStrRef host, constStrRef user,
			constStrRef password, constStrRef database);

		sql::Connection* m_Con;
		str m_Host, m_User, m_Password, m_Database;
		bool m_Connected;

		bool checkCurrentConnection() const;
		bool assignmentExists(constStrRef discordID, constStrRef assignmentName) const;
		bool isTableEmpty(constStr sqlTable) const;
		// ai private methods
		StateContext modifyTokenCount(constStrRef discordID, uint16_t amount, bool increment) const;
	public:
		/* singleton class, as database is only created once in main */
		static SachoolDB& getInstance(constStrRef host, constStrRef user,
			constStrRef password, constStrRef database);
			
		// delete copy & assignment operators
		SachoolDB(const SachoolDB&) = delete;
		void operator=(const SachoolDB&) = delete;

		~SachoolDB();

		bool connect();
		void disconnect();
		bool isConnected() const;
		void setDatabase(constStrRef database);

		// state checking

		// attachment methods
		bool setImageUrl(constStrRef discordID, constStrRef Url) const;
		std::optional<str> getImageUrl(constStrRef discordID) const;
		bool removeImageUrl(constStrRef discordID) const;

		// simple getters
		std::optional<Assignment> getAssignmentProperties(constStrRef discordID, constStrRef assignmentName) const;
		/*
		adds an assignment object to the mysql database associated with the discordID of the user
		*/
		StateContext addAssignment(constStrRef discordID, const Assignment& assignment) const;
		StateContext removeAssignment(constStrRef discordID, constStrRef assignmentName) const;

		// ai methods
		StateContext handleAITokens(constStrRef discordID) const;
		bool UseAITokens(constStrRef discordID) const;
	};
}