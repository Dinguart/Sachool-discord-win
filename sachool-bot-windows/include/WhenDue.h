#pragma once
#include <string>
#include <chrono>
#include <format>

struct Date {
	uint16_t year, month, day;
};

Date dateParse(const std::string& date);

std::string whenDue(const std::string& date);