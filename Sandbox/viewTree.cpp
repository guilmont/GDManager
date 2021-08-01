#include "GDManager.h"

static void printGroup(std::string step, GDM::Group &obj)
{
	for (auto &[label, ptr] : obj.children())
	{
		GDM::Type type = ptr->getType();

		if (type == GDM::Type::GROUP)
		{
			GDM::Group *group = reinterpret_cast<GDM::Group *>(ptr);
			printf("\n%s%s :: GROUP {%d} >> ", step.c_str(), label.c_str(), group->getNumChildren());
		}

		else
		{
			GDM::Data *data = reinterpret_cast<GDM::Data *>(ptr);
			GDM::Shape shape = data->getShape();

			printf("%s%s :: ", step.c_str(), label.c_str());

			switch (data->getType())
			{
			case GDM::Type::INT32:
				printf("DATA (INT32 {%d,%d}) >> ", shape.height, shape.width);
				break;
			case GDM::Type::INT64:
				printf("DATA (INT64 {%d,%d}) >> ", shape.height, shape.width);
				break;
			case GDM::Type::UINT8:
				printf("DATA (UINT8 {%d,%d}) >> ", shape.height, shape.width);
				break;
			case GDM::Type::UINT16:
				printf("DATA (UINT16 {%d,%d}) >> ", shape.height, shape.width);
				break;
			case GDM::Type::UINT32:
				printf("DATA (UINT32 {%d,%d}) >> ", shape.height, shape.width);
				break;
			case GDM::Type::UINT64:
				printf("DATA (UINT64 {%d,%d}) >> ", shape.height, shape.width);
				break;
			case GDM::Type::FLOAT:
				printf("DATA (FLOAT {%d,%d}) >> ", shape.height, shape.width);
				break;
			case GDM::Type::DOUBLE:
				printf("DATA (DOUBLE {%d,%d}) >> ", shape.height, shape.width);
				break;
			default:
				printf("UNKNOWN >> UNKNOWN\n");
				continue;
				break;
			}
		}

		int64_t nDesc = ptr->descriptions().size();

		if (nDesc == 0)
			printf("No description\n");
		else
		{
			for (auto &[name, desc] : ptr->descriptions())
				printf("%s: %s %s", name.c_str(), desc.c_str(), (--nDesc == 0 ? "\n" : "== "));
		}

		if (ptr->getType() == GDM::Type::GROUP)
			printGroup(step + "\t", *reinterpret_cast<GDM::Group *>(ptr));
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("ERROR: Missing GDM-file\n\tUsage: ./GDMTree $(filename)\n\n");
		return EXIT_FAILURE;
	}

	GDM::pout("Welcome to GDManager!!");

	GDM::File gdm(argv[1], GDM::State::READ);
	printGroup("  ", gdm);

	return EXIT_SUCCESS;
}
