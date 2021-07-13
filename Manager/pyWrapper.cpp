#include "gdm.h"

extern "C" {

    // Object
    GDM_API const char* getObjectLabel(GDM::Object *obj) { return obj->getLabel().c_str(); }
    GDM_API uint32_t getObjectType(GDM::Object *obj) { return static_cast<uint32_t>(obj->getType()); }

    GDM_API GDM::Group* getParent(GDM::Object* obj) { return obj->parent; }

    GDM_API void renameObject(GDM::Object* obj, const char* name) { obj->rename(name); }

    GDM_API void addDescription(GDM::Object* obj, const char* label, const char* description) { obj->addDescription(label, description); }
    GDM_API const char* getDescription(GDM::Object* obj, const char* label) { return obj->getDescription(label).c_str(); }

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // Data

    GDM_API GDM::Data* newData(const char* name, GDM::Type type) { return new GDM::Data(name, type); }
    GDM_API const uint32_t* getDataShape(GDM::Data* obj) { return reinterpret_cast<const uint32_t*>(&obj->getShape()); }
    GDM_API void releaseData(GDM::Data* obj) { obj->release(); }

    GDM_API const int32_t*  getInt32(GDM::Data* obj)  { return obj->getArray<int32_t>();  }
    GDM_API const uint8_t*  getUInt8(GDM::Data* obj)  { return obj->getArray<uint8_t>();  }
    GDM_API const uint16_t* getUInt16(GDM::Data* obj) { return obj->getArray<uint16_t>(); }
    GDM_API const uint32_t* getUInt32(GDM::Data* obj) { return obj->getArray<uint32_t>(); }
    GDM_API const float*    getFloat(GDM::Data* obj)  { return obj->getArray<float>();    }
    GDM_API const double*   getDouble(GDM::Data* obj) { return obj->getArray<double>();   }

    GDM_API void resetInt32 (GDM::Data* obj, const int32_t*  ptr, uint32_t width, uint32_t height) { return obj->reset<int32_t> (ptr, {width, height}); }
    GDM_API void resetUInt8 (GDM::Data* obj, const uint8_t*  ptr, uint32_t width, uint32_t height) { return obj->reset<uint8_t> (ptr, {width, height}); }
    GDM_API void resetUInt16(GDM::Data* obj, const uint16_t* ptr, uint32_t width, uint32_t height) { return obj->reset<uint16_t>(ptr, {width, height}); }
    GDM_API void resetUInt32(GDM::Data* obj, const uint32_t* ptr, uint32_t width, uint32_t height) { return obj->reset<uint32_t>(ptr, {width, height}); }
    GDM_API void resetFloat (GDM::Data* obj, const float*    ptr, uint32_t width, uint32_t height) { return obj->reset<float>   (ptr, {width, height}); }
    GDM_API void resetDouble(GDM::Data* obj, const double*   ptr, uint32_t width, uint32_t height) { return obj->reset<double>  (ptr, {width, height}); }

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // Group
    
    GDM_API GDM::Group *newGroup(const char *name) { return new GDM::Group(name); }
    GDM_API bool groupContains(GDM::Group *group, const char *name) { return group->contains(name); }
    GDM_API void groupRemove(GDM::Group *group, const char* label) { group->remove(label); }
    
    GDM_API GDM::Object* getObject(GDM::Group* group, const char* label) { return  &group->get<GDM::Object>(label); }

    GDM_API GDM::Group* addGroup   (GDM::Group* group, const char* label) { return &group->addGroup(label); }
    GDM_API void        addDataObj (GDM::Group* group, GDM::Data* data)   { group->addData(data); }
    GDM_API void        addGroupObj(GDM::Group* group, GDM::Group* obj)   { group->addGroup(obj); }

    GDM_API GDM::Data*  addInt32 (GDM::Group* obj, const char *label, const int32_t* ptr,  uint32_t width, uint32_t height) { return &(obj->add(label, ptr, { width, height })); }
    GDM_API GDM::Data*  addUInt8 (GDM::Group* obj, const char *label, const uint8_t* ptr,  uint32_t width, uint32_t height) { return &(obj->add(label, ptr, { width, height })); }
    GDM_API GDM::Data*  addUInt16(GDM::Group* obj, const char *label, const uint16_t* ptr, uint32_t width, uint32_t height) { return &(obj->add(label, ptr, { width, height })); }
    GDM_API GDM::Data*  addUInt32(GDM::Group* obj, const char *label, const uint32_t* ptr, uint32_t width, uint32_t height) { return &(obj->add(label, ptr, { width, height })); }
    GDM_API GDM::Data*  addFloat (GDM::Group* obj, const char *label, const float* ptr,    uint32_t width, uint32_t height) { return &(obj->add(label, ptr, { width, height })); }
    GDM_API GDM::Data*  addDouble(GDM::Group* obj, const char *label, const double* ptr,   uint32_t width, uint32_t height) { return &(obj->add(label, ptr, { width, height })); }

    
    // GDM_API uint32_t getNumChildren(GDM::Group *group) { return uint32_t(m_children.size()); }
    // GDM_API <std::string, Object *> &children() { return m_children; }


    ///////////////////////////////////////////////////////
    // File
    GDM_API GDM::File* readFile(const char *path) { return new GDM::File(path); }
    GDM_API GDM::File *newFile(void) { return new GDM::File(); }

    GDM_API const char* getFilePath(GDM::File *file) { return file->getFilePath().string().c_str(); }
    GDM_API void saveFile(GDM::File *file, const char *path) { file->save(path); }

    
 
}