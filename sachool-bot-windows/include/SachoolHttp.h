#pragma once

#include <WinSock2.h>
#include <memory>
#include <dpp/dpp.h>
#include <string>
#include <unordered_map>
#include "ReadFile.h"
#include "SachoolDB.h"

#include "Constants.h"

namespace Http {
	class SachoolHttp {
	private:
		explicit SachoolHttp(Database::SachoolDB& db);
        SachoolHttp(const std::string& prompt, Database::SachoolDB& db);

        Database::SachoolDB& m_Db;
        std::shared_ptr<dpp::cluster> m_Bot;
        nlohmann::json m_Body;
        std::string m_GroqPrompt, m_ApiKey;
        std::string m_GroqResponse;

        void setRequestModel();
	public:
        // singleton
        static SachoolHttp& getInstance(Database::SachoolDB& db);
        static SachoolHttp& getInstance(const std::string& prompt, Database::SachoolDB& db);

        // delete copy & assignment operator
        SachoolHttp(SachoolHttp&) = delete;
        void operator=(SachoolHttp&) = delete;

        /* thisll send the prompt to the openai model via the api */
        void setPrompt(const std::string& prompt);
        dpp::task<void> sendChat();
        std::string getLastChat() const;
	};
}