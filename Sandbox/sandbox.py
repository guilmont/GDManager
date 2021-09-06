import numpy as np
from math import pi
import GDManager as GDM



def createFile():
    print("###########################################################\ncreateFunction()\n")

    arq = GDM.File("pytest.gdm")
    arq.clear()

    arq.addData("this is pi", pi)

    hoxa = arq.addGroup("hoxa")

    hic = hoxa.addGroup("hic")
    hic.addData("CoordinateAtZero", np.uint32(49996001))
    hic.addData("resolution", 4000)

    cell = hoxa.addGroup("cellLines")
    cell.addDescription("test", "123")

    t12 = cell.addGroup("T1-T2")

    t12.addData("locations", [52014370, 52321145])
    t12.addData("distance", [0.347, 0.04])
    t12.getData("distance").addDescription("Units", "microns")

    D = np.array([[1.563, 0.186], [1.927, 0.316]], dtype=np.float32)
    t12.addData("D", D)

    A = np.array([[0.251, 0.027], [0.334, 0.033]])
    t12.addData("A", A)

    arq.save()
    


def readFile():
    print("\n###########################################################\nreadFunction()")

    arq = GDM.File("pytest.gdm")

    ola = arq.getData("hoxa/hic/resolution")
    print(ola.getType())
    print(ola.getLabel())

    group = arq.getGroup("hoxa/cellLines/T1-T2")
    loc = group.getData("distance")
    print(loc.get()[0], loc.getDescription("Units"))

    loc = arq.getData("hoxa/cellLines/T1-T2/locations").get()[0]
    print(loc)

    print(arq.getData("hoxa/cellLines/T1-T2/locations").getShape())

    pi = arq.getData("this is pi")
    print(pi.get()[0, 0])

    D = arq.getData("hoxa/cellLines/T1-T2/D")
    arq.importData(D)

    A = arq.getData("hoxa/cellLines/T1-T2/A")
    arq.copyData(A)

    arq.save()
    
    


if __name__ == "__main__":
    # Run individually or add some time in between both functions
    # It's weird, but python returns from save before the file is done
    createFile()
    readFile()