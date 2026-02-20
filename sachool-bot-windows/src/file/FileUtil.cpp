#include "../../include/FileUtil.h"

std::string getImageName(std::string& imageUrl) {
	size_t lastSlash = imageUrl.find_last_of('/');
	size_t queryPos = imageUrl.find_first_of('?', lastSlash);

	if (lastSlash == std::string::npos)
		return "image";

	if (queryPos == std::string::npos)
		return imageUrl.substr(lastSlash + 1);

	return imageUrl.substr(lastSlash + 1, queryPos - lastSlash - 1);
}

std::string changeImageExtension(const std::string& imageName, const std::string& extension)
{
	size_t extPos = imageName.find_last_of('.');
	return (extPos != std::string::npos) ?
		imageName.substr(0, extPos) + "." + extension :
		imageName + "." + extension;
}

std::pair<ConvertedImage, std::optional<FileContext>> convertImage(const std::string& fileID, std::ifstream& fileStream, const std::map<std::string, const FileSpecification> imgMap, std::string& inputImageName) {
	// for each extension check if its equal, if it is then we just return the pre-existing image into an embed
	// otherwise, we will convert the file to the specified format that the user requested.


	/*
	@brief
	we are including a constants namespace that defines all the necessary file format attributes, such as the 
	byte offset, byte size, as well as the signature itself in order to avoid re-initialization each time
	for now
	*/
	if (auto exists = signatureCheck(fileStream, fileID, imgMap); !exists.first) {
		return { {nullptr, false }, exists.second}; // so if the file format chosen = file format given then just dont convert.
	}
	// otherwise, just convert to the chosen format.
	/* may have to utilize fourier transforms*/


	// dont allow pdf to image conversion, but do display the pdf somehow.
	try {
		CImg<unsigned char> img(inputImageName.c_str());

		// make out filename
		std::string outputImageName = changeImageExtension(inputImageName, fileID);

		if (fileID == "jpg" || fileID == "jpeg") {
			img.save(outputImageName.c_str(), -1, 90);
		}
		else {
			img.save(outputImageName.c_str());
		}
		// now that we converted the image, we should create an embed 
		// or should we change the specification inside of the database?
		// figure out the architecture for this.
		// TODO: do something with the converted image.
		inputImageName = outputImageName;
		return { {std::make_unique<CImg<unsigned char>>(img), true}, std::nullopt};
	}
	catch (const CImgException& e) {
		std::println("File conversion exception (CImg) : {}", e.what());
	}
	catch (const std::exception& e) {
		std::println("File conversion exception (std) : {}", e.what());
	}
	return { {nullptr, false}, FileContext::EXCEPTION };
}

std::pair<bool, std::optional<FileContext>> signatureCheck(std::ifstream& fileStream, const std::string& fileID, const std::map<std::string, const FileSpecification> sigMap)
{
	try {
		if (!sigMap.contains(fileID))
			return { false, FileContext::FILETYPE_NOT_FOUND };

		const uint16_t targetByteOffset = sigMap.at(fileID).offset;
		const uint16_t targetByteSize = sigMap.at(fileID).signature.size();
		uint16_t maxSignatureSize = 0;
		for (const auto& item : sigMap) {
			maxSignatureSize = std::max(static_cast<uint16_t>(item.second.signature.size()), maxSignatureSize);
		}

		std::vector<unsigned char> buffer(maxSignatureSize);
		fileStream.seekg(targetByteOffset, std::ios::beg);
		fileStream.read(reinterpret_cast<char*>(buffer.data()), maxSignatureSize);

		// if hit eof early
		if (fileStream.gcount() < targetByteSize) return { false, FileContext::EARLY_EOF };

		// if the user's file has the same signature as the current option, then that means they dont need a conversion.
		if (std::memcmp(buffer.data(), sigMap.at(fileID).signature.data(), targetByteSize) == 0)
			return { false, FileContext::CONVERSION_NOT_NEEDED };

		/* to check if the specified exists in the map. */
		bool exists = false;
		for (const auto& signature : sigMap) {
			const auto& sig = signature.second.signature;
			if (buffer.size() >= sig.size() && std::memcmp(buffer.data(), sig.data(), sig.size()) == 0) {
				exists = true;
				break;
			}
		}
		if (!exists) return { false, FileContext::FILETYPE_NOT_FOUND };

		return { true, std::nullopt };
	}
	catch (const std::exception& e) {
		std::println("File signature verification exception (std) : {}", e.what());
		return { false, FileContext::EXCEPTION };
	}
}
