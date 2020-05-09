#ifndef OBJECT_HPP_INCLUDED
#define OBJECT_HPP_INCLUDED

#include <vector>
#include "tram.hpp"

class Object
{
public:

    class Object_Error : public Error
    {
    public:
        Object_Error(int numero, std::string const& _str,level _level)throw():Error(numero,_str,_level){this->m_class="Object::Error";};
        virtual ~Object_Error(){};
    };

    typedef enum class TYPE : byte const {USHORT,SHORT,FLOAT,INT,UINT,DOUBLE,STRING,BYTE,CHAR,UCHAR,LONG,ULONG,STRUCT} type;
    typedef enum class RIGHT : byte const {RW,R,W,NOT,} right;

    Object(byte const & _addr,type const & _type,right _right);
    //Object(void);
    ~Object(void);

    Object operator=(VCHAR const & _value);
    bool operator<(Object const & other);

    void set_obj(VCHAR const & _value);
    VCHAR get_obj(void) const ;

    VCHAR & obj(void);
    type get_type(void) const ;
    byte get_addr(void) const ;

    byte size(void)const;
    static byte size_of_type(type const & _type)
    {
        switch(_type)
        {
            case TYPE::FLOAT : return sizeof(float);
            case TYPE::INT : return sizeof(int);
            case TYPE::UINT : return sizeof(unsigned int);
            case TYPE::BYTE : return sizeof(byte);
            default: return 0; break;
        }

        return 0;
    }

    right RW_right=RIGHT::RW;
private:

    VCHAR m_data;
    byte const m_addr;
    byte m_size;
    type m_type_decrp;

};

#endif // OBJECT_HPP_INCLUDED
