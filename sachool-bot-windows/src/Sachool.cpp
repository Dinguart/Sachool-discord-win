#include "../include/Sachool.h"

Sachool::Sachool(const std::string& botToken, SachoolDB& db) : m_BotToken(botToken), m_Db(db) {
	m_Bot = std::make_shared<dpp::cluster>(botToken);

	log();
	commandsSetup();
	commandsHandler();
	runBot();
}

void Sachool::log() {
	m_Bot->on_log(dpp::utility::cout_logger());
}

void Sachool::commandsSetup() {
	m_Bot->on_ready([this](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			registerSlashCommands(m_Bot, event);
		}
	});
}

void Sachool::commandsHandler() {
	m_Bot->on_slashcommand([this](const dpp::slashcommand_t& event) -> dpp::task<void> {
		co_await handleSlashCommands(m_Bot, event, m_Db);
		co_return;
	});
}

void Sachool::runBot() {
	m_Bot->start(dpp::st_wait);
}

