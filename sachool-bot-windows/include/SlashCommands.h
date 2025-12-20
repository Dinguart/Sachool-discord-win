#pragma once
#include <dpp/dpp.h>
#include <memory>
#include <vector>

#include "SachoolDB.h"

#include "AssignmentCommands.h"

void registerSlashCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::ready_t& event);

dpp::task<void> handleSlashCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::slashcommand_t& event, SachoolDB& db);