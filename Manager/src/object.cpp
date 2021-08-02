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

    GDM_API void Data::load(void)
    {
        if (buffer)
            return;

        uint64_t pos = offset;

        // Getting compression method
        Compression comp;
        gdmFile->seekg(pos);
        gdmFile->read(reinterpret_cast<char*>(&comp), sizeof(Compression));
        pos += sizeof(Compression);

        assert(comp == Compression::NONE); // TODO: Implement other types of compression

        // Getting compressed number of bytes -> not important for now
        uint64_t nBytes;
        gdmFile->seekg(pos);
        gdmFile->read(reinterpret_cast<char*>(&nBytes), sizeof(uint64_t));
        pos += sizeof(uint64_t);

        assert(nBytes == this->numBytes); // If not compression is used, theses values should be the same

        // Importing data bytes
        buffer = new uint8_t[numBytes];
        gdmFile->seekg(pos);
        gdmFile->read(reinterpret_cast<char*>(buffer), numBytes);
    }

    uint8_t *Data::getRawBuffer(void) 
    {
        if (!buffer)
            load();

        return buffer;
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

    /*********************************************************************************************/
    /*********************************************************************************************/
    /*********************************************************************************************/

    Group::Group(const std::string &name) : Object(name, Type::GROUP) {}

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

    /*******************************************************************************************/

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

    void Group::addGroup(Group *group)
    {
        const std::string &label = group->getLabel();
        assert(m_children.find(label) == m_children.end());

        group->parent = this;

        m_children.emplace(label, std::move(group));
    }

    void Group::copyData(Data *data)
    {
        const std::string &label = data->getLabel();
        assert(m_children.find(label) == m_children.end());

        bool loaded = data->isLoaded();

        Shape sp = data->getShape();
        
        Data* loc = new Data(label, data->getType());
        loc->parent = this;

        // Copying data
        const uint8_t* ptr = data->getRawBuffer();
        loc->buffer = new uint8_t[data->numBytes];
        std::copy(ptr, ptr + data->numBytes, loc->buffer);

        loc->shape = data->shape;
        loc->numBytes = data->numBytes;
        loc->offset = data->offset;
        loc->gdmFile = data->gdmFile;

        

        // Copying descriptions
        for (auto& [label, desc] : data->m_description)
            loc->addDescription(label, desc);

        if (!loaded)
            data->release();

        m_children.emplace(label, std::move(loc));
    }


    void Group::moveData(Data* data)
    {
        const std::string& label = data->getLabel();
        assert(m_children.find(label) == m_children.end());

        Group* other = data->parent;
        data->parent = this;

        m_children.emplace(label, std::move(data));

        other->m_children[label] = nullptr;
        other->m_children.erase(label);
        // Now we need to remove from original group
    }

    const Group &Group::getGroup(const std::string &label) const { return reinterpret_cast<const Group &>(this->operator[](label)); }
    Group &Group::getGroup(const std::string &label) { return reinterpret_cast<Group &>(this->operator[](label)); }

    const Data &Group::getData(const std::string &label) const { return reinterpret_cast<const Data &>(this->operator[](label)); }
    Data &Group::getData(const std::string &label) { return reinterpret_cast<Data &>(this->operator[](label)); }

    void Group::remove(const std::string &label)
    {
        assert(label.size() < MAX_LABEL_SIZE);
        auto it = m_children.find(label);
        assert(it != m_children.end());
        m_children.erase(it);
    }

    Object& Group::operator[](const std::string &label) {
        uint64_t
            posZero = 0,
            posEnd = label.find_first_of('/');

        const Group* obj = this;
        while (posEnd != std::string::npos)
        {
            const std::string& sub = label.substr(posZero, posEnd - posZero);
            assert(sub.size() < MAX_LABEL_SIZE);

            auto it = obj->m_children.find(sub);
            assert(it != obj->m_children.end());               // Does it exist?
            assert(it->second->getType() == Type::GROUP); // Make sure it is a group
            obj = reinterpret_cast<const Group*>(it->second);

            posZero = posEnd + 1;
            posEnd = label.find('/', posZero);
        }

        auto out = obj->m_children.find(label.substr(posZero));
        assert(out != obj->m_children.end()); // Does it exist?

        return *(out->second);
    }

    const Object& Group::operator[](const std::string& label) const { 
        uint64_t
            posZero = 0,
            posEnd = label.find_first_of('/');

        const Group* obj = this;
        while (posEnd != std::string::npos)
        {
            const std::string& sub = label.substr(posZero, posEnd - posZero);
            assert(sub.size() < MAX_LABEL_SIZE);

            auto it = obj->m_children.find(sub);
            assert(it != m_children.end());               // Does it exist?
            assert(it->second->getType() == Type::GROUP); // Make sure it is a group
            obj = reinterpret_cast<const Group*>(it->second);

            posZero = posEnd + 1;
            posEnd = label.find('/', posZero);
        }

        auto it = obj->m_children.find(label.substr(posZero));
        assert(it != m_children.end()); // Does it exist?

        return *(it->second);
    }

}