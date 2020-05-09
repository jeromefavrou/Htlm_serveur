#ifndef EXCHANGER_HPP_INCLUDED
#define EXCHANGER_HPP_INCLUDED

#ifndef EXCHANGER_VERSION
#define EXCHANGER_VERSION 0x01
#endif // EXCHANGER_VERSION

#include "Object.hpp"
#include <memory>
#include <map>


class Exchanger
{
public:

    class Exchanger_Error : public Error
    {
    public:
        Exchanger_Error(int numero, std::string const& _str,level _level)throw():Error(numero,_str,_level){this->m_class="Exchanger::Error";};
        virtual ~Exchanger_Error(){};
    };

    struct Header
    {
        float trame_verbose;
        byte writer_id;
        byte reader_id;
        unsigned int trame_size;
    };

    typedef enum class DIR : byte const {INPUT, OUTPUT} direction;

    Exchanger(byte const & _Master_id,byte const & _Slave_id,bool const &_master);
    ~Exchanger(void);

    byte get_master_id(void)const;
    byte get_slave_id(void)const;

    std::shared_ptr<Object> get_shared_obj(byte const & _addr,direction const &_dir);
    Object & get_robj(byte const & _addr,direction const &_dir);
    Object get_obj(byte const & _addr,direction const &_dir);

    /*template<typename T> void set_var(byte const & _addr,T const & _var);
    template<typename T> T get_var(byte const & _addr) const;
    template<typename T> T & var(byte const & _addr);*/

    void read(Tram & _data);
    Tram write(void);

    void read_data(VCHAR & _data);
    VCHAR write_data(void);

    void create(byte const & _addr,Object::type const & _type,direction const &_dir,Object::right  _right);
    void insert(std::shared_ptr<Object> const & _obj,direction const &_dir);
    void erase(byte const & _addr,direction const &_dir);
    void clear(direction const &_dir);

private:

    bool check_free_addr(byte _addr,direction _dir);
    VCHAR::iterator read_header(VCHAR  & _data);
    void read_var(VCHAR::iterator & _It);

    struct Var
    {
        std::map<byte,std::shared_ptr<Object>> in;
        std::map<byte,std::shared_ptr<Object>> out;
    };

    struct Var m_var;

    byte const m_master_id;
    byte const m_slave_id;
    bool const m_ifmaster;

    struct Header m_last_head;
};


#endif // EXCHANGER_HPP_INCLUDED
