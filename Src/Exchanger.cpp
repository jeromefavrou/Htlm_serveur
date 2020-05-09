#include "Exchanger.hpp"

Exchanger::Exchanger(byte const & _Master_id,byte const & _Slave_id,bool const& _master):m_master_id(_Master_id),m_slave_id(_Slave_id),m_ifmaster(_master)
{
    this->m_last_head.trame_verbose=0;
    this->m_last_head.reader_id=0;
    this->m_last_head.writer_id=0;
    this->m_last_head.trame_size=0;
}

Exchanger::~Exchanger(void){}

byte Exchanger::get_master_id(void)const
{
    return this->m_master_id;
}

byte Exchanger::get_slave_id(void)const
{
    return this->m_slave_id;
}

std::shared_ptr<Object> Exchanger::get_shared_obj(byte const & _addr,direction const &_dir)
{
    if(this->check_free_addr(_addr,_dir))
        throw Exchanger_Error(3,"get partage impossible la variable n'existe pas",Error::level::WARNING);

    if(_dir == DIR::INPUT)
        return this->m_var.in[_addr];
    else if(_dir == DIR::OUTPUT)
        return this->m_var.out[_addr];

    return nullptr;
}

Object & Exchanger::get_robj(byte const & _addr,direction const &_dir)
{
    if(this->check_free_addr(_addr,_dir))
        throw Exchanger_Error(4,"get referencé impossible la variable n'existe pas",Error::level::WARNING);

    if(_dir == DIR::INPUT)
        return *this->m_var.in[_addr];
    else if(_dir == DIR::OUTPUT)
        return *this->m_var.out[_addr];

}
Object Exchanger::get_obj(byte const & _addr,direction const & _dir)
{
    if(this->check_free_addr(_addr,_dir))
        throw Exchanger_Error(5,"get impossible la variable n'existe pas",Error::level::WARNING);

    if(_dir == DIR::INPUT)
        return *this->m_var.in[_addr];
    else if(_dir == DIR::OUTPUT)
        return *this->m_var.out[_addr];
}

void Exchanger::create(byte const & _addr,Object::type const & _type,direction const &_dir,Object::right _right=Object::RIGHT::RW)
{
    if(!this->check_free_addr(_addr,_dir))
        throw Exchanger_Error(0,"creation impossible la variable existe déja",Error::level::WARNING);

    if(_dir == DIR::INPUT)
        this->m_var.in[_addr]=std::make_shared<Object>(_addr,_type,_right);
    else if(_dir == DIR::OUTPUT)
        this->m_var.out[_addr]=std::make_shared<Object>(_addr,_type,_right);
}

void Exchanger::insert(std::shared_ptr<Object> const & _obj,direction const &_dir)
{
    if(!this->check_free_addr(_obj->get_addr(),_dir))
        throw Exchanger_Error(1,"insertion impossible la variable existe déja",Error::level::WARNING);

    if(_dir == DIR::INPUT)
        this->m_var.in[_obj->get_addr()]=_obj;
    else if(_dir == DIR::OUTPUT)
        this->m_var.out[_obj->get_addr()]=_obj;
}

void Exchanger::erase(byte const & _addr,direction const &_dir)
{
    if(this->check_free_addr(_addr,_dir))
        throw Exchanger_Error(2,"suppression impossible la variable n'existe pas",Error::level::WARNING);

    if(_dir == DIR::INPUT)
        this->m_var.in.erase(_addr);
    else if(_dir == DIR::OUTPUT)
        this->m_var.out.erase(_addr);
}

void Exchanger::clear(direction const &_dir)
{
    if(_dir == DIR::INPUT)
        this->m_var.in.clear();
    else if(_dir == DIR::OUTPUT)
        this->m_var.out.clear();
}

void Exchanger::read(Tram & _data)
{
    this->read_data(_data.get_data());
}

void Exchanger::read_data(VCHAR & _data)
{
    auto It=this->read_header(_data);

    for(;It!=_data.end();)
    {
        try
        {
            this->read_var(It);
        }
        catch(Exchanger_Error & e)
        {
            std::cerr << e.what() <<std::endl;

            if(e.get_level()!=Error::level::WARNING)
                throw Exchanger_Error(e.get_num(),e.get_str(),e.get_level());

            //e.set_log_stat(false);
        }
        catch(Object::Object_Error & e)
        {
            //std::cerr << e.what() <<std::endl;

            ///revoir cette gestoin par rapport au num d'erreur pour le contraol size

            if(e.get_level()!=Error::level::WARNING)
                throw Exchanger_Error(e.get_num(),e.get_str(),e.get_level());

            //e.set_log_stat(false);
        }
    }
}

