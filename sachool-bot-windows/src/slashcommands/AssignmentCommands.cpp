#include "../../include/AssignmentCommands.h"

dpp::task<void> handleAssignmentCommands(std::shared_ptr<dpp::cluster>& bot, dpp::slashcommand_t event, const SachoolDB& db) {
	dpp::command_interaction cmdData = event.command.get_command_interaction();
	auto& subcommand = cmdData.options[0];
	const dpp::snowflake userID = event.command.get_issuing_user().id;
    const dpp::snowflake channelID = event.command.get_channel().id;

	if (subcommand.name == "add") {
        event.co_thinking(true);
		str name = std::get<str>(event.get_parameter("name"));
		str subject = std::get<str>(event.get_parameter("course/subject"));
		dpp::snowflake fileID = std::get<dpp::snowflake>(event.get_parameter("file"));
		dpp::attachment att = event.command.get_resolved_attachment(fileID);
		int importance = std::get<int64_t>(event.get_parameter("importance"));
		str dueDate = std::get<str>(event.get_parameter("duedate"));

		Assignment assignment = { name, subject, att.url, dueDate, importance };
		if (bool assignmentAdded = db.addAssignment(userID.str(), assignment); !assignmentAdded) {
            co_await event.co_reply("Unexpected error occurred when trying to add assignment, please try again later.");
			co_return;
		}

        dpp::embed assignmentEmbed = dpp::embed()
            .set_color(dpp::colors::green_leaves)
            .set_title("Assignment " + name + " added!")
            .set_url(att.url)
            .set_description(whenDue(dueDate))
            .set_thumbnail("https://dpp.dev/DPP-Logo.png") // change to bot icon.
            .add_field(
                "Assignment importance",
                std::to_string(importance)
            )
            .add_field(
                "Subject",
                subject,
                true
            )
            .add_field(
                "Due date",
                dueDate,
                true
            )
            .set_image("https://dpp.dev/DPP-Logo.png") // replace with bot icon
            .set_timestamp(time(0));

        /* Create a message with the content as our new embed. */
        dpp::message msg(channelID, assignmentEmbed);
        
        co_await event.co_reply(msg);
        co_return;
	}
}
