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

std::pair<bool, std::string> findAssignmentString(const std::string& original, const std::string& substr)
{
	//
}


