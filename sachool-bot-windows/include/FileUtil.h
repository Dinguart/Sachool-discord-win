#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>

struct FileProperty {
	uint16_t byteSize, byteOffset;
};

//std::unordered_map<std::string, FileProperty> mapFiles(const std::string& fileFormat, )