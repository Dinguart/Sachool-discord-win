#include "../WhenDue.h"

Date dateParse(const std::string& date) {
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

std::string whenDue(const std::string& date) {
	auto today = std::chrono::utc_clock::now();
	auto currentDate = std::chrono::floor<std::chrono::days>(today);
	std::string dateStr = std::format("{:%Y/%m/%d}", currentDate);

	Date dueDate = dateParse(date);
	Date currDate = dateParse(dateStr);

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