#include "file.h"

namespace GDM
{

	static 	uint32_t getDescSize(const std::map<std::string, std::string>& descriptions)
	{
		uint32_t  descSize = 0;
		for (auto& [label, desc] : descriptions)
		{
			descSize += uint32_t(label.size()) + 1;
			descSize += uint32_t(desc.size()) + 1;
		}

		return descSize;
	}


	void File::setHeader(const Data& obj)
	{
		Header head;

		// Copying label
		const std::string& label = obj.getLabel();
		std::copy(label.begin(), label.end(), head.label);

		// group type
		head.type = obj.getType();

		// Setup shape and address
		if (head.type == Type::GROUP)
		{
			head.shape = { 1, obj.getNumChildren() };
			head.dataAddress = 0;
		}
		else
		{
			head.shape = obj.getShape();
			if (head.shape.width * head.shape.height == 1)
			{
				std::copy(obj.buffer, obj.buffer + obj.numBytes, &head.dataAddress);
			}

			else
				head.dataAddress = 0;
		}
	}

	void File::save(void)
	{

		

		

	


	
	}

}