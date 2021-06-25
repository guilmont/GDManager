#include "object.h"

namespace GDM
{
    GDM_API void Object::rename(const std::string& name)
    {
        if (parent)
        {
            Group* group = reinterpret_cast<Group*>(parent);
            group->m_children[name] = this;
            group->m_children[label] = nullptr; // we don't want to destroy "this"
            group->remove(this->label);
        }

        this->label = name;
    }

    void Object::addDescription(const std::string &label, const std::string &description)
    {
        assert(m_description.find(label) == m_description.end());
        assert(label.size() < MAX_LABEL_SIZE);

        m_description.emplace(label, description);
    }

    const std::string &Object::getDescription(const std::string &label) const
    {
        auto it = m_description.find(label);
        assert(it != m_description.end());
        return it->second;
    }

    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////

    Data::Data(const std::string& name, Type type) : Object(name, type) {}

    Data::~Data(void)
    {
        if (buffer)
            delete[] buffer;
    }

    Data::Data(const Data &var)
    {
        this->label = var.label;
        this->type = var.type;
        this->numBytes = var.numBytes;
        this->shape = var.shape;

        this->buffer = new uint8_t[this->numBytes];
        std::copy(var.buffer, var.buffer + var.numBytes, this->buffer);

        // pout("DATA :: COPY CONSTRUCTOR >>", label);
    }

    Data::Data(Data &&var) noexcept
    {
        this->label = std::move(var.label);
        this->type = var.type;
        this->numBytes = var.numBytes;
        this->shape = var.shape;
        this->buffer = std::move(var.buffer);

        var.shape = {0, 0};
        var.numBytes = 0;
        var.buffer = nullptr;

        // pout("DATA :: MOVE CONSTRUCTOR >>", label);
    }

    Data &Data::operator=(const Data &var)
    {
        // pout("DATA::ASSIGN OPERATOR >>", label);
        if (&var != this)
        {
            this->~Data();
            new (this) Data(var);
        }

        return *this;
    }

    Data &Data::operator=(Data &&var) noexcept
    {
        // pout("DATA :: MOVE OPERATOR >>", label);

        if (&var != this)
        {
            this->~Data();
            new (this) Data(std::move(var));
        }

        return *this;
    }



    void Data::release(void)
    {
        assert(numBytes > sizeof(getNumBytes(type))); // We can only release data if if contains more than 8 bytes
        assert(buffer != nullptr);                    // why not to check that too?

        if (buffer)
        {
            delete[] buffer;
            buffer = nullptr;
            // the offset in the inpnut file should still be set, hence we can re-import this data if necessary
        }
    }

    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////

    Group::Group(const std::string& name) : Object(name, Type::GROUP) {}

    Group::~Group(void)
    {
        for (auto [label, ptr] : m_children)
        {
            // pout("GROUP :: DELETE >> ", label);
            delete ptr;
        }

        m_children.clear();
    }

    Group::Group(const Group &var)
    {
        this->label = var.label;
        this->type = var.type;
        this->m_children = var.m_children;

        // pout("GROUP :: COPY CONSTRUCTOR >>", label);
    }

    Group::Group(Group &&var) noexcept
    {
        this->label = std::move(var.label);
        this->type = var.type;
        this->m_children = std::move(var.m_children);

        var.m_children.clear();

        // pout("GROUP :: MOVE CONSTRUCTOR >>", label);
    }

    Group &Group::operator=(const Group &var)
    {
        // pout("GROUP::ASSIGN OPERATOR >>", label);
        if (&var != this)
        {
            this->~Group();
            new (this) Group(var);
        }

        return *this;
    }

    Group &Group::operator=(Group &&var) noexcept
    {
        // pout("GROUP :: MOVE OPERATOR >>", label);

        if (&var != this)
        {
            this->~Group();
            new (this) Group(std::move(var));
        }

        return *this;
    }

    bool Group::contains(const std::string &label) const
    {
        assert(label.size() < MAX_LABEL_SIZE);
        return m_children.find(label) != m_children.end();
    }


    Group &Group::addGroup(const std::string &label)
    {
        assert(label.size() < MAX_LABEL_SIZE);
        assert(m_children.find(label) == m_children.end());

        Group *ptr = new Group(label);
        ptr->parent = this;

        m_children.emplace(label, std::move(ptr));
        return *ptr;
    }

    void Group::remove(const std::string &label)
    {
        assert(label.size() < MAX_LABEL_SIZE);

        auto it = m_children.find(label);

        assert( it != m_children.end());
        m_children.erase(it);
    }

 
}