#include "GDManager.h"

using namespace GDM;

#include <corecrt_math_defines.h>

int main(void)
{

	File arq;

	arq.add<double>("this is pi", M_PI);

	Group &hoxa = arq.addGroup("hoxa");

	Group &hic = hoxa.addGroup("hic");
	hic.add<uint32_t>("CoordinateAtZero", 49996001);
	hic.add<uint32_t>("resolution", 4000);

	Group &cell = hoxa.addGroup("cellLines");
	cell.addDescription("test", "123");

	Group &t12 = cell.addGroup("T1-T2");

	uint32_t loc[2] = {52014370, 52321145};
	t12.add<uint32_t>("locations", loc, {1, 2});

	double dist[2] = {0.347, 0.04};
	t12.add<double>("distance", dist, {1, 2});
	t12.get<Data>("distance").addDescription("Unis", "microns");

	float D[4] = {1.563f, 0.186f, 1.927f, 0.316f};
	t12.add<float>("D", D, {2, 2});

	float A[4] = {0.251f, 0.027f, 0.334f, 0.033f};
	t12.add<float>("A", A, {2, 2});

	arq.save("../gdm_hello.gdm");

	return EXIT_SUCCESS;
}
