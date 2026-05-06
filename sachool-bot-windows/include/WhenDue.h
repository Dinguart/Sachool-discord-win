#pragma once
#include <string>
#include <chrono>
#include <format>
#include <print>
#include <exception>
#include <optional>

namespace DateLogic {
	enum class Context {
		INVALID_FORMAT, PAST_DUE, EXCEPTION
	};

	struct Date {
		int16_t year, month, day;
	};

	Date dateParse(const std::string& date);

	std::string whenDue(const std::string& date);

	/* @brief parses current date in yyyy/mm/dd format */
	std::optional<Date> parseCurrDate();

	std::optional<Context> isValidDateFormat(const std::string& input);
}
