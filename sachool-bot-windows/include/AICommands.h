#pragma once
#include <dpp/dpp.h>
#include <memory>

#include "SachoolDB.h"
#include "Constants.h"
#include "SachoolHttp.h"

dpp::task<void> handleAICommands(std::shared_ptr<dpp::cluster>& bot, const dpp::slashcommand_t& event, const Database::SachoolDB& db, Http::SachoolHttp& http);
