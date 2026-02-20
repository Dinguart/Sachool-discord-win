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
	EXCEPTION, EARLY_EOF, CONVERSION_NOT_NEEDED, FILETYPE_NOT_FOUND
};

struct ConvertedImage {
	std::unique_ptr<CImg<unsigned char>> fileData;
	bool successfullyConverted;
};

std::string getImageName(std::string& imageUrl);

std::string changeImageExtension(const std::string& imageName, const std::string& extension);

std::pair<ConvertedImage, std::optional<FileContext>> convertImage(const std::string& fileID, std::ifstream& fileStream, const std::map<std::string, const FileSpecification> imgMap, std::string& inputImageName);

std::pair<bool, std::optional<FileContext>> signatureCheck(std::ifstream& fileStream, const std::string& fileID, const std::map<std::string, const FileSpecification> sigMap);