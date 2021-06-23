#include "object.h"

namespace GDM
{

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

    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////

    Group::Group(const std::string& name) : Object(name, Type::GROUP) {}

    Group::~Group(void)
    {
        for (auto [label, ptr] : objs)
        {
            // pout("GROUP :: DELETE >> ", label);
            delete ptr;
        }

        objs.clear();
    }

    Group::Group(const Group &var)
    {
        this->label = var.label;
        this->type = var.type;
        this->objs = var.objs;

        // pout("GROUP :: COPY CONSTRUCTOR >>", label);
    }

    Group::Group(Group &&var) noexcept
    {
        this->label = std::move(var.label);
        this->type = var.type;
        this->objs = std::move(var.objs);

        var.objs.clear();

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
        return objs.find(label) != objs.end();
    }

    Group &Group::addGroup(const std::string &label)
    {
        assert(label.size() < MAX_LABEL_SIZE);
        assert(objs.find(label) == objs.end());

        Group *ptr = new Group(label);
        objs.emplace(label, std::move(ptr));
        return *ptr;
    }

    void Group::remove(const std::string &label)
    {
        assert(label.size() < MAX_LABEL_SIZE);
        assert(objs.find(label) != objs.end());
        objs.erase(label);
    }

}