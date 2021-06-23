#pragma once

#include "object.h"

namespace GDM
{

	enum class Compression : uint32_t
	{
		NONE,
		ZIP,
		LZW
	};

	class File : public Group
	{
	public:
		GDM_API File(const std::string& path) : filepath(path) {}
		GDM_API ~File(void) = default;

		GDM_API void load(void);
		GDM_API void save(void);

	private:
		void genDescriptionBuffer(uint64_t headerID, const Description& description);
		void setHeader(const Group& obj);


		std::string filepath;

	private:
		struct Header // The ordering is important for memory layout :: bigger tp smaller
		{
			char label[MAX_LABEL_SIZE] = {0x00};        // 4 x 8 bytes 
			uint64_t dataAddress, descriptionAddress;   // 2 x 8 bytes
			Shape shape;                                // 2 x 4 bytes 
			Type type;                                  // 4 bytes	   |
			uint32_t pad = 0xcccccccc;                  // 4 bytes     | 8 bytes after padding
		};


		struct HelperData // not to be used directly into file
		{
			uint64_t headerID;
			
			Compression cps = Compression::NONE;
			uint64_t numBytes;
			uint8_t* ptr = nullptr;
		};

		struct HelperDescription // not to be used directly into file
		{
			uint64_t headerID;
			std::vector<char> buffer;
		};

		std::vector<Header> vHeader;
		std::vector<HelperData> vData;
		std::vector<HelperDescription> vDesc;


	};

}