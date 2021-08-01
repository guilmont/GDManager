#include "file.h"

namespace GDM
{
	File::File(const fs::path &path, State state) : filePath(path), state(state)
	{
		if (state == State::READ)
			readFile();
	}

	void File::readFile(void)
	{
		if (!fs::exists(filePath))
		{
			pout("ERROR: File doesn't exist!!", __FUNCTION__);
			exit(EXIT_FAILURE);
		}
			
		pout("Openning file:", filePath.string(), "\n");

		gdmFile.open(filePath, std::fstream::binary);
		assert(gdmFile.good());

		offset = strlen(GDM_SIGNATURE);
		char buffer[sizeof(Header)] = {0x00};
		gdmFile.read(buffer, offset);

		// Just ot make sure we are dealing with the proper type of file
		assert(std::string(buffer).compare(GDM_SIGNATURE) == 0);

		// Start by reading the root directory
		gdmFile.seekg(offset);
		gdmFile.read(buffer, sizeof(Header));
		offset += sizeof(Header);

		Header root = *reinterpret_cast<Header *>(buffer);
		loadGroup(this, root.shape.height, root.dataAddress, root.descriptionAddress);
	}

	void File::saveFile(void)
	{
		if (fs::exists(filePath))
		{
			pout("ERROR: File already exists! :: function:", __FUNCTION__);
			exit(EXIT_FAILURE);
		}

		// Clearing data, so we can save fresh
		vHeader.clear();
		vData.clear();
		vDesc.clear();

		// Saving data about the root directory
		Header head;

		const std::string &label = getLabel();
		std::copy(label.begin(), label.end(), head.label);

		head.type = Type::GROUP;
		head.shape = {getNumChildren(), 1};
		head.dataAddress = strlen(GDM_SIGNATURE) + sizeof(Header);
		head.descriptionAddress = NO_DESCRIPTION;

		if (m_description.size() > 0)
			genDescriptionBuffer(0, m_description);

		vHeader.emplace_back(std::move(head));

		// Preparing all the other objects
		setHeader(*this);

		// Inserting description offset into header
		uint64_t headerOffset = strlen(GDM_SIGNATURE) + vHeader.size() * sizeof(Header);
		for (auto &desc : vDesc)
		{
			vHeader[desc.headerID].descriptionAddress = headerOffset;
			headerOffset += desc.buffer.size() + sizeof(uint32_t); // buffer is in bytes
		}

		// Compressing data and inserting offset into header
		for (auto &data : vData)
		{
			// TODO: update when compression is implemented
			assert(data.cps == Compression::NONE);

			vHeader[data.headerID].dataAddress = headerOffset;
			headerOffset += data.numBytes + sizeof(Compression) + sizeof(uint64_t); // uint64_t holds number of bytes
		}

		// It is time to send everything into a file
		pout("Saving data:", filePath.string());
		std::ofstream output(filePath, std::ofstream::binary | std::ofstream::trunc);

		output.write(GDM_SIGNATURE, strlen(GDM_SIGNATURE));

		for (const Header &head : vHeader)
			output.write(reinterpret_cast<const char *>(&head), sizeof(Header));

		for (const HelperDescription &desc : vDesc)
		{
			uint32_t sz = static_cast<uint32_t>(desc.buffer.size());
			output.write(reinterpret_cast<const char *>(&sz), sizeof(uint32_t));
			output.write(desc.buffer.data(), desc.buffer.size() * sizeof(char));
		}

		for (const HelperData &data : vData)
		{
			if (data.ptr) // Data is loaded into RAM
			{
				output.write(reinterpret_cast<const char *>(&data.cps), sizeof(Compression));
				output.write(reinterpret_cast<const char *>(&data.numBytes), sizeof(uint64_t));
				output.write(reinterpret_cast<const char *>(data.ptr), data.numBytes);
			}
			else
			{
				// TODO: Optimize this code
				uint64_t size = data.numBytes + sizeof(uint64_t) + sizeof(Compression);
				char *buf = new char[size];

				gdmFile.seekg(data.inputFileOffset);
				gdmFile.read(buf, size);
				output.write(buf, size);

				delete[] buf;
			}
		}

		output.close();
	}

	void File::setGroup(Group &&group)
	{
		assert(m_children.size() == 0);
		assert(m_description.size() == 0);

		m_children = std::move(group.m_children);
		m_description = std::move(group.m_description);

		group.m_children.clear();
		group.m_description.clear();
	}

	void File::close(void)
	{
		if (state == State::WRITE)
			saveFile();
		
		if (gdmFile.is_open())
			gdmFile.close();

	}

