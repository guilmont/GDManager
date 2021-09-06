#include "GDManager.h"
#include <cmath>

using namespace GDM;

#if _WIN32
#include <corecrt_math_defines.h>
#endif

int main(void)
{
	fs::path filename = "../../test.gdm";
	{
		File arq(filename);
		arq.clear();

		Data &pi = arq.add<double>("this is pi", M_PI);

		Group &hoxa = arq.addGroup("hoxa");

		hoxa.addDescription("test", "hello hoxa");

		uint32_t loc[2] = {52014370, 52321145};
		hoxa.add<uint32_t>("locations", loc, {1, 2}).addDescription("hello", "this is a description");
		
		Group &hic = arq.addGroup("hoxa/hic");
		hic.addDescription("hi", "yet another");
		hic.add<uint32_t>("CoordinateAtZero", 49996001);
		hic.add<uint32_t>("resolution", 4000);
				
		arq.save();
	}

	/************************************************/

	 {
	 	File arq(filename);

		arq.addGroup("newGroup") = std::move(arq.getGroup("hoxa"));
		arq.remove("hoxa");

	 	arq.save();
	 }

	return EXIT_SUCCESS;
}
