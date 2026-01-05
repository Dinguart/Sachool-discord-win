#pragma once
#include <dpp/dpp.h>
#include <memory>

#include "SachoolDB.h"

#include "WhenDue.h"

dpp::task<void> handleAssignmentCommands(std::shared_ptr<dpp::cluster>& bot, dpp::slashcommand_t event, const Database::SachoolDB& db);