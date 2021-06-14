#pragma once

#include "core.h"

namespace GDM
{

    struct Shape
    {
        uint64_t width, height;
    };

    enum class Type : uint32_t
    {
        GROUP,
        INT32,
        UINT8,
        UINT16,
        UINT32,
        FLOAT,
        DOUBLE,
        STRING,
    };

    class GDM_API Data
    {
    public:
        Data(const std::string &name = "root", Type type = Type::GROUP) : label(name), type(type) { pout("DATA :: DEFAULT CONSTRUCTOR >>", label); }
        ~Data(void);

        Data(const Data& var);
        Data(Data&& var) noexcept;

        Data& operator=(const Data& mat);
        Data& operator=(Data&& mat) noexcept;

        // utilities
        Type getType(void) const { return type; }
        const Shape &getShape(void) const { return shape; }
        const std::string &getLabel(void) const { return label; }

        bool contains(const std::string &name) const; // Used for groups

        // access element
        Data &operator[](const std::string &name);
        const Data &operator[](const std::string &name) const;

        // Add elements
        Data &addGroup(const std::string &label);

        template <typename TP>
        Data& add(const std::string& label, const TP* value, Shape shape = { 1,1 });

        // remove elements
        void remove(const std::string& label);

        // retrieving data
        template <typename TP>
        TP get(void) const { return reinterpret_cast<TP*>(buffer)[0]; }

        template <typename TP>
        const TP* getArray(void) const { return reinterpret_cast<TP*>(buffer); }

        // group interations
        uint64_t getNumChildren(void) const { assert(type == Type::GROUP);  return objs.size(); }
        std::map<std::string, Data>& children() { assert(type == Type::GROUP);  return objs;  }

    private:
        std::string label;
        Type type;

        // If class represents a group
        std::map<std::string, Data> objs;

        // If class represents a data type have a data type
        uint64_t numBytes = 1;
        Shape shape = {1, 1};
        uint8_t *buffer = nullptr;
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


}