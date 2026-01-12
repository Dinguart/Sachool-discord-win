#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <optional>

#include "Constants.h"

using namespace FileFormatSignatures;

// the return circumstance
enum class FileContext {
	EXCEPTION, EARLY_EOF, CONVERSION_NOT_NEEDED
};

std::pair<bool, std::optional<FileContext>> convertFile(const std::string& fileID, std::ifstream& fileStream, const std::map<std::string, const FileSpecification> sigMap);

std::pair<bool, std::optional<FileContext>> signatureCheck(std::ifstream& fileStream, const std::string& fileID, const std::map<std::string, const FileSpecification> sigMap);