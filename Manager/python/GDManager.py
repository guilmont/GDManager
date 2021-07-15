import ctypes
import numpy as np
from sys import platform

if platform == "win32":
    lib = ctypes.cdll.LoadLibrary('PyGDManager.dll')
elif platform == "darwin":
    lib = ctypes.cdll.LoadLibrary('libPyGDManager.dylib')
else:
    lib = ctypes.cdll.LoadLibrary('libPyGDManager.so')


class Type:
    # Important to identical to cpp enum Type
    NONE, GROUP, INT32, UINT8, UINT16, UINT32, FLOAT, DOUBLE = 0, 1, 2, 3, 4, 5, 6, 7

###############################################################################
###############################################################################


class Object:
    def __init__(self, label, type):
        self.label = label
        self.type = type
        self.obj = None

        lib.getObjectLabel.argtypes, lib.getObjectLabel.restype = [ctypes.c_void_p], ctypes.c_char_p
        lib.getObjectType.argtypes,  lib.getObjectType.restype = [ctypes.c_void_p], ctypes.c_uint32
        lib.getParent.argtypes,      lib.getParent.restype = [ctypes.c_void_p], ctypes.c_void_p
        lib.addDescription.argtypes, lib.addDescription.restype = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p], None
        lib.getDescription.argtypes, lib.getDescription.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_char_p

    def getLabel(self):
        return lib.getObjectLabel(self.obj).decode('ascii')

    def getType(self):
        return lib.getObjectType(self.obj)

    def addDescription(self, label, description):
        lib.addDescription(self.obj, label.encode('ascii'), description.encode('ascii'))

    def getDescription(self, label):
        return lib.getDescription(self.obj, label.encode('ascii')).decode('ascii')

    def parent(self):
        ptr = lib.getParent(self.obj)
        grp = Group(lib.getObjectLabel(ptr).decode('ascii'))
        grp.obj = ptr
        return grp

###############################################################################
###############################################################################


class Data(Object):
    def __init__(self, label, type):
        Object.__init__(self, label, type)

        lib.newData.argtypes, lib.newData.restype = [ctypes.c_char_p, ctypes.c_uint32], ctypes.c_void_p
        lib.getDataShape.argtypes, lib.getDataShape.restype = [ctypes.c_void_p], ctypes.c_void_p
        lib.releaseData.argtypes, lib.releaseData.restype = [ctypes.c_void_p], None

        lib.getInt32.argtypes,  lib.getInt32.restype = [ctypes.c_void_p], ctypes.c_void_p
        lib.getUInt8.argtypes,  lib.getUInt8.restype = [ctypes.c_void_p], ctypes.c_void_p
        lib.getUInt16.argtypes, lib.getUInt16.restype = [ctypes.c_void_p], ctypes.c_void_p
        lib.getUInt32.argtypes, lib.getUInt32.restype = [ctypes.c_void_p], ctypes.c_void_p
        lib.getFloat.argtypes,  lib.getFloat.restype = [ctypes.c_void_p], ctypes.c_void_p
        lib.getDouble.argtypes, lib.getDouble.restype = [ctypes.c_void_p], ctypes.c_void_p

        lib.resetInt32.argtypes,  lib.resetInt32.restype = [
            ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], None
        lib.resetUInt8.argtypes,  lib.resetUInt8.restype = [
            ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], None
        lib.resetUInt16.argtypes, lib.resetUInt16.restype = [
            ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], None
        lib.resetUInt32.argtypes, lib.resetUInt32.restype = [
            ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], None
        lib.resetFloat.argtypes,  lib.resetFloat.restype = [
            ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], None
        lib.resetDouble.argtypes, lib.resetDouble.restype = [
            ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], None

        self.obj = lib.newData(label.encode('ascii'), type)

    def getShape(self):
        arr = ctypes.string_at(lib.getDataShape(self.obj), 16)
        vec = np.frombuffer(arr, np.uint32, 2).reshape(-1)
        return (vec[0], vec[1])

    def release(self):
        lib.releaseData(self.obj)

    def get(self):
        typ = self.getType()

        if typ == Type.INT32:
            ptr = ctypes.cast(lib.getInt32(self.obj), ctypes.POINTER(ctypes.c_int32))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()

        elif typ == Type.UINT8:
            ptr = ctypes.cast(lib.getUInt8(self.obj), ctypes.POINTER(ctypes.c_uint8))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()

        elif typ == Type.UINT16:
            ptr = ctypes.cast(lib.getUInt16(self.obj), ctypes.POINTER(ctypes.c_uint16))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()

        elif typ == Type.UINT32:
            ptr = ctypes.cast(lib.getUInt32(self.obj), ctypes.POINTER(ctypes.c_uint32))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()

        elif typ == Type.FLOAT:
            ptr = ctypes.cast(lib.getFloat(self.obj), ctypes.POINTER(ctypes.c_float))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()

        elif typ == Type.DOUBLE:
            ptr = ctypes.cast(lib.getDouble(self.obj), ctypes.POINTER(ctypes.c_double))
            return np.ctypeslib.as_array(ptr, shape=self.getShape()).copy()

        else:
            raise TypeError

    def reset(self, array):
        typ = self.getType()
        hei, wid = array.shape

        if typ == Type.INT32:
            assert(array.dtype == np.int32)
            lib.resetInt32(self.obj, array.ctypes.data_as(ctypes.c_void_p), wid, hei)

        elif typ == Type.UINT8:
            assert(array.dtype == np.uint8)
            lib.resetUInt8(self.obj, array.ctypes.data_as(ctypes.c_void_p), wid, hei)

        elif typ == Type.UINT16:
            assert(array.dtype == np.uint16)
            lib.resetUInt16(self.obj, array.ctypes.data_as(ctypes.c_void_p), wid, hei)

        elif typ == Type.UINT32:
            assert(array.dtype == np.uint32)
            lib.resetUInt32(self.obj, array.ctypes.data_as(ctypes.c_void_p), wid, hei)

        elif typ == Type.FLOAT:
            assert(array.dtype == np.float32)
            lib.resetFloat(self.obj, array.ctypes.data_as(ctypes.c_void_p), wid, hei)

        elif typ == Type.DOUBLE:
            assert(array.dtype == np.float64)
            lib.resetDouble(self.obj, array.ctypes.data_as(ctypes.c_void_p), wid, hei)

        else:
            raise TypeError

