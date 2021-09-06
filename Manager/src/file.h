#pragma once

#include "object.h"

namespace GDM
{

	class File : public Group
	{
	public:
		GDM_API File(void) = default;
		GDM_API File(const fs::path &path);
		GDM_API ~File(void);

		GDM_API void setGroup(Group &&group);
		GDM_API void save(void);
		GDM_API void close(void);

		GDM_API const fs::path &getFilePath(void) const { return filePath; }

	private:
		// helper function to save data into file
		void genDescriptionBuffer(uint64_t headerID, const Description &description);
		void setHeader(const Group &obj);

		// helper functions to load data from file
		std::ifstream gdmFile;
		void loadDescription(Object &obj, uint64_t address);
		void loadGroup(Group *obj, uint64_t numChildren, uint64_t dataAddress, uint64_t descAddress);

		void readFile(void);

		const fs::path filePath; // used to check if saving address is different

	private:
		struct Header // The ordering is important for memory layout :: bigger tp smaller
		{
			char label[MAX_LABEL_SIZE] = {0x00};	  // 4 x 8 bytes
			uint64_t dataAddress, descriptionAddress; // 2 x 8 bytes
			Shape shape;							  // 2 x 8 bytes
			Type type;								  // 4 bytes	   |
			uint32_t pad = 0xcccccccc;				  // 4 bytes     | 8 bytes after padding
		};

		struct HelperData // not to be used directly into file
		{
			uint64_t headerID = 0;

			Compression cps = Compression::NONE;
			uint64_t numBytes = 0;
			uint8_t *ptr = nullptr;

			uint64_t inputFileOffset = 0; // In case the the data was never loaded into RAM
			std::ifstream* gdmFile = nullptr;
		};

		struct HelperDescription // not to be used directly into file
		{
			uint64_t headerID = 0;
			std::vector<char> buffer;
		};

		uint64_t offset = 0;
		std::vector<Header> vHeader;
		std::vector<HelperData> vData;
		std::vector<HelperDescription> vDesc;
	};

}