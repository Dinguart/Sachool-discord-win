#pragma once
#include <dpp/dpp.h>
#include <memory>

#include "SachoolDB.h"
#include "WhenDue.h"
#include "Constants.h"

dpp::task<void> handleAssignmentCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::slashcommand_t& event, const Database::SachoolDB& db);