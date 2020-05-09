#include "Object.hpp"

Object::Object(byte const & _addr,type const & _type,right _right=Object::RIGHT::RW):m_addr(_addr),m_type_decrp(_type),RW_right(_right),m_size(Object::size_of_type(_type)),m_data(VCHAR(Object::size_of_type(_type)))
{

}
/*Object::Object():m_addr(0x00),m_type_decrp(TYPE::BYTE),RW_right(Object::RIGHT::RW),m_size(0),m_data(VCHAR(0))
{
}*/

Object::~Object(void){}

Object Object::operator=(VCHAR const & _value)
{
    if(_value.size()!=Object::size_of_type(this->m_type_decrp) && this->m_type_decrp!=TYPE::STRING)
        throw Object_Error(0x01,"taille diferrante du type attendu",Error::level::ERROR);

    this->m_data=_value;
    this->m_size=this->m_data.size();

    return *this;
}
bool Object::operator<(Object const& other)
{
    return this->get_addr()<other.get_addr();
}

void Object::set_obj(VCHAR const & _value)
{
    if(_value.size()!=Object::size_of_type(this->m_type_decrp) && this->m_type_decrp!=TYPE::STRING)
        throw Object_Error(0x01,"taille diferrante du type attendu",Error::level::ERROR);

    this->m_data=_value;
    this->m_size=this->m_data.size();
}

VCHAR Object::get_obj(void) const
{
    return this->m_data;
}

VCHAR & Object::obj(void)
{
    return this->m_data;
}

Object::type Object::get_type(void) const
{
    return this->m_type_decrp;
}

byte Object::get_addr(void) const
{
    return this->m_addr;
}

byte Object::size(void) const
{
    return this->m_size;
}

