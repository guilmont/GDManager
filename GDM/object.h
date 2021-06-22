#pragma once

#include "core.h"

namespace GDM
{

    class Data
    {
    public:
        GDM_API Data(const std::string &name = "root", Type type = Type::GROUP) : label(name), type(type) { pout("DATA :: DEFAULT CONSTRUCTOR >>", label); }
        GDM_API ~Data(void);

        GDM_API Data(const Data& var);
        GDM_API Data(Data&& var) noexcept;

        GDM_API Data& operator=(const Data& mat);
        GDM_API Data& operator=(Data&& mat) noexcept;

        // utilities
        GDM_API Type getType(void) const { return type; }
        GDM_API const Shape &getShape(void) const { return shape; }
        GDM_API  const std::string &getLabel(void) const { return label; }

        GDM_API bool contains(const std::string &name) const; // Used for groups

        // access element
        GDM_API Data &operator[](const std::string &name);
        GDM_API const Data &operator[](const std::string &name) const;

        // Add elements
        GDM_API Data &addGroup(const std::string &label);

        GDM_API  void addDescription(const std::string& label, const std::string& description);

        template <typename TP>
        Data& add(const std::string& label, const TP* value, Shape shape);

        template <typename TP>
        Data& add(const std::string& label, TP value);

        // remove elements
        GDM_API void remove(const std::string& label);

        // retrieving data
        template <typename TP>
        TP get(void) const { return reinterpret_cast<TP*>(buffer)[0]; }

        GDM_API const std::string& getDescription(const std::string& label) const;

        template <typename TP>
        const TP* getArray(void) const { return reinterpret_cast<TP*>(buffer); }

        // description interations
        GDM_API std::map<std::string, std::string>& descriptions(void) { return objDescription;  }

        // group interations
        GDM_API uint32_t getNumChildren(void) const { assert(type == Type::GROUP);  return uint32_t(objs.size()); }
        GDM_API std::map<std::string, Data>& children() { assert(type == Type::GROUP);  return objs;  }

    protected:
        std::string label;
        Type type;

        // If class represents a group
        std::map<std::string, Data> objs;

        // If class represents a data type have a data type
        uint64_t numBytes = 1;
        Shape shape = {1, 1};
        uint8_t *buffer = nullptr;


        // We can always add some sort of description to the object
        std::map<std::string, std::string> objDescription;

     };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // TEMPLATE DEFINITIONS

    template <typename TP>  static Type GetType(void);
    template<> Type GetType<int32_t>(void) { return Type::INT32; }
    template<> Type GetType<uint32_t>(void) { return Type::UINT32; }
    template<> Type GetType<uint16_t>(void) { return Type::UINT16; }
    template<> Type GetType<uint8_t>(void) { return Type::UINT8; }
    template<> Type GetType<float>(void) { return Type::FLOAT; }
    template<> Type GetType<double>(void) { return Type::DOUBLE; }


    template <typename TP>
    Data& Data::add(const std::string& label, const TP* value, Shape shape)
    {
        assert(label.size() < MAX_LABEL_SIZE);
        assert(objs.find(label) == objs.end());

        Data var(label, GetType<TP>());
        var.shape = shape;
        var.numBytes = shape.width * shape.height * sizeof(TP);
        var.buffer = new uint8_t[var.numBytes];

        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(value);
        std::copy(ptr, ptr + var.numBytes, var.buffer);
        objs.emplace(label, std::move(var));

        return objs[label];
    }

    template<typename TP>
    Data& Data::add(const std::string& label, TP value)
    {
        assert(label.size() < MAX_LABEL_SIZE);
        assert(objs.find(label) == objs.end());

        Data var(label, GetType<TP>());
        var.shape = shape;
        var.numBytes = sizeof(TP);
        var.buffer = new uint8_t[var.numBytes];

        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        std::copy(ptr, ptr + var.numBytes, var.buffer);
        objs.emplace(label, std::move(var));

        return objs[label];
    }


}