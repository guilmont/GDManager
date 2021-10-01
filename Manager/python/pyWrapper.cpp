#include "GDManager.h"

extern "C"
{
    // Object
    GDM_API const char *getObjectLabel(GDM::Object *obj) { return obj->getLabel().c_str(); }
    GDM_API uint32_t getObjectType(GDM::Object *obj) { return static_cast<uint32_t>(obj->getType()); }

    GDM_API GDM::Group *getParent(GDM::Object *obj) { return obj->parent; }

    GDM_API void renameObject(GDM::Object *obj, const char *name) { obj->rename(name); }

    GDM_API uint64_t getNumDescriptions(GDM::Object *obj) { return obj->descriptions().size(); }

    GDM_API void addDescription(GDM::Object *obj, const char *label, const char *description) { obj->addDescription(label, description); }
    GDM_API const char *getDescription(GDM::Object *obj, const char *label) { return obj->getDescription(label).c_str(); }

    GDM_API uint8_t* descriptionChildren(GDM::Group *obj)
    {
        uint64_t ct = 0;
        for (auto &[label, desc] : obj->descriptions())
            ct += label.size()+1;

        uint8_t *ptr = new uint8_t[ct];

        ct = 0;        
        for (auto &[label, desc] : obj->descriptions())
        {
            uint8_t size = static_cast<uint8_t>(label.size()); // It must be less than 32 by definition

            memcpy(ptr + ct, &size, 1);
            ct++;

            memcpy(ptr + ct, label.c_str(), label.size());
            ct += label.size();
        }

        return ptr;
    } 

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // Data

    GDM_API GDM::Data *newData(const char *name, GDM::Type type) { return new GDM::Data(name, type); }
    GDM_API const uint64_t *getDataShape(GDM::Data *obj) { return reinterpret_cast<const uint64_t *>(&obj->getShape()); }
    GDM_API void releaseData(GDM::Data *obj) { obj->release(); }

    GDM_API const int32_t *getInt32(GDM::Data *obj) { return obj->getArray<int32_t>(); }
    GDM_API const int64_t *getInt64(GDM::Data *obj) { return obj->getArray<int64_t>(); }
    GDM_API const uint8_t *getUInt8(GDM::Data *obj) { return obj->getArray<uint8_t>(); }
    GDM_API const uint16_t *getUInt16(GDM::Data *obj) { return obj->getArray<uint16_t>(); }
    GDM_API const uint32_t *getUInt32(GDM::Data *obj) { return obj->getArray<uint32_t>(); }
    GDM_API const uint64_t *getUInt64(GDM::Data *obj) { return obj->getArray<uint64_t>(); }
    GDM_API const float *getFloat(GDM::Data *obj) { return obj->getArray<float>(); }
    GDM_API const double *getDouble(GDM::Data *obj) { return obj->getArray<double>(); }

    GDM_API void resetInt32(GDM::Data *obj, const int32_t *ptr, uint64_t height, uint64_t width) { return obj->reset<int32_t>(ptr, {height, width}); }
    GDM_API void resetInt64(GDM::Data *obj, const int64_t *ptr, uint64_t height, uint64_t width) { return obj->reset<int64_t>(ptr, {height, width}); }
    GDM_API void resetUInt8(GDM::Data *obj, const uint8_t *ptr, uint64_t height, uint64_t width) { return obj->reset<uint8_t>(ptr, {height, width}); }
    GDM_API void resetUInt16(GDM::Data *obj, const uint16_t *ptr, uint64_t height, uint64_t width) { return obj->reset<uint16_t>(ptr, {height, width}); }
    GDM_API void resetUInt32(GDM::Data *obj, const uint32_t *ptr, uint64_t height, uint64_t width) { return obj->reset<uint32_t>(ptr, {height, width}); }
    GDM_API void resetUInt64(GDM::Data *obj, const uint64_t *ptr, uint64_t height, uint64_t width) { return obj->reset<uint64_t>(ptr, {height, width}); }
    GDM_API void resetFloat(GDM::Data *obj, const float *ptr, uint64_t height, uint64_t width) { return obj->reset<float>(ptr, {height, width}); }
    GDM_API void resetDouble(GDM::Data *obj, const double *ptr, uint64_t height, uint64_t width) { return obj->reset<double>(ptr, {height, width}); }

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // Group

    GDM_API GDM::Group *newGroup(const char *name) { return new GDM::Group(name); }
    GDM_API bool groupContains(GDM::Group *group, const char *name) { return group->contains(name); }

    GDM_API void groupClear(GDM::Group *group) { group->clear(); }
    GDM_API void groupRemove(GDM::Group *group, const char *label) { group->remove(label); }

    GDM_API GDM::Group *addGroup(GDM::Group *group, const char *label) { return &group->addGroup(label); }
    GDM_API void copyDataObj(GDM::Group* group, const GDM::Data* data) { group->copyData(*data); }
    GDM_API void importDataObj(GDM::Group *group, GDM::Data *data) { group->importData(*data); }

    GDM_API GDM::Group *getGroup(GDM::Group *group, const char *label) { return &group->getGroup(label); }
    GDM_API GDM::Data *getData(GDM::Group *group, const char *label) { return &group->getData(label); }

    GDM_API GDM::Data *addInt32(GDM::Group *obj, const char *label, const int32_t *ptr, uint64_t height, uint64_t width) { return &(obj->add<int32_t>(label, ptr, {height, width})); }
    GDM_API GDM::Data *addInt64(GDM::Group *obj, const char *label, const int64_t *ptr, uint64_t height, uint64_t width) { return &(obj->add<int64_t>(label, ptr, {height, width})); }
    GDM_API GDM::Data *addUInt8(GDM::Group *obj, const char *label, const uint8_t *ptr, uint64_t height, uint64_t width) { return &(obj->add<uint8_t>(label, ptr, {height, width})); }
    GDM_API GDM::Data *addUInt16(GDM::Group *obj, const char *label, const uint16_t *ptr, uint64_t height, uint64_t width) { return &(obj->add<uint16_t>(label, ptr, {height, width})); }
    GDM_API GDM::Data *addUInt32(GDM::Group *obj, const char *label, const uint32_t *ptr, uint64_t height, uint64_t width) { return &(obj->add<uint32_t>(label, ptr, {height, width})); }
    GDM_API GDM::Data *addUInt64(GDM::Group *obj, const char *label, const uint64_t *ptr, uint64_t height, uint64_t width) { return &(obj->add<uint64_t>(label, ptr, {height, width})); }
    GDM_API GDM::Data *addFloat(GDM::Group *obj, const char *label, const float *ptr, uint64_t height, uint64_t width) { return &(obj->add<float>(label, ptr, {height, width})); }
    GDM_API GDM::Data *addDouble(GDM::Group *obj, const char *label, const double *ptr, uint64_t height, uint64_t width) { return &(obj->add<double>(label, ptr, {height, width})); }


    GDM_API uint64_t groupNumChildren(GDM::Group *obj) { return obj->getNumChildren(); }
    GDM_API GDM::Object *getObject(GDM::Group *obj, const char* label) { return obj->children().at(label); }    

    GDM_API uint8_t* groupChildren(GDM::Group *grp)
    {
        uint64_t ct = 0;
        for (auto &[label, obj] : grp->children())
            ct += label.size()+1;

        uint8_t *ptr = new uint8_t[ct];

        ct = 0;        
        for (auto &[label, obj] : grp->children())
        {
            uint8_t size = static_cast<uint8_t>(label.size()); // It must be less than 32 by definition

            memcpy(ptr + ct, &size, 1);
            ct++;

            memcpy(ptr + ct, label.c_str(), label.size());
            ct += label.size();
        }

        return ptr;
    } 


    
    ///////////////////////////////////////////////////////
    // File
    GDM_API GDM::File *newFile(const char *filepath) { return new GDM::File(fs::path(filepath)); }
    GDM_API void saveFile(GDM::File* file) { file->save(); }
    GDM_API void closeFile(GDM::File* file) { file->close(); }

    GDM_API const char *getFilePath(GDM::File *file) 
    { 
        const std::string& pathStr = file->getFilePath().string();
        return pathStr.c_str(); 
    }

 
}