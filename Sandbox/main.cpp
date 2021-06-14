#include "object.h"

int main(void)
{
	float val[] = {1.0f, 2.0f,3.0f};

	GDM::Data root;

	GDM::Data& folder = root.addGroup("folder");

	folder.add("value", val, GDM::Shape{ 3,1 });
	folder.add("hello", val, GDM::Shape{ 3,1 });

	root["folder"].remove("value");

	root.remove("folder");
	
	return EXIT_SUCCESS;
}