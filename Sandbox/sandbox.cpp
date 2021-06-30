#include "gdm.h"

using namespace GDM;

int main(void)
{
	
	File  arq;

	Group& hoxa = arq.addGroup("hoxa");
	
	Group& hic = hoxa.addGroup("hic");
	hic.add<uint32_t>("CoordinateAtZero", 49996001);
	hic.add<uint32_t>("resolution", 4000);

	Group& cell = hoxa.addGroup("cellLines");
	cell.addDescription("test", "123");

	Group& t12 = cell.addGroup("T1-T2");

	uint32_t loc[2] = { 52014370, 52321145 };
	t12.add<uint32_t>("locations", loc, { 2,1 });

	float dist[2] = { 0.347f, 0.04f };
	t12.add<float>("distance", dist, { 2,1 });
	t12.get<Data>("distance").addDescription("Unis", "microns");

	float D[4] = { 1.563f, 0.186f, 1.927f, 0.316f };
	t12.add<float>("D", D, { 2,2 });

	float A[4] = { 0.251f, 0.027f, 0.334f, 0.033f };
	t12.add<float>("A", A, { 2,2 });

	arq.save("C:/Users/GM Oliveira/Desktop/hello.gdm");


	return EXIT_SUCCESS;
}

//
//const char* name = "test.gdm";
//const char* name2 = "test2.gdm";
//
//{
//	GDM::Group gdm;
//	gdm.addDescription("hello", "I'm groot");
//
//	gdm.add("pi", 3.14159f);
//	gdm.get<GDM::Data>("pi").addDescription("Number", "This is the value of pi");
//
//	float vec[] = { 1.0f, 2.0f, 3.0f };
//	gdm.add("vec", vec, GDM::Shape{ 3, 1 });
//
//	GDM::File arq(std::move(gdm));
//	arq.save(name);
//}
//
//{
//	GDM::File gdm(name);
//
//	gdm.get<GDM::Data>("pi").rename("PI");
//
//	GDM::Group& group = gdm.addGroup("veryLongNameForAGroup");
//
//	group.addDescription("Desc", "this is a group");
//
//	GDM::Data& golden = group.add("goldenRatio", 1.61803398875f);
//	golden.addDescription("Why?", "This is an important number");
//	group.get<GDM::Data>("goldenRatio").addDescription("Really?", "Yeah, it is found a lot in nature");
//
//	GDM::Group& group2 = group.addGroup("cool");
//
//	uint32_t vc[] = { 1, 2, 3 };
//	group2.add("sameVec", vc, { 1, 3 });
//	group2.get<GDM::Data>("sameVec").addDescription("desc", "Indeed, but transpose");
//
//	gdm.save(name2);
//
//	// Just printing some stuff
//
//	for (auto& [label, desc] : gdm.descriptions())
//		GDM::pout(label, "::", desc);
//
//	const float* vec = gdm.get<GDM::Data>("vec").getArray<float>();
//	GDM::pout(vec[0], vec[1], vec[2]);
//
//	const float* vec2 = gdm.get<GDM::Data>("vec").getArray<float>();
//	GDM::pout(vec2[0], vec2[1], vec2[2]);
//}