#include "../WhenDue.h"

using namespace DateLogic;

Date DateLogic::dateParse(const std::string& date) {
	size_t find = date.find('/');
	std::string year = date.substr(0, find);
	std::string newDate = date.substr(find + 1);
	find = newDate.find('/');
	std::string month = newDate.substr(0, find);
	std::string day = newDate.substr(find + 1);

	int16_t yearNum, monthNum, dayNum;
	yearNum = std::stoul(year);
	monthNum = std::stoul(month);
	dayNum = std::stoul(day);

	return { yearNum, monthNum, dayNum };
}

std::string DateLogic::whenDue(const std::string& date) {
	auto today = std::chrono::utc_clock::now();
	auto currentDate = std::chrono::floor<std::chrono::days>(today);
	std::string dateStr = std::format("{:%Y/%m/%d}", currentDate);

	Date dueDate = DateLogic::dateParse(date);
	Date currDate = DateLogic::dateParse(dateStr);

	int16_t yearDiff, monthDiff, dayDiff;
	yearDiff = dueDate.year - currDate.year;
	monthDiff = dueDate.month - currDate.month;
	dayDiff = dueDate.day - currDate.day;



	std::string result("The assignment is ");
	// separated for readability
	if (yearDiff < 0) result += "overdue by ";
	else if (yearDiff == 0 && monthDiff < 0) result += "overdue by ";
	else if (yearDiff == 0 && monthDiff == 0 && dayDiff < 0) result += "overdue by ";
	else result += "due in ";

	if (!yearDiff && !monthDiff && !dayDiff) return "The assignment is due today!";

	result += std::format("{} year(s), {} month(s), {} day(s)", abs(yearDiff), abs(monthDiff), abs(dayDiff));
	return result;
}

std::optional<Date> DateLogic::parseCurrDate() {
	auto currDate = std::chrono::utc_clock::now();
	std::string currDateStr = std::format("{:%Y/%m/%d}", std::chrono::floor<std::chrono::days>(currDate));


	size_t firstPos = currDateStr.find_first_of('/');
	std::string year = currDateStr.substr(0, firstPos);
	size_t secondPos = currDateStr.substr(firstPos + 1).find_first_of('/');
	std::string month = currDateStr.substr(firstPos + 1, secondPos);
	std::string day = currDateStr.substr(currDateStr.find_last_of('/')+1);

	try { 
		Date d = { static_cast<int16_t>(std::stoi(year)), static_cast<int16_t>(std::stoi(month)), static_cast<int16_t>(std::stoi(day)) }; 
		return d;
	} catch (const std::invalid_argument& e) { 
		std::println("Current date parse exception (std::exception) : {}", e.what()); 
		return std::nullopt;
	}
}

// yyyy/mm/dd
std::optional<Context> DateLogic::isValidDateFormat(const std::string& input) {
	if (!input.contains('/') || input.size() != 10) return Context::INVALID_FORMAT;
	uint16_t slashCount = 0;
	std::string year, month, day;

	/*
	2026/02/04

	1 month difference
	have priority over overdue assignments


	2026/01/23 -> 2026/02/23 -> 2026/03/23


	*/


	size_t firstPos = input.find_first_of('/');
	year = input.substr(0, firstPos);
	size_t secondPos = input.substr(firstPos + 1).find_first_of('/');
	month = input.substr(firstPos + 1, secondPos);
	day = input.substr(input.find_last_of('/')+1);

	int16_t yearNum, monthNum, dayNum;


	if (year.size() != 4 || month.size() != 2 || day.size() != 2) return Context::INVALID_FORMAT;

	try {
		yearNum = static_cast<int16_t>(std::stoi(year));
		monthNum = static_cast<int16_t>(std::stoi(month));
		dayNum = static_cast<int16_t>(std::stoi(day));
	}
	catch (const std::invalid_argument& e) {
		std::println("Date format verification exception (std) : {}", e.what());
		return Context::EXCEPTION;
	}

	std::optional<Date> currentDateOpt;

	if (currentDateOpt = DateLogic::parseCurrDate(); !currentDateOpt.has_value())
		return Context::EXCEPTION;

	Date currentDate = currentDateOpt.value();
	if (currentDate.year > yearNum || currentDate.month > monthNum || currentDate.day > dayNum)
		return Context::PAST_DUE;

	// no context means it return successfully.
	return std::nullopt;
}