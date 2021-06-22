#pragma once

#include "object.h"

namespace GDM
{

	class File : public Data
	{
	public:
		GDM_API File(void) = default;
		GDM_API ~File(void) = default;

		GDM_API void save(void);

	private:

		struct Header  // This is the bytes layout for the file format
		{
			char label[64] = { 0 };
			Type type;
			Shape shape;
			uint64_t dataAddress;
			uint32_t descriptionSize;
			uint64_t descriptionAddress;
		};

		using Group = std::vector<Header>;


	private:

		std::vector<Group> data;

		void setHeader(const Data& obj);



	};

}