	void File::genDescriptionBuffer(uint64_t headerID, const Description &description)
	{
		HelperDescription hds;
		hds.headerID = headerID;

		for (auto &[label, desc] : description)
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

		for (auto &[label, ptr] : obj.m_children)
		{
			Header var;
			std::copy(label.begin(), label.end(), var.label);
			var.type = ptr->getType();

			if (ptr->getType() == Type::GROUP)
			{
				const Group &gp = *reinterpret_cast<Group *>(ptr);

				var.shape = {gp.getNumChildren(), 1};
				var.dataAddress = strlen(GDM_SIGNATURE) + (vHeader.size() + 1) * sizeof(Header);
				var.descriptionAddress = NO_DESCRIPTION; // later

				if (gp.m_description.size() > 0)
					genDescriptionBuffer(vHeader.size(), gp.m_description);

				// Appending header to output variable
				vHeader.emplace_back(std::move(var));

				// creating header for this group
				setHeader(gp);
			}

			else
			{
				const Data &dt = *reinterpret_cast<Data *>(ptr);

				var.shape = dt.shape;

				// if data consumes less than 8 bytes, I'm going to save it here for efficiency
				if (dt.numBytes <= sizeof(uint64_t))
				{
					var.dataAddress = 0;
					uint8_t *v = reinterpret_cast<uint8_t *>(&var.dataAddress);
					std::copy(dt.buffer, dt.buffer + dt.numBytes, v);
				}
				else
				{
					var.dataAddress = NO_DESCRIPTION; // will be set later, a generic value for now

					// TODO: Update when compression is implemented
					HelperData hdt;
					hdt.cps = Compression::NONE;
					hdt.numBytes = dt.numBytes;
					hdt.headerID = vHeader.size();

					if (dt.buffer)
						hdt.ptr = dt.buffer;
					else
						hdt.inputFileOffset = dt.offset;

					vData.emplace_back(std::move(hdt));
				}
				var.descriptionAddress = NO_DESCRIPTION; //  if necessary, it will be set later

				if (dt.m_description.size() > 0)
					genDescriptionBuffer(vHeader.size(), dt.m_description);

				// Appending header to output variable
				vHeader.emplace_back(std::move(var));
			}
		}
	}

	void File::loadDescription(Object &obj, uint64_t address)
	{
		// First we get the total number of bytes describing this object
		uint32_t descSize;
		gdmFile.seekg(address);
		gdmFile.read(reinterpret_cast<char *>(&descSize), sizeof(uint32_t));

		char *data = new char[descSize];

		gdmFile.seekg(address + sizeof(uint32_t));
		gdmFile.read(data, descSize);

		// Now we load all labels and descriptions
		uint64_t offset = 0;
		while (offset < descSize)
		{
			std::string label(data + offset);
			offset += label.size() + 1;
			std::string desc(data + offset);
			offset += desc.size() + 1;

			obj.m_description.emplace(std::move(label), std::move(desc));
		}

		delete[] data;
	}

	void File::loadGroup(Group *obj, uint32_t numChildren, uint64_t dataAddress, uint64_t descAddress)
	{
		if (descAddress != NO_DESCRIPTION)
			loadDescription(*obj, descAddress);

		offset = dataAddress;
		char buffer[sizeof(Header)] = {0x00};

		for (uint32_t k = 0; k < numChildren; k++)
		{
			gdmFile.seekg(offset);
			gdmFile.read(buffer, sizeof(Header));
			offset += sizeof(Header);

			Header &loc = *reinterpret_cast<Header *>(buffer);

			if (loc.type == Type::GROUP)
			{
				Group *gp = new Group(loc.label);
				gp->parent = obj;
				loadGroup(gp, loc.shape.height, loc.dataAddress, loc.descriptionAddress);

				obj->m_children.emplace(loc.label, std::move(gp));
			}
			else
			{
				Data *dt = new Data(loc.label, loc.type);
				dt->parent = obj;
				dt->shape = loc.shape;
				dt->numBytes = uint64_t(dt->shape.width) * uint64_t(dt->shape.height) * getNumBytes(dt->type);

				if (dt->numBytes <= sizeof(uint64_t))
				{
					dt->buffer = new uint8_t[dt->numBytes];
					memcpy(dt->buffer, buffer + offsetof(Header, dataAddress), dt->numBytes);
				}
				else
				{
					dt->offset = loc.dataAddress;
					dt->gdmFile = &gdmFile;
				}

				if (loc.descriptionAddress != NO_DESCRIPTION)
					loadDescription(*dt, loc.descriptionAddress);

				obj->m_children.emplace(loc.label, std::move(dt));
			}
		} // loop-children
	}

}