Tram Exchanger::write(void)
{
    return Tram(this->write_data());
}

VCHAR Exchanger::write_data(void)
{
    VCHAR data(0);

    for(auto & it  : this->m_var.out)
    {
        if(it.second->RW_right==Object::RIGHT::R || it.second->RW_right==Object::RIGHT::NOT)
            continue;

        data.push_back(it.first); //adrr
        data.push_back(it.second->size()); // size

        for(auto & it2 : it.second->get_obj())
            data.push_back(it2); //data
    }


    Tram header;

    header+=Tram::cast_to_vchar<float>(0.01);//version
    header+=Tram::cast_to_vchar<byte>(this->m_master_id);//writer_id
    header+=Tram::cast_to_vchar<byte>(this->m_slave_id);//reader_id
    header+=Tram::cast_to_vchar<unsigned int>(data.size()+header.size()+sizeof(unsigned int));//size

    data.insert(data.begin(),header.get_data().begin(),header.get_data().end());

    return data;
}

VCHAR::iterator Exchanger::read_header(VCHAR & _data)
{
    struct Header head;

    auto It=_data.begin();

    if(_data.size()<10)
        throw Exchanger_Error(6,"Trame trop courte pour contenir une entete (taille: "+ss_cast<size_t,std::string>(_data.size())+" < 10)",Error::level::ERROR);

    head.trame_verbose=Tram::cast_to_type<float>(VCHAR(It,It+sizeof(float)));
    It+=sizeof(float);

    if(head.trame_verbose<0.009)
        throw Exchanger_Error(7,"La version de la trame ne peut etre lu (version: "+ss_cast<float,std::string>(head.trame_verbose)+"< 0.009)",Error::level::ERROR);

    head.writer_id=*It;
    It+=sizeof(byte);

    if(((head.writer_id!=this->m_slave_id) && this->m_ifmaster) || ((head.writer_id!=this->m_master_id) && !this->m_ifmaster))
        throw Exchanger_Error(8,"la trame n'est pas du bon destinaire (emmeteur id: "+ss_cast<int,std::string>(head.writer_id)+")",Error::level::WARNING);

    head.reader_id=*It;
    It+=sizeof(byte);

    if((head.reader_id!=this->m_master_id && this->m_ifmaster) || (head.reader_id!=this->m_slave_id && !this->m_ifmaster) )
        throw Exchanger_Error(9,"la trame n'est pas addresse a cette instance (destinataire: "+ss_cast<int,std::string>(head.reader_id)+")",Error::level::WARNING);

    head.trame_size=Tram::cast_to_type<unsigned int>(VCHAR(It,It+sizeof(unsigned int)));
    It+=sizeof(unsigned int);

    if(head.trame_size!=_data.size())
        throw Exchanger_Error(10,"taille trame indiquer et taille reel non correspondantent (taille: "+ss_cast<unsigned int,std::string>(head.trame_size)+" != "+ss_cast<size_t,std::string>(_data.size())+")",Error::level::ERROR);

    this->m_last_head=head;

    return It;
}

void Exchanger::read_var(VCHAR::iterator & _It)
{
    byte addr=*_It;
    _It++;

    byte size=*_It;
    _It++;

    if(this->check_free_addr(addr,DIR::INPUT))
    {
        _It+=size;
        throw Exchanger_Error(11,"lecture impossible la variable n'existe pas donc ignorer (addr: "+ss_cast<short,std::string>(addr)+", taille: "+ss_cast<short,std::string>(size)+")",Error::level::WARNING);
    }

    if(this->m_var.in[addr]->RW_right==Object::RIGHT::RW || this->m_var.in[addr]->RW_right==Object::RIGHT::R)
        *this->m_var.in[addr]=VCHAR(_It,_It+size);

    _It+=size;


}


bool Exchanger::check_free_addr(byte _addr,direction _dir)
{
    if(_dir == DIR::INPUT)
        return this->m_var.in.find(_addr)==this->m_var.in.end()?true:false;
    else if(_dir == DIR::OUTPUT)
        return this->m_var.out.find(_addr)==this->m_var.out.end()?true:false;

    return false;
}
