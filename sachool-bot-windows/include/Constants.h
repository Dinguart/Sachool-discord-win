#pragma once
#include <map>
#include <string>
#include <vector>

namespace FileFormatSignatures {
	
	enum ByteOffsets {
		PNG_BYTE_OFFSET = 0
	};

	// the signature contains the ByteSizes 
	struct FileSpecification {
		std::vector<unsigned char> signature;
		ByteOffsets offset;
	};

	// file specifications for each file format.
	const FileSpecification pngSpec = { {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, PNG_BYTE_OFFSET };

	const std::map<std::string, const FileSpecification> signatureMap = {
		{"png", pngSpec}
	};
}