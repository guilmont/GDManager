import ctypes
import numpy as np
from sys import platform

from numpy.core.numeric import isscalar

if platform == "win32":
    lib = ctypes.cdll.LoadLibrary('PyGDManager.dll')
elif platform == "darwin":
    lib = ctypes.cdll.LoadLibrary('libPyGDManager.dylib')
else:
    lib = ctypes.cdll.LoadLibrary('libPyGDManager.so')


## OBJECT
lib.getObjectLabel.argtypes, lib.getObjectLabel.restype = [ctypes.c_void_p], ctypes.c_char_p
lib.getObjectType.argtypes,  lib.getObjectType.restype = [ctypes.c_void_p], ctypes.c_uint32
lib.getParent.argtypes,      lib.getParent.restype = [ctypes.c_void_p], ctypes.c_void_p

## DESCRIPTION
lib.getNumDescriptions.argtypes, lib.getNumDescriptions.restype = [ctypes.c_void_p], ctypes.c_uint64
lib.addDescription.argtypes, lib.addDescription.restype = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p], None
lib.getDescription.argtypes, lib.getDescription.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_char_p
lib.descriptionChildren.argtypes, lib.descriptionChildren.restype = [ctypes.c_void_p], ctypes.c_void_p

## DATA
lib.newData.argtypes, lib.newData.restype = [ctypes.c_char_p, ctypes.c_uint32], ctypes.c_void_p
lib.getDataShape.argtypes, lib.getDataShape.restype = [ctypes.c_void_p], ctypes.c_void_p
lib.releaseData.argtypes, lib.releaseData.restype = [ctypes.c_void_p], None

lib.getInt32.argtypes,  lib.getInt32.restype = [ctypes.c_void_p], ctypes.c_void_p
lib.getInt64.argtypes,  lib.getInt64.restype = [ctypes.c_void_p], ctypes.c_void_p
lib.getUInt8.argtypes,  lib.getUInt8.restype = [ctypes.c_void_p], ctypes.c_void_p
lib.getUInt16.argtypes, lib.getUInt16.restype = [ctypes.c_void_p], ctypes.c_void_p
lib.getUInt32.argtypes, lib.getUInt32.restype = [ctypes.c_void_p], ctypes.c_void_p
lib.getUInt64.argtypes, lib.getUInt64.restype = [ctypes.c_void_p], ctypes.c_void_p
lib.getFloat.argtypes,  lib.getFloat.restype = [ctypes.c_void_p], ctypes.c_void_p
lib.getDouble.argtypes, lib.getDouble.restype = [ctypes.c_void_p], ctypes.c_void_p

lib.resetInt32.argtypes,  lib.resetInt32.restype = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], None
lib.resetInt64.argtypes,  lib.resetInt64.restype = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], None
lib.resetUInt8.argtypes,  lib.resetUInt8.restype = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], None
lib.resetUInt16.argtypes, lib.resetUInt16.restype = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], None
lib.resetUInt32.argtypes, lib.resetUInt32.restype = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], None
lib.resetUInt64.argtypes, lib.resetUInt64.restype = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], None
lib.resetFloat.argtypes,  lib.resetFloat.restype = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], None
lib.resetDouble.argtypes, lib.resetDouble.restype = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], None

## GROUP
lib.newGroup.argtypes, lib.newGroup.restype = [ctypes.c_char_p], ctypes.c_void_p
lib.groupContains.argtypes, lib.groupContains.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_bool

lib.groupClear.argtypes, lib.groupClear.restype = [ctypes.c_void_p], None
lib.groupRemove.argtypes, lib.groupRemove.restype = [ctypes.c_void_p, ctypes.c_char_p], None

lib.addGroup.argtypes,    lib.addGroup.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_void_p
lib.copyDataObj.argtypes,  lib.copyDataObj.restype = [ctypes.c_void_p, ctypes.c_void_p], None
lib.importDataObj.argtypes,  lib.importDataObj.restype = [ctypes.c_void_p, ctypes.c_void_p], None

