#pragma once

#include "core.h"

namespace GDM
{
    class File;
    class Data;
    class Group;

    class Object
    {
        friend class File;

    public:
        GDM_API Object(const std::string &label, Type type) : label(label), type(type) {}
        GDM_API Object(void) = default;
        GDM_API virtual ~Object(void) = default;

        // utilities
        GDM_API const std::string &getLabel(void) const { return label; }
        GDM_API Type getType(void) const { return type; }
        GDM_API void rename(const std::string &name);

        GDM_API void addDescription(const std::string &label, const std::string &description);
        GDM_API const std::string &getDescription(const std::string &label) const;

        // description interations
        GDM_API std::unordered_map<std::string, std::string> &descriptions(void) { return m_description; }

    protected:
        std::string label = "";
        Type type = Type::NONE;

        // We can always add some sort of description to the object
        Description m_description;

        // We might want to now the parent object as well
        Object *parent = nullptr;
    };

    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////

    class Data : public Object
    {
        friend class File;
        friend class Object; // So other objects can change this Data
        friend class Group;

    public:
        GDM_API Data(const std::string &name, Type type);
        GDM_API ~Data(void) override;

        GDM_API Data(const Data &var);
        GDM_API Data(Data &&var) noexcept;

        GDM_API Data &operator=(const Data &mat);
        GDM_API Data &operator=(Data &&mat) noexcept;

        // utilities
        GDM_API const Shape &getShape(void) const { return shape; }

        // setting data
        template <typename TP>
        void reset(const TP *ptr, Shape sp);

        template <typename TP>
        void reset(TP val);

        // retrieving data
        template <typename TP>
        TP get(void) const;

        template <typename TP>
        const TP *getArray(void);

        // Release memory in RAM in case data is to big
        void release(void);

    private:
        uint64_t numBytes = 1;
        Shape shape = {1, 1};
        uint8_t *buffer = nullptr;

        uint64_t offset = 0; // We are going to do lazy loading, so we have the offset to data in file gmdFile
        std::ifstream *gdmFile = nullptr;
    };

    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////

    class Group : public Object
    {
        friend class File;
        friend class Object; // so other objects can change this Group
        friend class Data;

    public:
        GDM_API Group(const std::string &name = "root");
        GDM_API ~Group(void) override;

        GDM_API Group(const Group &var);
        GDM_API Group(Group &&var) noexcept;

        GDM_API Group &operator=(const Group &mat);
        GDM_API Group &operator=(Group &&mat) noexcept;

        // utilities
        GDM_API bool contains(const std::string &name) const;

        template <typename TP>
        TP &get(const std::string &label);

        template <typename TP>
        const TP &get(const std::string &label) const;

        // Add new groups to current group
        GDM_API Group &addGroup(const std::string &label);

        // Adding data to group
        template <typename TP>
        Data &add(const std::string &label, const TP *value, Shape shape);

        template <typename TP>
        Data &add(const std::string &label, TP value);

        // remove elements
        GDM_API void remove(const std::string &label);

        // iterations
        GDM_API uint32_t getNumChildren(void) const { return uint32_t(m_children.size()); }
        GDM_API std::unordered_map<std::string, Object *> &children() { return m_children; }

    private:
        std::unordered_map<std::string, Object *> m_children;
    };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // TEMPLATE DEFINITIONS

    template <typename TP>
    static Type GetType(void);
    template <>
    Type GetType<int32_t>(void) { return Type::INT32; }
    template <>
    Type GetType<uint32_t>(void) { return Type::UINT32; }
    template <>
    Type GetType<uint16_t>(void) { return Type::UINT16; }
    template <>
    Type GetType<uint8_t>(void) { return Type::UINT8; }
    template <>
    Type GetType<float>(void) { return Type::FLOAT; }
    template <>
    Type GetType<double>(void) { return Type::DOUBLE; }

    static uint64_t getNumBytes(Type type)
    {
        switch (type)
        {
        case Type::INT32:
            return sizeof(int32_t);
        case Type::UINT8:
            return sizeof(uint8_t);
        case Type::UINT16:
            return sizeof(uint16_t);
        case Type::UINT32:
            return sizeof(uint32_t);
        case Type::FLOAT:
            return sizeof(float);
        case Type::DOUBLE:
            return sizeof(double);
        default:
            assert(false);
            return 0;
            break;
        }
    }

    /////////////////////////
    /////////////////////////

    template <typename TP>
    void Data::reset(const TP *ptr, Shape sp)
    {
        uint64_t newSize = sp.width * sp.height * sizeof(TP);
        if (newSize != numBytes)
        {
            delete[] buffer;
            numBytes = newSize;
            buffer = new uint8_t[numBytes];
        }

        type = GetType<TP>();
        shape = sp;

        const uint8_t *var = reinterpret_cast<const uint8_t *>(ptr);
        std::copy(var, var + numBytes, buffer);
    }

    template <typename TP>
    void Data::reset(TP value) { reset(&value, {1, 1}); }

    template <typename TP>
    TP Data::get(void) const
    {
        assert(numBytes == sizeof(TP));
        return reinterpret_cast<TP *>(buffer)[0];
    }

    template <typename TP>
    const TP *Data::getArray(void)
    {
        uint64_t pos = offset;
        assert(GetType<TP>() == type);
        if (!buffer)
        {
            // Getting compression method
            Compression comp;
            gdmFile->seekg(pos);
            gdmFile->read(reinterpret_cast<char *>(&comp), sizeof(Compression));
            pos += sizeof(Compression);

            assert(comp == Compression::NONE); // TODO: Implement other types of compression

            // Getting compressed number of bytes -> not important for now
            uint64_t nBytes;
            gdmFile->seekg(pos);
            gdmFile->read(reinterpret_cast<char *>(&nBytes), sizeof(uint64_t));
            pos += sizeof(uint64_t);

            assert(nBytes == this->numBytes); // If not compression is used, theses values should be the same

            // Importing data bytes
            buffer = new uint8_t[numBytes];
            gdmFile->seekg(pos);
            gdmFile->read(reinterpret_cast<char *>(buffer), numBytes);
        }

        return reinterpret_cast<const TP *>(buffer);
    }

    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////

    template <typename TP>
    TP &Group::get(const std::string &label)
    {
        assert(label.size() < MAX_LABEL_SIZE);

        auto it = m_children.find(label);
        assert(it != m_children.end()); // Does it exist?
        return *reinterpret_cast<TP *>(it->second);
    }

    template <typename TP>
    const TP &Group::get(const std::string &label) const
    {
        assert(label.size() < MAX_LABEL_SIZE);

        auto it = m_children.find(label);
        assert(it != m_children.end()); // Does it exist?

        return *reinterpret_cast<TP *>(it->second);
    }

    template <typename TP>
    Data &Group::add(const std::string &label, const TP *value, Shape shape)
    {
        assert(label.size() < MAX_LABEL_SIZE);
        assert(m_children.find(label) == m_children.end());

        Data *var = new Data(label, GetType<TP>());
        var->parent = this;
        var->reset(value, shape);

        m_children.emplace(label, std::move(var));

        return *var;
    }

    template <typename TP>
    Data &Group::add(const std::string &label, TP value)
    {
        assert(label.size() < MAX_LABEL_SIZE);
        assert(m_children.find(label) == m_children.end());

        Data *var = new Data(label, GetType<TP>());
        var->parent = this;
        var->reset(value);

        m_children.emplace(label, std::move(var));

        return *var;
    }

} // namespace