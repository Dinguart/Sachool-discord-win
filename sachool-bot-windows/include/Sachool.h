#pragma once
#include <dpp/dpp.h>
#include <string>
#include <memory>

#include "SlashCommands.h"
#include "SachoolDB.h"
#include "SachoolHttp.h"

namespace Bot {
	class Sachool {
	private:
		explicit Sachool(const std::string& botToken, Database::SachoolDB& db, Http::SachoolHttp& http);
		
		Database::SachoolDB& m_Db;
		Http::SachoolHttp& m_Http;
		std::shared_ptr<dpp::cluster> m_Bot;
		std::string m_BotToken;

		void commandsSetup();
		void commandsHandler();
		void clickHandler();
		void runBot();

	public:
		// singleton
		static Sachool& getInstance(const std::string& botToken, Database::SachoolDB& db, Http::SachoolHttp& http);
		
		// delete copy & assignment operator 
		Sachool(Sachool&) = delete;
		void operator=(Sachool&) = delete;

		void log();
	};
}