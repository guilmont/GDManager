#include "gdm.h"

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
				printf("DATA (INT32 {%d,%d}) >> ", shape.width, shape.height);
				break;
			case GDM::Type::UINT8:
				printf("DATA (UINT8 {%d,%d}) >> ", shape.width, shape.height);
				break;
			case GDM::Type::UINT16:
				printf("DATA (UINT16 {%d,%d}) >> ", shape.width, shape.height);
				break;
			case GDM::Type::UINT32:
				printf("DATA (UINT32 {%d,%d}) >> ", shape.width, shape.height);
				break;
			case GDM::Type::FLOAT:
				printf("DATA (FLOAT {%d,%d}) >> ", shape.width, shape.height);
				break;
			case GDM::Type::DOUBLE:
				printf("DATA (DOUBLE {%d,%d}) >> ", shape.width, shape.height);
				break;
			case GDM::Type::STRING:
				printf("DATA (STRING {%d,%d}) >> ", shape.width, shape.height);
				break;
			default:
				printf("UNKNOWN >> ");
				break;
			}
		}

		int32_t nDesc = ptr->descriptions().size();

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
		printf("ERROR: Missing GDM-file\n\tUsage: ./ViewTree $(filename)\n\n");
		return EXIT_FAILURE;
	}

	printf("Welcome to GDManager\nFile: %s\n\n", argv[1]);

	GDM::File gdm(argv[1]);
	printGroup("  ", gdm);

	return EXIT_SUCCESS;
}
