#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <optional>
#include <print>

#include "Constants.h"

#include "../external-includes/CImg.h"

using namespace FileFormatSignatures;
using namespace cimg_library;

// the return circumstance
enum class FileContext {
	EXCEPTION, EARLY_EOF, CONVERSION_NOT_NEEDED
};

struct ConvertedFile {
	std::unique_ptr<CImg<unsigned char>> fileData;
	bool successfullyConverted;
};

std::string changeFileExtension(const std::string& fileName, const std::string& extension);

std::pair<ConvertedFile, std::optional<FileContext>> convertFile(const std::string& fileID, std::ifstream& fileStream, const std::map<std::string, const FileSpecification> sigMap, std::string& inputFileName);

std::pair<bool, std::optional<FileContext>> signatureCheck(std::ifstream& fileStream, const std::string& fileID, const std::map<std::string, const FileSpecification> sigMap);