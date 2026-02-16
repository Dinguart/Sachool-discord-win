#include "../../include/AssignmentCommands.h"

dpp::task<void> handleAssignmentCommands(std::shared_ptr<dpp::cluster>& bot, dpp::slashcommand_t event, const Database::SachoolDB& db) {
	dpp::command_interaction cmdData = event.command.get_command_interaction();
	auto& subcommand = cmdData.options[0];
	const dpp::snowflake userID = event.command.get_issuing_user().id;
    const dpp::snowflake channelID = event.command.get_channel().id;
    constStr userPfp = event.command.get_issuing_user().get_avatar_url();
    
    event.thinking(true); // use edit response to reply

    if (subcommand.name == "add") {
        str name = std::get<str>(event.get_parameter("name"));
        str subject = std::get<str>(event.get_parameter("subject"));
        str dueDate = std::get<str>(event.get_parameter("duedate"));

        auto fileIDParam = event.get_parameter("file");
        dpp::snowflake fileID;
        if (std::holds_alternative<dpp::snowflake>(fileIDParam)) {
            fileID = std::get<dpp::snowflake>(fileIDParam);
        }
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
        if (auto assignmentAdded = db.addAssignment(userID.str(), assignment); !assignmentAdded.state) {
            switch (assignmentAdded.context.value()) {
            case Context::NORMAL: 
                co_await event.co_edit_response("This assignment already exists, assignment insertion failed."); 
                co_return;
                break;
            case Context::EXCEPTION:
            case Context::INTERNAL: 
                co_await event.co_edit_response("Unexpected error occurred when trying to add assignment, please try again later."); 
                co_return;
                break;
            }
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
    else if (subcommand.name == "remove") {
        str assignmentName = std::get<str>(event.get_parameter("name"));
            
        if (auto assignmentRemoved = db.removeAssignment(userID.str(), assignmentName); !assignmentRemoved.state) {
            if (!assignmentRemoved.context.has_value()) { 
                co_await event.co_edit_response("Unexpected error occurred when trying to remove assignment, please try again later.");
                co_return;
            }
            switch (assignmentRemoved.context.value()) {
            case Context::NORMAL: 
                co_await event.co_edit_response("Assignment to remove was not found listed."); 
                break;
            case Context::EXCEPTION:
            case Context::INTERNAL: 
                co_await event.co_edit_response("Unexpected error occurred when trying to remove assignment, please try again later.");           
                break;
            }
            co_return;
        }

        dpp::embed removalEmbed;
        removalEmbed.set_title("Assignment " + assignmentName + " removed successfully!")
            .set_color(dpp::colors::red_wine)
            .set_thumbnail(userPfp)
            .set_image("https://dpp.dev/DPP-Logo.png")
            .set_timestamp(time(0));
        
        dpp::message msg(channelID, removalEmbed);

        co_await event.co_edit_response(msg);
        co_return;
    }
    else if (subcommand.name == "convert") {
        str assignmentName = std::get<str>(event.get_parameter("name"));

        if (auto assignmentURL = db.getAssignmentProperties(userID.str(), assignmentName); !assignmentURL.has_value() || assignmentURL.value().url.empty()) {
            if (!assignmentURL.has_value()) co_await event.co_edit_response("Unexpected error occurred when trying to convert assignment, please try again later.");
            else if (assignmentURL.value().url.empty()) co_await event.co_edit_response("Assignment provided does not contain a URL.");
            co_return;
        }

        dpp::message viewSelect(channelID.str(), "Select a file format to convert this assignment to!");

        viewSelect.add_component(
            dpp::component().add_component(
                dpp::component()
                .set_type(dpp::cot_selectmenu)
                .set_placeholder("File format dropdown") // the value is "[fileformat]-[byte offset]-[byte size]"
                .add_select_option(dpp::select_option("png", "png-png", "png file format"))
                .add_select_option(dpp::select_option("pdf", "pdf-pdf", "pdf file format"))
                .add_select_option(dpp::select_option("ppm", "ppm-ppm", "ppm6 file format (p6)"))
                .add_select_option(dpp::select_option("jpeg", "jpeg-jpeg", "jpeg file format{beta}"))
                .set_id(assignmentName+"-convert")
            )
        );

        co_await event.co_edit_response(viewSelect);
        co_return;
    }
}
