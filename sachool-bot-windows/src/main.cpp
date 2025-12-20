#include <print>
#include <dpp/dpp.h>
#include <string>

#include "../include/ReadFile.h"
#include "../include/Sachool.h"
#include "../include/SachoolDB.h"

int main() {
	if (auto configMap = readFile("config.env"); !configMap.has_value()) {
		std::println("Main exception : Cannot read env file.");
		return -1;
	}
	fileMap configMap = readFile("config.env").value();

	// init db before bot
	std::string DB_NAME, DB_HOST, DB_PASS, DB_USER;
	DB_HOST = configMap["DB_HOST"];
	DB_USER = configMap["DB_USER"];
	DB_PASS = configMap["DB_PASS"];
	DB_NAME = configMap["DB_NAME"];
	SachoolDB sachoolDB(DB_HOST, DB_USER, DB_PASS, DB_NAME);

	// init bot
	constStr BOT_TOKEN = configMap["BOT_TOKEN"];
	Sachool sachoolBot(BOT_TOKEN, sachoolDB);
}