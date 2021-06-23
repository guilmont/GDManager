#include "gdm.h"

int main(void)
{
	GDM::File gdm("C:/Users/GM Oliveira/Dropbox/Software/GDManager/bin/test.gdm");
	gdm.addDescription("hello", "I'm groot");

	gdm.add("pi", 3.14159f);
	gdm.get<GDM::Data>("pi").addDescription("Number", "This is the value of pi");

	float vec[] = {1.0f, 2.0f, 3.0f};

	//////////////////////////
	GDM::Group& group = gdm.addGroup("veryLongNameForAGroup");

	group.addDescription("Desc", "this is a group");

	GDM::Data& golden = group.add("goldenRatio", 1.61803398875f);
	golden.addDescription("Why?", "This is an important number");
	group.get<GDM::Data>("goldenRatio").addDescription("Really?", "Yeah, it is found a lot in nature");

	GDM::Group& group2 = group.addGroup("cool");
	group2.add("sameVec", vec, { 1, 3 });
	group2.get<GDM::Data>("sameVec").addDescription("desc", "Indeed, but transpose");

	////////////////////////////

	gdm.add("vec", vec, GDM::Shape{3, 1});

	 
	

	gdm.save();



	return EXIT_SUCCESS;
}


