#pragma once
#include <map>
#include <string>
#include <vector>

namespace FileFormatSignatures {
	
	enum ByteOffsets {
		PNG_BYTE_OFFSET = 0,
		JPEG_BYTE_OFFSET = 0,
		PPM_BYTE_OFFSET = 0
	};

	// the signature contains the ByteSizes 
	struct FileSpecification {
		std::vector<unsigned char> signature;
		ByteOffsets offset;
	};

	// file specifications for each file format.
	const FileSpecification pngSpec = { {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, PNG_BYTE_OFFSET };
	const FileSpecification jpegSpec = { {0xFF, 0xD8, 0xFF }, JPEG_BYTE_OFFSET };
	const FileSpecification ppmSpec = { {'P', '6'}, PPM_BYTE_OFFSET };

	const std::map<std::string, const FileSpecification> signatureMap = {
		{"png", pngSpec},
		{"jpeg", jpegSpec},
		{"ppm", ppmSpec}
	};
}