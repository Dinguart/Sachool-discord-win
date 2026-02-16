#include "../../include/FindString.h"

bool findString(const std::string& original, const std::string& substr)
{
	size_t substrIndex = 0;

	for (size_t i = 0; i < original.size(); i++) {
		if (original[i] == substr[substrIndex]) substrIndex++;
		else substrIndex = 0;
		if (substrIndex == substr.size() - 1) return true;
	}
	return false;
}

std::pair<std::string, std::string> SeparateByDelimPair(const std::string& str, const char delim) {
	size_t pos = str.find(delim);

	if (pos == std::string::npos) {
		return { str, "" };
	}
	
	return { str.substr(0, pos), str.substr(pos + 1) };
}
