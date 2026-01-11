#pragma once
#include <dpp/dpp.h>
#include <memory>
#include <vector>

#include "SachoolDB.h"

#include "AssignmentCommands.h"
#include "FindString.h"
#include "FileUtil.h"

void registerSlashCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::ready_t& event);

dpp::task<void> handleSlashCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::slashcommand_t& event, Database::SachoolDB& db);

dpp::task<void> handleClickEvents(std::shared_ptr <dpp::cluster>& bot, const dpp::select_click_t& event, Database::SachoolDB& db);