lib.getGroup.argtypes, lib.getGroup.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_void_p
lib.getData.argtypes, lib.getData.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_void_p

lib.addInt32.argtypes,  lib.addInt32.restype = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], ctypes.c_void_p
lib.addInt64.argtypes,  lib.addInt64.restype = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], ctypes.c_void_p
lib.addUInt8.argtypes,  lib.addUInt8.restype = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], ctypes.c_void_p
lib.addUInt16.argtypes, lib.addUInt16.restype = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], ctypes.c_void_p
lib.addUInt32.argtypes, lib.addUInt32.restype = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], ctypes.c_void_p
lib.addUInt64.argtypes, lib.addUInt64.restype = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], ctypes.c_void_p
lib.addFloat.argtypes,  lib.addFloat.restype = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], ctypes.c_void_p
lib.addDouble.argtypes, lib.addDouble.restype = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64], ctypes.c_void_p

lib.groupNumChildren.argtypes, lib.groupNumChildren.restype = [ctypes.c_void_p], ctypes.c_uint64
lib.groupChildren.argtypes, lib.groupChildren.restype = [ctypes.c_void_p], ctypes.c_void_p
lib.getObject.argtypes, lib.getObject.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_void_p


######################################################################################################################################################
######################################################################################################################################################
## API

class Type:
    # Important to identical to cpp enum class Type
    NONE, GROUP, INT32, UINT8, UINT16, UINT32, FLOAT, DOUBLE, INT64, UINT64 = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9


class State:
    # Importatnt to identical to cpp enum class State
    NONE, READ, WRITE = 0, 1, 2

###############################################################################
###############################################################################

class Description:
    def __init__(self, obj):
        self.obj = obj
        self.iter = 0
        self.descLabel = list()

    def size(self):
        return lib.getNumDescriptions(self.obj)

    def __iter__(self):
        N = self.size()
        ptr = ctypes.cast(lib.descriptionChildren(self.obj), ctypes.POINTER(ctypes.c_uint8))

        ct = 0
        for k in range(N):
            size = int(ptr[ct])
            ct+=1

            vec = [ chr(a) for a in ptr[ct:ct+size]]
            ct += size

            self.descLabel.append("".join(vec))

        self.iter = 0
        return self

    def __next__(self):
        if self.iter < len(self.descLabel):
            label = self.descLabel[self.iter]
            self.iter += 1
            
            return label, self.__getitem__(label)

        else:
            raise StopIteration

    def __getitem__(self, label):
        return lib.getDescription(self.obj, label.encode('utf-8')).decode('utf-8')


###############################################################################
###############################################################################

class Object:
    def __init__(self, label, type):
        self.label = label
        self.type = type
        self.obj = None

    def getLabel(self):
        return lib.getObjectLabel(self.obj).decode('utf-8')

    def getType(self):
        return lib.getObjectType(self.obj)

    def parent(self):
        ptr = lib.getParent(self.obj)
        grp = Group(lib.getObjectLabel(ptr).decode('utf-8'))
        grp.obj = ptr
        return grp

    def addDescription(self, label, description):
        lib.addDescription(self.obj, label.encode('utf-8'), description.encode('utf-8'))

    def getDescription(self, label):
        return lib.getDescription(self.obj, label.encode('utf-8')).decode('utf-8')

    def descriptions(self):
        return Description(self.obj)

    def getNumDescriptions(self):
        return lib.getNumDescriptions(self.obj)        

   
###############################################################################
###############################################################################

