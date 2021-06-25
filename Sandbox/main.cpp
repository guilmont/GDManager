#include "gdm.h"

int main(void)
{
#ifdef WIN32
	const char* name = "C:/Users/GM Oliveira/Dropbox/Software/GDManager/bin/test.gdm";
	const char* name2 = "C:/Users/GM Oliveira/Dropbox/Software/GDManager/bin/test2.gdm";
#else
	// const char* name = "/mnt/c/Users/GM Oliveira/Dropbox/Software/GDManager/bin/test.gdm";
	const char* name = "test.gdm";
#endif

	{
		GDM::Group gdm;
		gdm.addDescription("hello", "I'm groot");

		gdm.add("pi", 3.14159f);
		gdm.get<GDM::Data>("pi").addDescription("Number", "This is the value of pi");

		float vec[] = { 1.0f, 2.0f, 3.0f };
		gdm.add("vec", vec, GDM::Shape{ 3, 1 });

		GDM::File arq(std::move(gdm));
		arq.save(name);

	}

	{
		GDM::File gdm(name);

		GDM::Group& group = gdm.addGroup("veryLongNameForAGroup");

		group.addDescription("Desc", "this is a group");

		GDM::Data& golden = group.add("goldenRatio", 1.61803398875f);
		golden.addDescription("Why?", "This is an important number");
		group.get<GDM::Data>("goldenRatio").addDescription("Really?", "Yeah, it is found a lot in nature");

		GDM::Group& group2 = group.addGroup("cool");
		float vec[] = { 1.0f, 2.0f, 3.0f };

		group2.add("sameVec", vec, { 1, 3 });
		group2.get<GDM::Data>("sameVec").addDescription("desc", "Indeed, but transpose");

		gdm.save(name2);
	}

	////////////////////////////




	/*for (auto& [label, desc] : gdm.descriptions())
		GDM::pout(label, "::", desc);


	const float* vec = gdm.get<GDM::Data>("vec").getArray<float>();
	GDM::pout(vec[0], vec[1], vec[2]);

	const float* vec2 = gdm.get<GDM::Data>("vec").getArray<float>();
	GDM::pout(vec2[0], vec2[1], vec2[2]);*/

	return EXIT_SUCCESS;
}


