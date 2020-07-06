#ifndef TH_COM_ATMEGA328_HPP_INCLUDED
#define TH_COM_ATMEGA328_HPP_INCLUDED

#include "uart.hpp"
#include "Exchanger.hpp"
#include <chrono>
#include <thread>
#include <string>
#include <sstream>



void extract_input(Tram & _tram,Exchanger & _in)
{
    if(_tram.size()<=0)
        return;

    if(_tram.get_data().front()!=0x00)
        throw Error(1,"serial input invalid first byte",Error::level::WARNING);

    if(_tram.get_data().back()!=(byte)Tram::Com_bytes::EOT)
        throw Error(1,"serial input invalid end byte",Error::level::WARNING);

    _tram.get_data().erase(_tram.get_data().begin());
    _tram.get_data().pop_back();

    try
    {
        _in.read(_tram);
    }
    catch(Error & e)
    {
        std::cerr <<e.what() << std::endl;

        return;
    }
}

void insert_output(Tram & _tram,Exchanger & _out)
{
    _tram+=0x00;

    _tram+= _out.write();

    _tram+=(byte)Tram::Com_bytes::EOT;
}

void th_lp_wr(bool & Cs,Serial & com ,bool & sync_serial ,Exchanger & _Variable)
{
    int len(0);

    std::stringstream flux;

    while(Cs)
    {
        std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(1000));

        flux.clear();

        try
        {
            Tram caster;

            sync_serial=true;

            insert_output(caster,_Variable);

//////            /*for(auto & i : caster.get_data())
//////                std::cout << "0x"<<std::hex<<(int) i<< " ";
//////            std::cout << std::dec << std::endl;*/

            if(caster.size() > sizeof(Exchanger::Header))
                com.write(caster);


            _Variable.get_robj(0x01,Exchanger::DIR::OUTPUT).RW_right=Object::RIGHT::NOT;
            _Variable.get_robj(0x02,Exchanger::DIR::OUTPUT).RW_right=Object::RIGHT::NOT;
            _Variable.get_robj(0x03,Exchanger::DIR::OUTPUT).RW_right=Object::RIGHT::NOT;

            caster.clear();

            std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(50));

            len=com.availble();

            sync_serial=false;

            if(len<=0 || sync_serial)
                continue;

            _Variable.get_robj(0x7F,Exchanger::DIR::OUTPUT).RW_right=Object::RIGHT::NOT;
            _Variable.get_robj(0x7F,Exchanger::DIR::OUTPUT)=Tram::cast_to_vchar<byte>(0x00);

            Error::add_to_log("serial bytes received: "+ ss_cast<int,std::string>(len));

            sync_serial=true;

            for(;len>0;len--)
                caster+=com.read_byte();

            com.flush();

            std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(10));

            extract_input(caster,_Variable);

            caster.clear();

            Error::add_to_log("Slave bytes ram memory avaible: "+ ss_cast<int,std::string>(Tram::cast_to_type<int>(_Variable.get_obj(0x03,Exchanger::direction::INPUT).get_obj())));//sur watch dog

            if(Tram::cast_to_type<byte>(_Variable.get_obj(0x02,Exchanger::direction::INPUT).get_obj())!=0x00)
                Error::add_to_log("Slave num_error: "+ ss_cast<byte,std::string>(Tram::cast_to_type<byte>(_Variable.get_obj(0x02,Exchanger::direction::INPUT).get_obj())));//sur watch dog

            sync_serial=false;
        }
        catch(Error & e)
        {
            sync_serial=false;

            std::cerr << e.what() << std::endl;

            if(e.get_level()==Error::level::FATAL_ERROR)
            {
                Cs=false;
                break;
            }
        }

    }
}


#endif // TH_COM_ATMEGA328_HPP_INCLUDED
