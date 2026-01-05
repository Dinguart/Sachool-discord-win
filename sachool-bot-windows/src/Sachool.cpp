#include "../include/Sachool.h"

Bot::Sachool::Sachool(const std::string& botToken, Database::SachoolDB& db) : m_BotToken(botToken), m_Db(db) {
	m_Bot = std::make_shared<dpp::cluster>(botToken);

	log();
	commandsSetup();
	commandsHandler();
	clickHandler();
	runBot();
}

void Bot::Sachool::log() {
	m_Bot->on_log(dpp::utility::cout_logger());
}

void Bot::Sachool::commandsSetup() {
	m_Bot->on_ready([this](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			registerSlashCommands(m_Bot, event);
		}
	});
}

void Bot::Sachool::commandsHandler() {
	m_Bot->on_slashcommand([this](const dpp::slashcommand_t& event) -> dpp::task<void> {
		co_await handleSlashCommands(m_Bot, event, m_Db);
		co_return;
	});
}

void Bot::Sachool::clickHandler() {
	m_Bot->on_select_click([this](const dpp::select_click_t& event) -> dpp::task<void> {
		co_await handleClickEvents(m_Bot, event);
		co_return;
	});
}

void Bot::Sachool::runBot() {
	m_Bot->start(dpp::st_wait);
}

