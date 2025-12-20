#pragma once
#include <dpp/dpp.h>
#include <string>
#include <memory>

#include "SlashCommands.h"
#include "SachoolDB.h"

class Sachool {
private:
	SachoolDB m_Db;
	std::shared_ptr<dpp::cluster> m_Bot;
	std::string m_BotToken;
	
	void commandsSetup();
	void commandsHandler();
	void runBot();

public:
	explicit Sachool(const std::string& botToken, SachoolDB& db);
	void log();
};