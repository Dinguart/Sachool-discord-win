#pragma once
#include <dpp/dpp.h>
#include <memory>
#include <print>

#include "SachoolDB.h"

dpp::task<void> handleUtilityCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::slashcommand_t& event, const Database::SachoolDB& db);