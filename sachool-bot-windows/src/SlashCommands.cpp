#include "../include/SlashCommands.h"

void registerSlashCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::ready_t& event) {
	// user subcommands
	//dpp::slashcommand user("User", "Commands for registering into the bot.", bot->me.id);
	// User register
	/*user.add_option(
		dpp::command_option(dpp::co_sub_command, "register", "sign in to the bot.")
		.add_option(dpp::command_option(dpp::co_string, "email", "template", false))
	);*/
	// TODO: think about whether to email / google email add
	// TODO: think about which APIs to use.
	std::vector<dpp::slashcommand> commands;

	// assignment subcommands
	dpp::slashcommand assignment("Assignment", "Commands for assignments.", bot->me.id);
	// assignment add
	assignment.add_option(
		dpp::command_option(dpp::co_sub_command, "add", "add an assignment to your profile.")
		.add_option(dpp::command_option(dpp::co_string, "name", "name of the assignment.", true))
		.add_option(dpp::command_option(dpp::co_string, "subject", "which course is the assignment from?", true))
		.add_option(dpp::command_option(dpp::co_string, "duedate", "when is the assignment due? (YYYY/MM/DD)", true))
		.add_option(dpp::command_option(dpp::co_attachment, "file", "file containing the assignment/instructions.", false))
		.add_option(dpp::command_option(dpp::co_integer, "importance", "importance value (1-5) of the assignment", false))
	);
	// assignment remove
	assignment.add_option( // have some logic if two assignments with same name exist.
		dpp::command_option(dpp::co_sub_command, "remove", "Specify an assignment to remove.")
		.add_option(dpp::command_option(dpp::co_string, "name", "name of the assignment.", true))
	);
	commands.push_back(assignment);

	bot->global_bulk_command_create(commands);
}

dpp::task<void> handleSlashCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::slashcommand_t& event, SachoolDB& db) {
	std::string commandName = event.command.get_command_name();

	if (commandName == "assignment") {
		co_await handleAssignmentCommands(bot, event, db);
	}

	co_return;
}