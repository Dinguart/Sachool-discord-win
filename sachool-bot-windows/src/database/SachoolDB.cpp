#include "../../include/SachoolDB.h"

Database::SachoolDB::SachoolDB(constStrRef host, constStrRef user,
	constStrRef password, constStrRef database) :
	m_Host(host), m_User(user), m_Password(password), 
	m_Database(database), m_Con(nullptr), m_Connected(false) {
}

Database::SachoolDB& Database::SachoolDB::getInstance(constStrRef host, constStrRef user,
	constStrRef password, constStrRef database) {
	static SachoolDB db(host, user, password, database);
	return db;
}

Database::SachoolDB::~SachoolDB() {
	disconnect();
}

bool Database::SachoolDB::checkCurrentConnection() const {
	if (!m_Connected) {
		std::println("Not connected to the database.");
		return false;
	}
	return true;
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
	if (!checkCurrentConnection()) return std::nullopt;

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
	if (!checkCurrentConnection()) return { Context::INTERNAL, false };

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
	if (!checkCurrentConnection()) return { Context::INTERNAL, false };

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

StateContext Database::SachoolDB::handleAITokens(constStrRef discordID) const {
	if (!checkCurrentConnection()) return { Context::INTERNAL, false };

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("SELECT lastmodified FROM useraiprofiles WHERE discordid = ?")
		);
		stmt->setString(1, discordID);

		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
		if (res->next()) {
			str lastModified = res->getString("lastmodified");
			auto parseCurr = DateLogic::parseCurrDate();
			if (!parseCurr.has_value()) { return { Context::EXCEPTION, false }; }
			auto parseLastModified = DateLogic::dateParse(lastModified);

			// if the next day passed reset their tokens
			if (parseCurr->day > parseLastModified->day) {
				modifyTokenCount(discordID, TOKEN_LIMIT, false);
			}

			// add 1 to their count every 24 minutes
			int32_t timeBetween = (parseCurr->hours * 60 + parseCurr->minutes) - (parseLastModified->hours * 60 + parseLastModified->minutes);
			if (parseCurr->day == parseLastModified->day && timeBetween > 24) {
				if (timeBetween > 120) modifyTokenCount(discordID, 5, false);
				else modifyTokenCount(discordID, static_cast<int16_t>(timeBetween / 24), false);
			}

			return { std::nullopt, true };
		}
		else {
			std::string currDateStr = DateLogic::getCurrDate();
			std::unique_ptr<sql::PreparedStatement> insertstmt(
				m_Con->prepareStatement("INSERT INTO useraiprofiles (discordid, aitokens, lastmodified) VALUES (?, ?, ?)")
			);
			insertstmt->setString(1, discordID);
			insertstmt->setUInt(2, TOKEN_LIMIT);
			insertstmt->setString(3, currDateStr);
			insertstmt->executeQuery();

			// call it again now that the user is added.
			return handleAITokens(discordID);
		}
	}
	catch (const sql::SQLException& e) {
		std::println("Database token handling exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database token handling exception (std) : {}", e.what());
	}
	return { Context::EXCEPTION, false };
}

bool Database::SachoolDB::UseAITokens(constStrRef discordID) const {
	if (!checkCurrentConnection()) return false;

	// use a token, but if they dont have any, we prevent the usage of the command
	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("SELECT * FROM useraiprofiles WHERE discordid = ?")
		);
		stmt->setString(1, discordID);

		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
		if (res->next()) {
			if (res->getUInt("aitokens") == 0) return false;
		}

		std::string currDateStr = DateLogic::getCurrDate();
		std::unique_ptr<sql::PreparedStatement> updatestmt(
			m_Con->prepareStatement("UPDATE useraiprofiles SET aitokens = aitokens - 1, lastmodified = ? WHERE discordid = ?")
		);
		updatestmt->setString(1, currDateStr);
		updatestmt->setString(2, discordID);
		updatestmt->executeQuery();
		return true;
	}
	catch (const sql::SQLException& e) {
		std::println("Database token usage exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database token usage exception (std) : {}", e.what());
	}
	return false;
}

bool Database::SachoolDB::assignmentExists(constStrRef discordID, constStrRef assignmentName) const {
	if (!checkCurrentConnection()) return false;
	
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

bool Database::SachoolDB::isTableEmpty(constStr sqlTable) const {
	if (!checkCurrentConnection()) return false;

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("SELECT EXISTS(SELECT 1 FROM " + sqlTable + ") AS Output")
		);
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

		if (res->next()) {
			if (res->getInt("Output") == 1) return false;
		}
		return true;
	}
	catch (const sql::SQLException& e) {
		std::println("Table is empty exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Table is empty exception (std) : {}", e.what());
	}
}

StateContext Database::SachoolDB::modifyTokenCount(constStrRef discordID, uint16_t amount, bool increment) const {
	if (!checkCurrentConnection()) return { Context::INTERNAL, false };

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("SELECT * FROM useraiprofiles WHERE discordid = ?")
		);
		stmt->setString(1, discordID);
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

		std::optional<str> tokenstmt = std::nullopt;
		if (res->next()) {
			uint16_t tokens = res->getUInt("aitokens");
			std::string currDateStr = DateLogic::getCurrDate();
			if (increment) {
				if (tokens + amount < TOKEN_LIMIT) {
					tokenstmt = std::format("UPDATE useraiprofiles SET aitokens = aitokens + {}, lastmodified = {} WHERE discordid = {}", amount, currDateStr, discordID);
				}
			}
			else {
				if (tokens < amount) {
					tokenstmt = std::format("UPDATE useraiprofiles SET aitokens = {}, lastmodified = {} WHERE discordid = {}", amount, currDateStr, discordID);
				}
			}
		}
		if (!tokenstmt.has_value()) return { Context::NORMAL, false };

		std::unique_ptr<sql::PreparedStatement> updatestmt(
			m_Con->prepareStatement(tokenstmt.value())
		);
		stmt->executeQuery();
		return { std::nullopt, true };
	}
	catch (const sql::SQLException& e) {
		std::println("Database assignment existence exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database assignment existence exception (std) : {}", e.what());
	}
	return { Context::EXCEPTION, false };
}

bool Database::SachoolDB::setImageUrl(constStrRef discordID, constStrRef Url) const {
	if (!checkCurrentConnection()) return false;

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
	if (!checkCurrentConnection()) return std::nullopt;

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement("SELECT * FROM temporaryimages WHERE discordid = ? ORDER BY url DESC")
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

bool Database::SachoolDB::removeImageUrl(constStrRef discordID) const {
	if (!checkCurrentConnection()) return false;

	try {
		std::unique_ptr<sql::PreparedStatement> stmt(
			m_Con->prepareStatement(
				"DELETE FROM temporaryimages WHERE discordid = ?"
			)
		);
		stmt->setString(1, discordID);
		stmt->executeQuery();

		if (!isTableEmpty("temporaryimages")) return false;

		return true;
	}
	catch (const sql::SQLException& e) {
		std::println("Database tempimage removal exception (sql) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("Database tempimage removal exception (std) : {}", e.what());
	}
	return false;
}