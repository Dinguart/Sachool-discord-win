#include <print>
#include <dpp/dpp.h>
#include <string>

#include "../include/ReadFile.h"
#include "../include/Sachool.h"
#include "../include/SachoolDB.h"

int main() {
	std::optional<fileMap> configMap = readFile("config.env");
	if (!configMap.has_value()) {
		std::println("Main exception : Cannot read env file.");
		return -1;
	}

	// init db before bot
	std::string DB_NAME, DB_HOST, DB_PASS, DB_USER;
	DB_HOST = configMap->at("DB_HOST");
	DB_USER = configMap->at("DB_USER");
	DB_PASS = configMap->at("DB_PASS");
	DB_NAME = configMap->at("DB_NAME");
	SachoolDB sachoolDB(DB_HOST, DB_USER, DB_PASS, DB_NAME);
	sachoolDB.connect();

	// init bot
	constStr BOT_TOKEN = configMap->at("BOT_TOKEN");
	Sachool sachoolBot(BOT_TOKEN, sachoolDB);
}