class Data(Object):
    def __init__(self, label, type):
        Object.__init__(self, label, type)

        self.obj = lib.newData(label.encode('utf-8'), type)

    def getShape(self):
        arr = ctypes.string_at(lib.getDataShape(self.obj), 16)
        vec = np.frombuffer(arr, np.uint64, 2).reshape(-1)
        return (vec[0], vec[1])

    def release(self):
        lib.releaseData(self.obj)

    def __getitem__(self, id):
        typ = self.getType()

        if typ == Type.INT32:
            ptr = ctypes.cast(lib.getInt32(self.obj), ctypes.POINTER(ctypes.c_int32))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()[id]

        elif typ == Type.INT64:
            ptr = ctypes.cast(lib.getInt64(self.obj), ctypes.POINTER(ctypes.c_int64))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()[id]

        elif typ == Type.UINT8:
            ptr = ctypes.cast(lib.getUInt8(self.obj), ctypes.POINTER(ctypes.c_uint8))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()[id]

        elif typ == Type.UINT16:
            ptr = ctypes.cast(lib.getUInt16(self.obj), ctypes.POINTER(ctypes.c_uint16))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()[id]

        elif typ == Type.UINT32:
            ptr = ctypes.cast(lib.getUInt32(self.obj), ctypes.POINTER(ctypes.c_uint32))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()[id]

        elif typ == Type.UINT64:
            ptr = ctypes.cast(lib.getUInt64(self.obj), ctypes.POINTER(ctypes.c_uint64))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()[id]

        elif typ == Type.FLOAT:
            ptr = ctypes.cast(lib.getFloat(self.obj), ctypes.POINTER(ctypes.c_float))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()[id]

        elif typ == Type.DOUBLE:
            ptr = ctypes.cast(lib.getDouble(self.obj), ctypes.POINTER(ctypes.c_double))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()[id]

        else:
            raise TypeError

    def reset(self, array):
        typ = self.getType()
        hei, wid = array.shape

        if typ == Type.INT32:
            assert(array.dtype == np.int32)
            lib.resetInt32(self.obj, array.ctypes.data_as(ctypes.c_void_p), hei, wid)

        elif typ == Type.INT64:
            assert(array.dtype == np.int64)
            lib.resetInt64(self.obj, array.ctypes.data_as(ctypes.c_void_p), hei, wid)

        elif typ == Type.UINT8:
            assert(array.dtype == np.uint8)
            lib.resetUInt8(self.obj, array.ctypes.data_as(ctypes.c_void_p), hei, wid)

        elif typ == Type.UINT16:
            assert(array.dtype == np.uint16)
            lib.resetUInt16(self.obj, array.ctypes.data_as(ctypes.c_void_p), hei, wid)

        elif typ == Type.UINT32:
            assert(array.dtype == np.uint32)
            lib.resetUInt32(self.obj, array.ctypes.data_as(ctypes.c_void_p), hei, wid)

        elif typ == Type.UINT64:
            assert(array.dtype == np.uint64)
            lib.resetUInt64(self.obj, array.ctypes.data_as(ctypes.c_void_p), hei, wid)

        elif typ == Type.FLOAT:
            assert(array.dtype == np.float32)
            lib.resetFloat(self.obj, array.ctypes.data_as(ctypes.c_void_p), hei, wid)

        elif typ == Type.DOUBLE:
            assert(array.dtype == np.float64)
            lib.resetDouble(self.obj, array.ctypes.data_as(ctypes.c_void_p), hei, wid)

        else:
            raise TypeError

###############################################################################
###############################################################################


