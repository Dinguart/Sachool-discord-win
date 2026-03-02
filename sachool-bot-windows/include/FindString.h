#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <print>

bool findString(const std::string& original, const std::string& substr);

std::pair<std::string, std::string> SeparateByDelimPair(const std::string& str, const char delim);