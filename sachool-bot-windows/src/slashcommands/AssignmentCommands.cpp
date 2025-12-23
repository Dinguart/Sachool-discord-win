#include "../../include/AssignmentCommands.h"

dpp::task<void> handleAssignmentCommands(std::shared_ptr<dpp::cluster>& bot, dpp::slashcommand_t event, const SachoolDB& db) {
	dpp::command_interaction cmdData = event.command.get_command_interaction();
	auto& subcommand = cmdData.options[0];
	const dpp::snowflake userID = event.command.get_issuing_user().id;
    const dpp::snowflake channelID = event.command.get_channel().id;

    if (subcommand.name == "add") {
        event.thinking(true);
        str name = std::get<str>(event.get_parameter("name"));
        str subject = std::get<str>(event.get_parameter("subject"));
        str dueDate = std::get<str>(event.get_parameter("duedate"));

        auto fileIDParam = event.get_parameter("file");
        dpp::snowflake fileID;
        if (std::holds_alternative<dpp::snowflake>(fileIDParam)) {
            fileID = std::get<dpp::snowflake>(fileIDParam);
        }
        str userPfp = event.command.get_issuing_user().get_avatar_url();
        str assignmentURL = "No URL provided";

        dpp::embed assignmentEmbed;
        // fix this logic later (code can be cleaned up here)
        if (!fileID.empty()) {
            dpp::attachment att = event.command.get_resolved_attachment(fileID);
            assignmentURL = att.url;
            assignmentEmbed.set_url(assignmentURL);
        }

        int64_t importanceVal = 0;
        auto importanceParam = event.get_parameter("importance");
        if (std::holds_alternative<int64_t>(importanceParam)) {
            importanceVal = std::get<int64_t>(importanceParam);
        }

        Assignment assignment = { name, subject, assignmentURL, dueDate, importanceVal };
        if (bool assignmentAdded = db.addAssignment(userID.str(), assignment); !assignmentAdded) {
            co_await event.co_edit_response("Unexpected error occurred when trying to add assignment, please try again later.");
            co_return;
        }

        str importanceStr = std::to_string(importanceVal);
        if (!importanceVal) importanceStr = "No importance value listed";

        assignmentEmbed
            .set_color(dpp::colors::green_leaves)
            .set_title("Assignment " + name + " added!")
            .set_description(whenDue(dueDate))
            .set_thumbnail(userPfp)
            .add_field(
                "Assignment importance",
                importanceStr
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

        dpp::message msg(channelID, assignmentEmbed);

        co_await event.co_edit_response(msg);
        co_return;
    }
}
