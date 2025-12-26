#include "../../include/SachoolDB.h"

SachoolDB::SachoolDB(constStrRef host, constStrRef user,
	constStrRef password, constStrRef database) :
	m_Host(host), m_User(user), m_Password(password), 
	m_Database(database), m_Con(nullptr), m_Connected(false) {
}

SachoolDB::~SachoolDB() {
	disconnect();
}

bool SachoolDB::connect() {
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		m_Con = driver->connect(m_Host, m_User, m_Password);
		m_Con->setSchema(m_Database);
		m_Connected = true;
	}
	catch (const sql::SQLException& e) {
		std::println("Database connection exception (sql) : {}", e.what());
		m_Connected = false;
	}
	catch (const std::exception& e) {
		std::println("Database connection exception (std) : {}", e.what());
		m_Connected = false;
	}
	return m_Connected;
}

void SachoolDB::disconnect() {
	if (m_Con) {
		delete m_Con;
		m_Con = nullptr;
		m_Connected = false;
	}
}

bool SachoolDB::isConnected() const {
	return m_Connected;
}

void SachoolDB::setDatabase(constStrRef database) {
	m_Database = database;
}

// assignment methods

bool SachoolDB::addAssignment(constStrRef discordID, const Assignment& assignment) const
{
	if (!m_Connected) {
		std::println("Not connected to the database.\n");
		return false;
	}

	if (assignmentExists(discordID, assignment.name)) return false;

	try {
		str assignmentJson = std::format(
			R"({{"name": "{}", "subject": "{}", "url": "{}", "duedate": "{}", "importance": "{}"}})",
			assignment.name, assignment.subject, assignment.url, assignment.duedate, assignment.importance
		);
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("INSERT INTO userprofiles (discordid, assignments) VALUES (?, ?)")
		);
		stmt->setString(1, discordID);
		stmt->setString(2, assignmentJson);
		stmt->executeQuery();

		return true;
	}
	catch (const sql::SQLException& e) {
		std::println("Database assignment insertion exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database assignment insertion exception (std) : {}", e.what());
	}
	return false;
}

bool SachoolDB::removeAssignment(constStrRef discordID, constStrRef assignmentName) const
{
	if (!m_Connected) {
		std::println("Not connected to the database.\n");
		return false;
	}

	if (!assignmentExists(discordID, assignmentName)) return false;

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("DELETE FROM userprofiles WHERE discordid = ? AND assignments->>'$.name' = ?")
		);
		stmt->setString(1, discordID);
		stmt->setString(2, assignmentName);
		stmt->executeQuery();

		return true;
	}
	catch (const sql::SQLException& e) {
		std::println("Database assignment removal exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database assignment removal exception (std) : {}", e.what());
	}
	return false;
}

bool SachoolDB::assignmentExists(constStrRef discordID, constStrRef assignmentName) const
{
	nlohmann::json json;
	str assignmentString;
	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("SELECT * FROM userprofiles WHERE discordid = ?")
		);
		stmt->setString(1, discordID);

		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
		while (res->next()) {
			assignmentString = res->getString("assignments");
			json = nlohmann::json::parse(assignmentString);

			if (json["name"].get<std::string>() == assignmentName) return true;
		}
	}
	catch (const sql::SQLException& e) {
		std::println("Database assignment existence exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database assignment existence exception (std) : {}", e.what());
	}
	return false;
}
