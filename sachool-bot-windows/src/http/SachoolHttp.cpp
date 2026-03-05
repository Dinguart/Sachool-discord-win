#include "../../include/SachoolHttp.h"
/* 
add rate limiting via tokens / asks
use db for it
ex: each user gets 5 by default, when u ask sum, it drops to 4
once it drops to 0, you must wait 2 hours for it to refill.
each token refills per 24 minutes

handle requests as well
requests per minute >= global limit
deny request


*/
Http::SachoolHttp::SachoolHttp(Database::SachoolDB& db) : m_Db(db) {
	m_ApiKey = configMap->at("GROQ_API_KEY");
	m_GroqPrompt = "";
	setRequestModel();
}

Http::SachoolHttp& Http::SachoolHttp::getInstance(Database::SachoolDB& db) {
	static SachoolHttp http(db);
	return http;
}

Http::SachoolHttp::SachoolHttp(std::shared_ptr<dpp::cluster> bot, Database::SachoolDB& db) : m_Bot(bot), m_Db(db) {
	m_ApiKey = configMap->at("GROQ_API_KEY");
	m_GroqPrompt = "";
	setRequestModel();
}

Http::SachoolHttp& Http::SachoolHttp::getInstance(std::shared_ptr<dpp::cluster> bot, Database::SachoolDB& db) {
	static SachoolHttp http(bot, db);
	return http;
}

Http::SachoolHttp::SachoolHttp(const std::string& prompt, std::shared_ptr<dpp::cluster> bot, Database::SachoolDB& db) : m_Bot(bot), m_Db(db) {
	m_ApiKey = configMap->at("GROQ_API_KEY");
	m_GroqPrompt = prompt;
	setRequestModel();
}

Http::SachoolHttp& Http::SachoolHttp::getInstance(const std::string& prompt, std::shared_ptr<dpp::cluster> bot, Database::SachoolDB& db) {
	static SachoolHttp http(prompt, bot, db);
	return http;
}

void Http::SachoolHttp::setRequestModel() {
	m_Body["model"] = "llama-3.3-70b-versatile";
	m_Body["messages"] = nlohmann::json::array();
}

void Http::SachoolHttp::setBot(std::shared_ptr<dpp::cluster> bot) { 
	m_Bot = bot;
}

void Http::SachoolHttp::setPrompt(const std::string& prompt) {
	m_GroqPrompt = prompt;
}

std::string Http::SachoolHttp::getLastChat() const {
	if (!m_GroqResponse.empty()) return m_GroqResponse;
	return "";
}

dpp::task<void> Http::SachoolHttp::sendChat() {
	try {
		m_Body["messages"].push_back({
			{"role", "user"},
			{"content", m_GroqPrompt}
		});

		auto response = co_await m_Bot->co_request(
			"https://api.groq.com/openai/v1/chat/completions",
			dpp::m_post,
			m_Body.dump(),
			"application/json",
			{
				{"Authorization", "Bearer " + m_ApiKey},
				{"Content-Type", "application/json"}
			}
		);
		// add this to ai command group

		if (response.status == 200) {
			nlohmann::json output;
			output = nlohmann::json::parse(response.body);

			m_GroqResponse = output["choices"][0]["message"]["content"].get<std::string>();
			
			// save the chat in messages array
			m_Body["messages"].push_back({
				{"role", "assistant"},
				{"content", m_GroqResponse}
			});
			
			
			co_return;
		}
		else if (response.status == 429) {
			// resend with a timer, if fails then tell user to try again later
			std::println("429");
		}
		else {
			// handle other stuff
			std::println("other error: {}", response.status);
		}
	}
	catch (const dpp::exception& e) {
		std::println("AI chat exception (dpp) : {}", e.what());
	}
	catch (const nlohmann::json::exception& e) {
		std::println("AI chat exception (json) : {}", e.what());
	}
	catch (...) {
		std::println("AI chat exception (not specified)");
	}
	co_return;
}