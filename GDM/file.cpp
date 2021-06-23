#include "file.h"

namespace GDM
{
	GDM_API void File::load(void)
	{
		
	}


	void File::save(void)
	{
		// Saving data about the root directory
		Header head;

		const std::string& label = getLabel();
		std::copy(label.begin(), label.end(), head.label);

		head.type = Type::GROUP;
		head.shape = { 1, getNumChildren() };
		head.dataAddress = strlen(GDM_SIGNATURE) + sizeof(Header);
		head.descriptionAddress = 0xffffffffffffffff;

		if (m_description.size() > 0)
			genDescriptionBuffer(0, m_description);

		vHeader.emplace_back(std::move(head));

		// Preparing all the other objects
		setHeader(*this);

		// Inserting description offset into header
		uint64_t headerOffset = strlen(GDM_SIGNATURE) + vHeader.size() * sizeof(Header);
		for (auto& desc : vDesc)
		{
			vHeader[desc.headerID].descriptionAddress = headerOffset;
			headerOffset += desc.buffer.size() + sizeof(uint32_t); // buffer is in bytes
		}

		// Compressing data and inserting offset into header
		for (auto& data : vData)
		{
			switch (data.cps)
			{
			case Compression::NONE:
				break;
			case Compression::ZIP:
				break;
			case Compression::LZW:
				break;
			}

			vHeader[data.headerID].dataAddress = headerOffset;
			headerOffset += data.numBytes + sizeof(Compression) + sizeof(uint64_t); // uint64_t holds number of bytes
		}


		// It is time to send everything into a file
		std::ofstream arq(filepath, std::fstream::binary);

		arq.write(GDM_SIGNATURE, strlen(GDM_SIGNATURE));

		for (const Header& head : vHeader)
			arq.write(reinterpret_cast<const char*>(&head), sizeof(Header));

		for (const HelperDescription& desc : vDesc)
		{
			uint32_t sz = static_cast<uint32_t>(desc.buffer.size());
			arq.write(reinterpret_cast<const char*>(&sz), sizeof(uint32_t));
			arq.write(desc.buffer.data(), desc.buffer.size()*sizeof(char));
		}


		for (const HelperData& data : vData)
		{
			arq.write(reinterpret_cast<const char*>(&data.cps), sizeof(Compression));
			arq.write(reinterpret_cast<const char*>(&data.numBytes), sizeof(uint64_t));
			arq.write(reinterpret_cast<const char*>(data.ptr), data.numBytes);
		}

		arq.close();
	}

	void File::genDescriptionBuffer(uint64_t headerID, const Description& description)
	{
		HelperDescription hds;
		hds.headerID = headerID;

		for (auto& [label, desc] : description)
		{
			std::copy(label.begin(), label.end(), std::back_inserter(hds.buffer));
			hds.buffer.emplace_back('\0');
			std::copy(desc.begin(), desc.end(), std::back_inserter(hds.buffer));
			hds.buffer.emplace_back('\0');
		}

		vDesc.emplace_back(std::move(hds));
	}

	void File::setHeader(const Group &obj)
	{
		uint64_t 
			counter = vHeader.size(),
			groupSize = obj.getNumChildren();

		vHeader.resize(counter + groupSize);

		std::vector<const Object *> vGroup; // used to generate header for nested groups
		
		for (auto &[label, ptr] : obj.objs)
		{
			Header var;
			std::copy(label.begin(), label.end(), var.label);
			var.type = ptr->getType();

			
			if (ptr->getType() == Type::GROUP)
			{
				const Group &gp = *reinterpret_cast<Group *>(ptr);
				var.shape = {1, gp.getNumChildren()};
				var.dataAddress = strlen(GDM_SIGNATURE) + vHeader.size() * sizeof(Header); // later
				var.descriptionAddress = 0xffffffffffffffff; // later

				if (gp.m_description.size() > 0)
					genDescriptionBuffer(counter, gp.m_description);


				// So we can run separately later
				vGroup.push_back(ptr);
			}

			else
			{
				const Data &dt = *reinterpret_cast<Data *>(ptr);

				var.shape = dt.shape;

				// if data consumes less than 8 bytes, I'm going to save it here for efficiency
				if (dt.numBytes < 8)
				{
					var.dataAddress = 0;
					uint8_t *v = reinterpret_cast<uint8_t *>(&var.dataAddress);
					std::copy(dt.buffer, dt.buffer + dt.numBytes, v);
				}
				else
				{
					var.dataAddress = 0xffffffffffffffff; // will be set later

					HelperData hdt;
					hdt.numBytes = dt.numBytes;
					hdt.ptr = dt.buffer;
					hdt.headerID = counter;

					vData.emplace_back(std::move(hdt));

				}
					var.descriptionAddress = 0xffffffffffffffff; //  will be set later

					if (dt.m_description.size() > 0)
						genDescriptionBuffer(counter, dt.m_description);
			}

			// Appending header to output variable
			vHeader[counter] = std::move(var);
			counter++;
		}

		// Submitting groups within this group
		for (const Object* ptr : vGroup)
		{
			const Group& group = *reinterpret_cast<const Group*>(ptr);
			setHeader(group);
		}
	}

}