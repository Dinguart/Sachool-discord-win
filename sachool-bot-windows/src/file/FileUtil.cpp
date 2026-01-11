#include "../../include/FileUtil.h"

std::pair<bool, std::optional<FileContext>> convertFile(const std::string& fileID, std::ifstream& fileStream) {
	// for each extension check if its equal, if it is then we just return the pre-existing image into an embed
	// otherwise, we will convert the file to the specified format that the user requested.


	/*
	@brief
	we are including a constants namespace that defines all the necessary file format attributes, such as the 
	byte offset, byte size, as well as the signature itself in order to avoid re-initialization each time
	for now
	*/
	if (auto exists = signatureCheck(fileStream, fileID); !exists.first) {
		return exists; // so if the file format chosen = file format given then just dont convert.
	}

	if (fileID == "png") {
		
	}
	else if (fileID == "ppm") {

	}
}

std::pair<bool, std::optional<FileContext>> signatureCheck(std::ifstream& fileStream, const std::string& fileID)
{
	const uint16_t byteOffset = signatureMap.at(fileID).offset;
	const uint16_t byteSize = signatureMap.at(fileID).signature.size();

	std::vector<unsigned char> buffer;
	fileStream.seekg(byteOffset, std::ios::beg);
	fileStream.read(reinterpret_cast<char*>(buffer.data()), byteSize);

	// if hit eof early
	if (fileStream.gcount() != byteSize) return { false, FileContext::EARLY_EOF };

	// if the user's file has the same signature as the current option, then that means they dont need a conversion.
	if (std::memcmp(buffer.data(), signatureMap.at(fileID).signature.data(), byteSize) == 0)
		return { false, FileContext::CONVERSION_NOT_NEEDED };

	return { true, std::nullopt };
}
