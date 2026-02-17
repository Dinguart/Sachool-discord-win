#include "../../include/UtilityCommands.h"

dpp::task<void> handleUtilityCommands(std::shared_ptr<dpp::cluster>& bot, const dpp::slashcommand_t& event, const Database::SachoolDB& db) {
    dpp::command_interaction cmdData = event.command.get_command_interaction();
    auto& subcommand = cmdData.options[0];
    const dpp::snowflake userID = event.command.get_issuing_user().id;
    const dpp::snowflake channelID = event.command.get_channel().id;
    constStr userPfp = event.command.get_issuing_user().get_avatar_url();

    event.thinking(true);

    if (subcommand.name == "convert-image") {
        auto attachmentIDParam = event.get_parameter("image");
        dpp::snowflake attachmentID;
        if (std::holds_alternative<dpp::snowflake>(attachmentIDParam)) {
            attachmentID = std::get<dpp::snowflake>(attachmentIDParam);
        }
        str imageURL = "No URL provided";
        if (!attachmentID.empty()) {
            dpp::attachment att = event.command.get_resolved_attachment(attachmentID);
            imageURL = att.url;
        }

        dpp::message viewSelect(channelID.str(), "Select an image format to convert this image to!");
        std::println("{}\n", imageURL);

        if (bool insertUrl = db.setImageUrl(userID.str(), imageURL); !insertUrl) {
            co_await event.co_edit_response("Unexpected error occurred trying to convert image.");
            co_return;
        }

        viewSelect.add_component(
            dpp::component().add_component(
                dpp::component()
                .set_type(dpp::cot_selectmenu)
                .set_placeholder("File format dropdown") // the value is "[fileformat]-[byte offset]-[byte size]"
                .add_select_option(dpp::select_option("png", "png-png", "png file format"))
                .add_select_option(dpp::select_option("pdf", "pdf-pdf", "pdf file format"))
                .add_select_option(dpp::select_option("ppm", "ppm-ppm", "ppm6 file format (p6)"))
                .add_select_option(dpp::select_option("jpeg", "jpeg-jpeg", "jpeg file format{beta}"))
                .set_id("image|convertutility")
            )
        );

        co_await event.co_edit_response(viewSelect);
        co_return;
    }
}