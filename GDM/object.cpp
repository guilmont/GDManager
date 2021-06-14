#include "object.h"

namespace GDM
{
    Data::Data(const Data& var)
    {
        this->label = var.label;
        this->type = var.type;
        this->objs = var.objs;
        this->numBytes = var.numBytes;
        this->shape = var.shape;

        this->buffer = new uint8_t[this->numBytes];
        std::copy(var.buffer, var.buffer + var.numBytes, this->buffer);

        pout("DATA :: COPY CONSTRUCTOR >>", label);
    }


    Data::Data(Data&& var) noexcept
    {
        this->label = std::move(var.label);
        this->type = var.type;
        this->objs = std::move(var.objs);
        this->numBytes = var.numBytes;
        this->shape = var.shape;
        this->buffer = std::move(var.buffer);

        // manually emptying the other class
        var.objs.clear();

        var.shape = { 0,0 };
        var.numBytes = 0;
        var.buffer = nullptr;

        pout("DATA :: MOVE CONSTRUCTOR >>", label);

    }

    Data& Data::operator=(const Data& var)
    {
        pout("DATA::ASSIGN OPERATOR >>", label);
        if(&var!= this)
            new (this) Data(var);

        return *this;

    }

    Data& Data::operator=(Data&& var) noexcept
    {
        pout("DATA::MOVE OPERATOR >>", label);

        if (&var != this)
            new (this)Data(std::move(var));

        return *this;
    }

    Data::~Data(void)
    {
        objs.clear();
        if (buffer)
        {
            pout("DATA :: DESTRUCTOR >>", label);
            delete[] buffer;
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    bool Data::contains(const std::string &label) const
    {
        assert(type == Type::GROUP);
        return objs.find(label) != objs.end();
    }

    Data &Data::operator[](const std::string &label)
    {
        assert(objs.find(label) != objs.end());
        return objs.at(label);
    }

    const Data &Data::operator[](const std::string &label) const
    {
        assert(objs.find(label) != objs.end());
        return objs.at(label);
    } 

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    Data &Data::addGroup(const std::string &label)
    {
        assert(type == Type::GROUP && objs.find(label) == objs.end());
        objs.emplace(label, Data(label, Type::GROUP));
        return objs[label];
    }
    void Data::remove(const std::string& label)
    {
        // checking that object is present
        assert(objs.find(label) != objs.end());
        objs.erase(label);
    }


}