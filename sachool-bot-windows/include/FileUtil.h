#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <optional>

#include "Constants.h"

extern const std::map<std::string, FileFormatSignatures::FileSpecification> signatureMap;

// the return circumstance
enum class FileContext {
	EXCEPTION, EARLY_EOF, CONVERSION_NOT_NEEDED
};

std::pair<bool, std::optional<FileContext>> convertFile(const std::string& fileID, std::ifstream& fileStream);

std::pair<bool, std::optional<FileContext>> signatureCheck(std::ifstream& fileStream, const std::string& fileID);