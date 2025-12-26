#pragma once
#include <string>

bool findString(const std::string& original, const std::string& substr);
// std::pair which returns if string exists, and where in json was located
std::pair<bool, std::string> findAssignmentString(const std::string& original, const std::string& substr);