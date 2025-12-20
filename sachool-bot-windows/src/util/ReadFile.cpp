#include "../../include/ReadFile.h"

std::optional<fileMap> readFile(const std::string file) {
	std::unordered_map<std::string, std::string> map;
	std::ifstream fileStream(file);
	std::string line;

	if (!fileStream.is_open()) {
		printf("ERROR: Could not open specified file, please make sure the file location is correct.\n");
		return std::nullopt;
	}

	while (getline(fileStream, line)) {
		size_t splitter = line.find('=');
		std::string key = line.substr(0, splitter);
		std::string value = line.substr(splitter + 1);
		map[key] = value;
	}

	return map;
}