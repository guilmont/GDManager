#include "gdm.h"

int main(void)
{
	GDM::File gdm;


	gdm.add("pi", 3.14159f);
	gdm["pi"].addDescription("Number", "This is the value of pi");
	

	float vec[] = { 1.0f, 2.0f, 3.0f };
	gdm.add("vec", vec, GDM::Shape{ 3, 1 });


	GDM::Data &group = gdm.addGroup("otherStuff");

	group.addDescription("Desc", "this is a fucking group");

	group.add("goldenRatio", 1.61803398875f);
	group["goldenRatio"].addDescription("Why?", "This is an important number");
	group["goldenRatio"].addDescription("Really?", "Yeah, it is found a lot in nature");

	GDM::Data& group2 = group.addGroup("cool");
	group2.add("sameVec", vec, { 1,3 });
	group2["sameVec"].addDescription("desc", "Indeed, but transpose");
	

	gdm.save();

	return EXIT_SUCCESS;
}