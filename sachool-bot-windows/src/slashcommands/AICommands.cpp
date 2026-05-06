#include "../../include/AICommands.h"

dpp::task<void> handleAICommands(std::shared_ptr<dpp::cluster>& bot, const dpp::slashcommand_t& event, Database::SachoolDB& db) {
    dpp::command_interaction cmdData = event.command.get_command_interaction();
    auto& subcommand = cmdData.options[0];
    const dpp::snowflake userID = event.command.get_issuing_user().id;
    const dpp::snowflake channelID = event.command.get_channel().id;
    constStr userPfp = event.command.get_issuing_user().get_avatar_url();

    Http::SachoolHttp& http = Http::SachoolHttp::getInstance(bot, db);

    event.thinking(false); // use edit response to reply

    if (subcommand.name == "prompt") {
        str prompt = std::get<str>(event.get_parameter("message"));
    
        http.setPrompt(prompt);
        http.setBot(bot);

        co_await http.sendChat(userID.str());

        str response = http.getLastchat();
        if (response.empty()) {
            co_await event.co_edit_response("Unable to retrieve chat, please try again later.");
            co_return;
        }
        co_await event.co_edit_response(response);
        co_return;
    }
}