class Group(Object):
    def __init__(self, name):
        Object.__init__(self, name, Type.GROUP)

        self.obj = lib.newGroup(name.encode('utf-8'))
        self.iter, self.children = 0, list()

    def __iter__(self):
        # Get number of children
        N = lib.groupNumChildren(self.obj)

        ptr = ctypes.cast(lib.groupChildren(self.obj), ctypes.POINTER(ctypes.c_uint8))

        ct = 0
        for k in range(N):
            size = int(ptr[ct])
            ct+=1

            vec = [ chr(a) for a in ptr[ct:ct+size]]
            ct += size

            self.children.append("".join(vec))

        self.iter = 0
        return self

    def __next__(self):
        if self.iter < len(self.children):
            
            label = self.children[self.iter]
            self.iter += 1
            
            ptr = lib.getObject(self.obj, label.encode('utf-8'))
            tp = lib.getObjectType(ptr) 

            if (tp == Type.GROUP):
                grp = Group(label)
                grp.obj = ptr
                return label, grp

            else:
                dt = Data(label, tp)
                dt.obj = ptr

                return label, dt
        else:
            raise StopIteration


    def contains(self, label):
        return lib.groupContains(self.obj, label.encode('utf-8'))

    def remove(self, label):
        lib.groupRemove(self.obj, label.encode('utf-8'))

    def clear(self):
        lib.groupClear(self.obj)

    #########################
    # Getting objects

    def getData(self, label):
        ptr = lib.getData(self.obj, label.encode('utf-8'))
        tp = lib.getObjectType(ptr)

        if lib.getObjectType(ptr) == Type.GROUP:
            raise TypeError

        loc = Data(label, tp)
        loc.obj = ptr

        return loc

    def getGroup(self, label):
        ptr = lib.getGroup(self.obj, label.encode('utf-8'))
        tp = lib.getObjectType(ptr)

        if lib.getObjectType(ptr) != Type.GROUP:
            raise TypeError

        loc = Group(label)
        loc.obj = ptr

        return loc

    #########################
    # Adding objects

    def addGroup(self, label):
        var = Group(label)
        var.obj = lib.addGroup(self.obj, label.encode('utf-8'))
        return var

    def addGroupObj(self, group):
        lib.addGroupObj(self.obj, group.obj)

    def copyData(self, data):
        lib.copyDataObj(self.obj, data.obj)

    def importData(self, data):
        lib.importDataObj(self.obj, data.obj)

    def addData(self, label, array):
        if (np.isscalar(array)):
            array = np.asarray([[array]], dtype=type(array))

        if isinstance(array, list):
            N = len(array)
            tp = type(array[0])
            array = np.asarray(array, dtype=tp).reshape(1, N)

        if array.size == array.shape[0]:
            wid = array.shape[0]
            array = array.reshape(1, wid)

        hei, wid = array.shape[0], array.shape[1]
        lbl = label.encode('utf-8')

        if array.dtype == np.int32:
            var = Data(label, Type.INT32)
            var.obj = lib.addInt32(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), hei, wid)
            return var

        elif array.dtype == np.int64:
            var = Data(label, Type.INT64)
            var.obj = lib.addInt64(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), hei, wid)
            return var

        elif array.dtype == np.uint8:
            var = Data(label, Type.UINT8)
            var.obj = lib.addUInt8(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), hei, wid)
            return var

        elif array.dtype == np.uint16:
            var = Data(label, Type.UINT16)
            var.obj = lib.addUInt16(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), hei, wid)
            return var

        elif array.dtype == np.uint32:
            var = Data(label, Type.UINT32)
            var.obj = lib.addUInt32(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), hei, wid)

        elif array.dtype == np.uint64:
            var = Data(label, Type.UINT64)
            var.obj = lib.addUInt64(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), hei, wid)
            return var

        elif array.dtype == np.float32:
            var = Data(label, Type.FLOAT)
            var.obj = lib.addFloat(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), hei, wid)
            return var

        elif array.dtype == np.float64 or array.dtype == np.double or array.dtype == 'float':
            var = Data(label, Type.DOUBLE)
            var.obj = lib.addDouble(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), hei, wid)
            return var

        else:
            raise TypeError

###########################################################################
###########################################################################

class File(Group):
    def __init__(self, filename):
        Group.__init__(self, "root")

        lib.newFile.argtypes, lib.newFile.restype = [ctypes.c_char_p], ctypes.c_void_p
        lib.saveFile.argtypes, lib.saveFile.restype = [ctypes.c_void_p], None
        lib.closeFile.argtypes, lib.closeFile.restype = [ctypes.c_void_p], None

        lib.getFilePath.argtypes, lib.getFilePath.restype = [ctypes.c_void_p], ctypes.c_char_p

        self.obj = lib.newFile(filename.encode('utf-8'))

    def getfilePath(self):
        return lib.getFilePath(self.obj).decode('utf-8')

    def save(self):
        lib.saveFile(self.obj)

    def close(self):
        lib.closeFile(self.obj)
