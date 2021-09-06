#include "object.h"

namespace GDM
{
    void Object::rename(const std::string &name)
    {
        if (parent)
        {
            parent->m_children[name] = this;
            parent->m_children[label] = nullptr; // we don't want to destroy "this"
            parent->remove(this->label);
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

    Data::Data(const std::string &name, Type type) : Object(name, type) {}

    Data::~Data(void)
    {
        if (isLoaded())
            delete[] buffer;
    }

    Data::Data(const Data &var)
    {
        this->label = var.label;
        this->parent = var.parent;

        this->type = var.type;
        this->numBytes = var.numBytes;
        this->shape = var.shape;

        this->offset = var.offset;
        this->gdmFile = var.gdmFile;

        if (var.isLoaded())
        {
            this->buffer = new uint8_t[this->numBytes];
            std::copy(var.buffer, var.buffer + var.numBytes, this->buffer);
        }

        this->m_description = var.m_description; // This is a simple map, we don't need anything fancy

    }

    Data::Data(Data &&var) noexcept
    {
        this->label = std::move(var.label);
        this->parent = var.parent;

        this->type = var.type;
        this->numBytes = var.numBytes;
        this->shape = var.shape;

        this->offset = var.offset;
        this->gdmFile = var.gdmFile;

        this->m_description = std::move(var.m_description); // This is a simple map, so we don't need anything fancy

        // if var.buffer is nullptr, there is no point in moving anything, otherwise the offset takes care of it
        if (var.isLoaded()) 
            this->buffer = std::move(var.buffer);

        var.shape = {0, 0};
        var.numBytes = 0;
        var.buffer = nullptr;
        
        var.parent = nullptr;
        var.gdmFile = nullptr;
    }

    Data &Data::operator=(const Data &var)
    {
        if (&var != this)
        {   
            // we want to keep original label and parent
            std::string name = this->label; 
            Group *par = this->parent;

            this->~Data();
            new (this) Data(var);
            
            // Resetting original label and parent
            this->label = name;
            this->parent = par;
        }

        return *this;
    }

    Data &Data::operator=(Data &&var) noexcept
    {
        if (&var != this)
        {
            // We want to keep original label and parent
            std::string name = this->label; 
            Group *par = this->parent;

            this->~Data();
            new (this) Data(std::move(var));

            // resetting to original label and parent
            this->label = name;
            this->parent = par;
        }

        return *this;
    }

    GDM_API void Data::load(void)
    {
        if (buffer)
            return;

        uint64_t pos = offset;

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

    uint8_t *Data::getRawBuffer(void)
    {
        if (!buffer)
            load();

        return buffer;
    }

    void Data::release(void)
    {
        if (buffer && (numBytes > sizeof(getNumBytes(type)))) // We can only release data if if contains more than 8 bytes
        {
            delete[] buffer;
            buffer = nullptr;
            // the offset in the inpnut file should still be set, hence we can re-import this data if necessary
        }
    }

    /*********************************************************************************************/
    /*********************************************************************************************/
    /*********************************************************************************************/

    Group::Group(const std::string &name) : Object(name, Type::GROUP) {}

    Group::~Group(void)
    {
        for (auto [label, ptr] : m_children)
            delete ptr;

        m_children.clear();
    }


    Group::Group(const Group &var) 
    {
        this->label = var.label;
        this->parent = var.parent;
        this->type = Type::GROUP;

        this->m_description = var.m_description;

        for (auto [label, obj] : var.m_children)
        {
            if (obj->getType() == Type::GROUP)
            {
                const Group &group = reinterpret_cast<Group &>(*obj);
                Group *another = new Group(group);
                another->parent = this;
                m_children.emplace(group.getLabel(), std::move(another));
            }
            else
            {
                const GDM::Data &data = reinterpret_cast<Data &>(*obj);
                this->copyData(data);
            }
        }
    }

     Group::Group(Group &&var) noexcept
     {
         this->label = var.label;
         this->parent = var.parent;
         this->type = Type::GROUP;

         this->m_description = std::move(var.m_description);

         for (auto [label, obj] : var.m_children)
         {
             if (obj->getType() == Type::GROUP)
             {
                 Group& group = reinterpret_cast<Group&>(*obj);
                 Group* another = new Group(std::move(group));
                 another->parent = this;
                 m_children.emplace(group.getLabel(), std::move(another));
             }
             else
             {
                 GDM::Data& data = reinterpret_cast<Data&>(*obj);

                 Data* ptr = new Data(std::move(data));
                 ptr->parent = this;
                 m_children.emplace(label, std::move(ptr));
             }

            var.m_children.at(label) = nullptr;
         }

         var.m_children.clear();
     }

    Group &Group::operator=(const Group &var)
    {
        if (&var != this)
        {
            std::string name = this->label;
            Group *par = this->parent;

            this->~Group();
            new (this) Group(var);

            this->label = name;
            this->parent = par;
        }

        return *this;
    }

    Group &Group::operator=(Group &&var) noexcept
    {
        if (&var != this)
        {
            std::string name = this->label;
            Group* par = this->parent;

            this->~Group();
            new (this) Group(std::move(var));

            this->label = name;
            this->parent = par;
        }

        return *this;
    }

    /*******************************************************************************************/

    bool Group::contains(const std::string &label) const
    {
        assert(label.size() < MAX_LABEL_SIZE);
        return m_children.find(label) != m_children.end();
    }

    Group &Group::addGroup(const std::string &label)
    {
        uint64_t pos = label.find_last_of('/');

        Group *grp = this;
        if (pos != std::string::npos)
            grp = reinterpret_cast<Group *>(getObject(label.substr(0, pos)));


        const std::string name = label.substr(pos+1);
        assert(name.size() < MAX_LABEL_SIZE);

        Group* ptr = new Group(name);
        ptr->parent = grp;
        grp->m_children.emplace(name, std::move(ptr));

        return *ptr;
    }

    void Group::copyData(const Data& data)
    {
        Data *ptr = new Data(data);
        ptr->parent = this;
        m_children.emplace(ptr->getLabel(), std::move(ptr));
    }

    void Group::importData(Data &data)
    {
        Group *par = data.parent;
        std::string label = data.getLabel();

        Data *ptr = new Data(std::move(data));
        ptr->parent = this;
        m_children.emplace(label, std::move(ptr));

        par->m_children.at(label) = nullptr;
        par->m_children.erase(label);

    }

    Group &Group::getGroup(const std::string &label) 
    {
        Object *obj = getObject(label);

        assert(obj->getType() == Type::GROUP);
        return reinterpret_cast<Group &>(*obj); 
    }

    const Group &Group::getGroup(const std::string &label) const 
    {
        const Object *obj = getObject(label);

        assert(obj->getType() == Type::GROUP);
        return reinterpret_cast<const Group &>(*obj); 
    }

    Data &Group::getData(const std::string &label) 
    {
        Object *obj = getObject(label);

        assert(obj->getType() != Type::GROUP);
        return reinterpret_cast<Data &>(*obj); 
    }

    const Data &Group::getData(const std::string &label) const 
    {
        const Object *obj = getObject(label);

        assert(obj->getType() != Type::GROUP);
        return reinterpret_cast<const Data &>(*obj);
    }

    GDM_API void Group::clear(void) 
    {
        m_children.clear(); 
        m_description.clear();
    }

    void Group::remove(const std::string &label)
    {
        assert(label.size() < MAX_LABEL_SIZE);
        auto it = m_children.find(label);
        assert(it != m_children.end());
        m_children.erase(it);
    }

    Object *Group::getObject(const std::string &label)
    {
        uint64_t pos = label.find_first_of('/');

        if (pos != std::string::npos)
        {
            const std::string &bot = label.substr(0, pos);
            const std::string &top = label.substr(pos+1);

            auto out = m_children.find(bot);
            assert(out != m_children.end()); // Does it exist?

            Group *grp = reinterpret_cast<Group*>(out->second);
            return grp->getObject(top);
        }

        auto out = m_children.find(label);
        assert(out != m_children.end()); // Does it exist?
        return out->second;
    }

    const Object *Group::getObject(const std::string &label) const
    {
        uint64_t pos = label.find_first_of('/');

        if (pos != std::string::npos)
        {
            const std::string &bot = label.substr(0, pos);
            const std::string &top = label.substr(pos+1);

            auto out = m_children.find(bot);
            assert(out != m_children.end()); // Does it exist?

            const Group *grp = reinterpret_cast<const Group*>(out->second);
            return grp->getObject(top);
        }

        auto out = m_children.find(label);
        assert(out != m_children.end()); // Does it exist?
        return out->second;
    }

}