###############################################################################
###############################################################################


class Group(Object):
    def __init__(self, name):
        Object.__init__(self, name, Type.GROUP)

        lib.newGroup.argtypes, lib.newGroup.restype = [ctypes.c_char_p], ctypes.c_void_p
        lib.groupContains.argtypes, lib.groupContains.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_bool
        lib.groupRemove.argtypes, lib.groupRemove.restype = [ctypes.c_void_p, ctypes.c_char_p], None

        lib.getObject.argtypes, lib.getObject.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_void_p

        lib.addGroup.argtypes,    lib.addGroup.restype = [ctypes.c_void_p, ctypes.c_char_p], ctypes.c_void_p
        lib.addGroupObj.argtypes, lib.addGroupObj.restype = [ctypes.c_void_p, ctypes.c_void_p], None
        lib.addDataObj.argtypes,  lib.addDataObj.restype = [ctypes.c_void_p, ctypes.c_void_p], None

        lib.addInt32.argtypes,  lib.addInt32.restype = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], ctypes.c_void_p
        lib.addUInt8.argtypes,  lib.addUInt8.restype = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], ctypes.c_void_p
        lib.addUInt16.argtypes, lib.addUInt16.restype = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], ctypes.c_void_p
        lib.addUInt32.argtypes, lib.addUInt32.restype = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], ctypes.c_void_p
        lib.addFloat.argtypes,  lib.addFloat.restype = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], ctypes.c_void_p
        lib.addDouble.argtypes, lib.addDouble.restype = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32], ctypes.c_void_p

        self.obj = lib.newGroup(name.encode('ascii'))

    #########################
    # Utilities

    def contains(self, label):
        return lib.groupContains(self.obj, label.encode('ascii'))

    def remove(self, label):
        lib.groupRemove(self.obj, label.encode('ascii'))

    #########################
    # Getting objects

    def get(self, label):
        ptr = lib.getObject(self.obj, label.encode('ascii'))
        tp = lib.getObjectType(ptr)

        if tp == Type.GROUP:
            gr = Group(label)
            gr.obj = ptr
            return gr
        else:
            dt = Data(label, tp)
            dt.obj = ptr
            return dt

    def __getitem__(self, key):  # To facilitate navigation

        obj = self
        for label in key.split('/'):
            obj = obj.get(label)

        return obj

    #########################
    # Adding objects

    def addGroup(self, label):
        var = Group(label)
        var.obj = lib.addGroup(self.obj, label.encode('ascii'))
        return var

    def addGroupObj(self, group):
        lib.addGroupObj(self.obj, group.obj)

    def addDataObj(self, data):
        lib.addDataObj(self.obj, data.obj)

    def addData(self, label, array):
        hei, wid = array.shape
        lbl = label.encode('ascii')

        if array.dtype == np.int32:
            var = Data(label, Type.INT32)
            var.obj = lib.addInt32(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), wid, hei)
            return var

        elif array.dtype == np.uint8:
            var = Data(label, Type.UINT8)
            var.obj = lib.addUInt8(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), wid, hei)
            return var

        elif array.dtype == np.uint16:
            var = Data(label, Type.UINT16)
            var.obj = lib.addUInt16(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), wid, hei)
            return var

        elif array.dtype == np.uint32:
            var = Data(label, Type.UINT32)
            var.obj = lib.addUInt32(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), wid, hei)
            return var

        elif array.dtype == np.float32:
            var = Data(label, Type.FLOAT)
            var.obj = lib.addFloat(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), wid, hei)
            return var

        elif array.dtype == np.float64 or array.dtype == np.double or array.dtype == 'float':
            var = Data(label, Type.DOUBLE)
            var.obj = lib.addDouble(self.obj, lbl, array.ctypes.data_as(ctypes.c_void_p), wid, hei)
            return var

        else:
            raise TypeError

    ###########################################################################
    ###########################################################################


class File(Group):
    def __init__(self, filename=""):
        Group.__init__(self, "root")

        lib.readFile.argtypes = [ctypes.c_char_p]
        lib.readFile.restype = ctypes.c_void_p

        lib.newFile.argtypes = None
        lib.newFile.restype = ctypes.c_void_p

        lib.getFilePath.argtypes = [ctypes.c_void_p]
        lib.getFilePath.restype = ctypes.c_char_p

        lib.saveFile.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        lib.saveFile.restype = None

        if (filename == ""):
            self.obj = lib.newFile()
        else:
            self.obj = lib.readFile(filename.encode('ascii'))

    def getfilePath(self):
        return lib.getFilePath(self.obj).decode('ascii')

    def save(self, filename):
        lib.saveFile(self.obj, filename.encode('ascii'))
