#include "../../include/SachoolDB.h"

Database::SachoolDB::SachoolDB(constStrRef host, constStrRef user,
	constStrRef password, constStrRef database) :
	m_Host(host), m_User(user), m_Password(password), 
	m_Database(database), m_Con(nullptr), m_Connected(false) {
}

Database::SachoolDB::~SachoolDB() {
	disconnect();
}

bool Database::SachoolDB::connect() {
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

void Database::SachoolDB::disconnect() {
	if (m_Con) {
		delete m_Con;
		m_Con = nullptr;
		m_Connected = false;
	}
}

bool Database::SachoolDB::isConnected() const {
	return m_Connected;
}

void Database::SachoolDB::setDatabase(constStrRef database) {
	m_Database = database;
}

// assignment methods

std::optional<Assignment> Database::SachoolDB::getAssignmentProperties(constStrRef discordID, constStrRef assignmentName) const {
	if (!m_Connected) {
		std::println("Not connected to the database.\n");
		return std::nullopt;
	}

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement(
				"SELECT jt.*FROM userprofiles,"
				"JSON_TABLE("
				"assignments,"
				"'$' COLUMNS("
				"url VARCHAR(510) PATH '$.url',"
				"name VARCHAR(255) PATH '$.name',"
				"duedate VARCHAR(255) PATH '$.duedate',"
				"subject VARCHAR(255) PATH '$.subject',"
				"importance INT PATH '$.importance'"
				")) AS jt WHERE discordid = ? AND jt.name = ?;"
			)
		);
		stmt->setString(1, discordID);
		stmt->setString(2, assignmentName);
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

		Assignment asmt;
		if (res->next()) {
			asmt.url = res->getString("url");
			asmt.name = res->getString("name");
			asmt.duedate = res->getString("duedate");
			asmt.subject = res->getString("subject");
			asmt.importance = res->getUInt("importance");

			return asmt;
		}
	}
	catch (const sql::SQLException& e) {
		std::println("Database assignment viewing exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database assignment viewing exception (std) : {}", e.what());
	}
	return std::nullopt;
}

StateContext Database::SachoolDB::addAssignment(constStrRef discordID, const Assignment& assignment) const {
	if (!m_Connected) {
		std::println("Not connected to the database.\n");
		return { Context::INTERNAL, false };
	}

	if (assignmentExists(discordID, assignment.name)) return { Context::NORMAL, false };

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

		return { std::nullopt, true };
	}
	catch (const sql::SQLException& e) {
		std::println("Database assignment insertion exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database assignment insertion exception (std) : {}", e.what());
	}
	return { Context::EXCEPTION, false };
}

StateContext Database::SachoolDB::removeAssignment(constStrRef discordID, constStrRef assignmentName) const {
	if (!m_Connected) {
		std::println("Not connected to the database.\n");
		return { Context::INTERNAL, false };
	}

	if (!assignmentExists(discordID, assignmentName)) return { Context::NORMAL, false };

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("DELETE FROM userprofiles WHERE discordid = ? AND assignments->>'$.name' = ?")
		);
		stmt->setString(1, discordID);
		stmt->setString(2, assignmentName);
		stmt->executeQuery();

		return { std::nullopt, true };
	}
	catch (const sql::SQLException& e) {
		std::println("Database assignment removal exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database assignment removal exception (std) : {}", e.what());
	}
	return { Context::EXCEPTION, false };
}

bool Database::SachoolDB::assignmentExists(constStrRef discordID, constStrRef assignmentName) const
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

			if (json["name"].get_ref<const std::string&>() == assignmentName) return true;
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

bool Database::SachoolDB::setImageUrl(constStrRef discordID, constStrRef Url) const {
	if (!m_Connected) {
		std::println("Not connected to the database.\n");
		return false;
	}

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("INSERT INTO temporaryimages (discordid, url) VALUES (?, ?)")
		);
		stmt->setString(1, discordID);
		stmt->setString(2, Url);
		stmt->executeQuery();

		return true;
	}
	catch (const sql::SQLException& e) {
		std::println("Database assignment existence exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database assignment existence exception (std) : {}", e.what());
	}
	return false;
}

std::optional<str> Database::SachoolDB::getImageUrl(constStrRef discordID) const {
	if (!m_Connected) {
		std::println("Not connected to the database.\n");
		return std::nullopt;
	}

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("SELECT * FROM temporaryimages WHERE discordid = ?")
		);
		stmt->setString(1, discordID);
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

		str imageUrl;
		if (res->next()) {
			imageUrl = res->getString("url");
		}

		return imageUrl;
	}
	catch (const sql::SQLException& e) {
		std::println("Database assignment existence exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database assignment existence exception (std) : {}", e.what());
	}
	return std::nullopt;
}