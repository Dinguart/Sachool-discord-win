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
	/*
	@brief
	adds an assignment to the database, with file & importance being optional parameters
	*/
	assignment.add_option(
		dpp::command_option(dpp::co_sub_command, "add", "add an assignment to your profile.")
		.add_option(dpp::command_option(dpp::co_string, "name", "name of the assignment.", true))
		.add_option(dpp::command_option(dpp::co_string, "subject", "which course is the assignment from?", true))
		.add_option(dpp::command_option(dpp::co_string, "duedate", "when is the assignment due? (YYYY/MM/DD)", true))
		.add_option(dpp::command_option(dpp::co_attachment, "file", "file containing the assignment/instructions.", false))
		.add_option(dpp::command_option(dpp::co_integer, "importance", "importance value (1-5) of the assignment", false))
	);
	/*
	@brief
	removes an assignment from the user's profile in db by name
	*/
	assignment.add_option( // have some logic if two assignments with same name exist.
		dpp::command_option(dpp::co_sub_command, "remove", "Specify an assignment to remove.")
		.add_option(dpp::command_option(dpp::co_string, "name", "name of the assignment.", true))
	);

	/*
	@brief
	lets you view an assignment via dropdown menu
	*/
	assignment.add_option(
		dpp::command_option(dpp::co_sub_command, "convert", "Will convert to different file format.")
		.add_option(dpp::command_option(dpp::co_string, "name", "name of the assignment to convert.", true))
	);
	commands.push_back(assignment);

	bot->global_bulk_command_create(commands);
}

dpp::task<void> handleSlashCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::slashcommand_t& event, Database::SachoolDB& db) {
	str commandName = event.command.get_command_name();

	if (commandName == "assignment") {
		co_await handleAssignmentCommands(bot, event, db);
	}

	co_return;
}

dpp::task<void> handleClickEvents(std::shared_ptr<dpp::cluster>& bot, const dpp::select_click_t& event, Database::SachoolDB& db) {
	str selectMenuID = event.custom_id;
	str chosenOption = event.values[0];
	str userID = event.command.get_issuing_user().id.str();

	if (selectMenuID == "fileformat-view") {
		auto pair = SeparateByDelimPair(chosenOption, '-');

		auto assignmentInfo = db.getAssignmentProperties(userID, pair.first);
		if (!assignmentInfo.has_value()) {
			co_await event.co_edit_response("Unexpected error occurred when trying to retrieve assignment, please try again later.");
			co_return;
		}

		str assignmentName = assignmentInfo.value().name;
		dpp::http_request_completion_t httpRes = co_await bot->co_request(
			assignmentInfo.value().url,
			dpp::m_get
		);
		if (httpRes.status == 200) {
			std::ofstream ofile(assignmentName, std::ios_base::binary);
			ofile.write(httpRes.body.c_str(), httpRes.body.size());
			// check the signatures

			
			
			ofile.close();
		}


		// now check if the file format is the same as the one chosen

	}
}
