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
	// we keep the assignment name (allows us to do things from here)
	auto menuIDSeparator = SeparateByDelimPair(selectMenuID, '-');

	event.thinking();

	if (menuIDSeparator.second == "convert") {
		auto pair = SeparateByDelimPair(chosenOption, '-');
		/* todo: fix error pair, its not getting assignment name as pair.first.
		find way to connect the handleclickevents with assignment commands
		
		*/
		auto assignmentInfo = db.getAssignmentProperties(userID, menuIDSeparator.first);
		if (!assignmentInfo.has_value()) {
			co_await event.co_edit_response("Unexpected error occurred when trying to retrieve assignment, please try again later.");
			co_return;
		}

		str assignmentUrl = assignmentInfo.value().url;
		if (assignmentUrl.empty()) {
			co_await event.co_edit_response("No file URL found for this assignment");
			co_return;
		}

		if (assignmentUrl.find("http://") != 0 && assignmentUrl.find("https://") != 0) {
			co_await event.co_edit_response("Invalid file URL format.");
			co_return;
		}
		str assignmentName = assignmentInfo.value().name;

		bot->log(dpp::ll_info, "Fetching assignment URL: " + assignmentUrl);

		dpp::http_request_completion_t httpRes = co_await bot->co_request(
			assignmentInfo.value().url,
			dpp::m_get
		);
		if (httpRes.status == 200) {
			std::ofstream ofile(assignmentName, std::ios_base::binary);
			ofile.write(httpRes.body.c_str(), httpRes.body.size());
			ofile.close();
			// check the signatures
			std::ifstream ifile(assignmentName, std::ios_base::binary);
			auto converted = convertFile(pair.first, ifile, FileFormatSignatures::signatureMap, assignmentName);
			if (!converted.first.successfullyConverted) {
				if (!converted.second.has_value()) {
					co_await event.co_edit_response("Unexpected error occurred trying to convert your assignment file.");
				}
				else {
					switch (converted.second.value()) {
					case FileContext::EXCEPTION: co_await event.co_edit_response("Unexpected error occurred trying to convert your assignment file.");
						break;
					case FileContext::EARLY_EOF: co_await event.co_edit_response("Problem with verifying file type, please try another file format to convert.");
						break;
					case FileContext::CONVERSION_NOT_NEEDED: co_await event.co_edit_response("Your file is already of this file format!");
						break;
					}
				}
				co_return;
			}

			std::unique_ptr<CImg<unsigned char>> convertedFile = std::move(converted.first.fileData);
			/* save the image to a buffer depending on the file format, then send the buffer data to the request */
			/* send the image to a server endpoint for retrieval of the url */
			
			str outputPath = assignmentName;
			dpp::message msg;
			msg.add_file(outputPath, dpp::utility::read_file(outputPath)).set_flags(dpp::m_ephemeral);


			co_await event.co_edit_response(msg);
		}
		else {
			co_await event.co_edit_response("An unexpected error was found when attempting to fetch to convert assignment.");
		}


		// now check if the file format is the same as the one chosen

	}
}
