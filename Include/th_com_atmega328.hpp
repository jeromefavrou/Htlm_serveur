#ifndef TH_COM_ATMEGA328_HPP_INCLUDED
#define TH_COM_ATMEGA328_HPP_INCLUDED

#include "uart.hpp"
#include <chrono>
#include <thread>
#include <string>
#include <sstream>

struct stepper_assign
{
    byte direction=0x00;
    byte stat=0x00;
    byte speed=0x00;
};

struct output_var
{
    struct stepper_assign stepper;
    std::array<byte,6> out_id;
    std::array<byte,6> out_stat;
    std::array<byte,6> out_reg;
    std::array<byte,4> sensor_reg;
    std::array<byte,4> direction_reg;
    std::array<float,4> order_reg;
    std::array<float,4> proportional_reg;
    std::array<float,4> intergrate_reg;
    std::array<float,4> derivate_reg;
};

struct AM
{
    float humidity;
    float temperate;
};
struct input_var
{
    AM am2320;
    std::array<float,4> thermistor;
};

struct arduino_tram
{
    struct output_var out;
    struct input_var in;
};

typedef struct arduino_tram Uart_atemga_tram;

void extract_input(Tram & _tram,struct input_var  & _in)
{
    if(_tram.size()!=26)
        throw Error(1,"serial input invalid size",Error::level::WARNING);

    if(_tram.get_data().front()!=0x00)
        throw Error(1,"serial input invalid first byte",Error::level::WARNING);

    if(_tram.get_data().back()!=(byte)Tram::Com_bytes::EOT)
        throw Error(1,"serial input invalid end byte",Error::level::WARNING);

    _in.am2320.temperate=Tram::cast_to_type<float>(VCHAR(_tram.get_data().begin()+0x01,_tram.get_data().begin()+0x05));
    _in.am2320.humidity=Tram::cast_to_type<float>(VCHAR(_tram.get_data().begin()+0x05,_tram.get_data().begin()+0x09));

    for(auto i=0u;i<_in.thermistor.size();i++)
        _in.thermistor[i]=Tram::cast_to_type<float>(VCHAR(_tram.get_data().begin()+0x09+i*sizeof(float),_tram.get_data().begin()+0x0D+i*sizeof(float)));
}

void insert_output(Tram & _tram,struct output_var & _out)
{
    _tram+=VCHAR{0x00,_out.stepper.stat,_out.stepper.direction,_out.stepper.speed,Tram::Com_bytes::GS};//stepper assign  stat/dir/speed

    _tram+=0x01;//out id assign
    _tram+=VCHAR(_out.out_id.begin(),_out.out_id.end());
    _tram+=(byte)Tram::Com_bytes::GS;

    _tram+=0x02;//out stat assign
    _tram+=VCHAR(_out.out_stat.begin(),_out.out_stat.end());
    _tram+=(byte)Tram::Com_bytes::GS;

    _tram+=0x03;//out reg assign
    _tram+=VCHAR(_out.out_reg.begin(),_out.out_reg.end());
    _tram+=(byte)Tram::Com_bytes::GS;

    _tram+=0x04;//sensor reg assign
    _tram+=VCHAR(_out.sensor_reg.begin(),_out.sensor_reg.end());
    _tram+=(byte)Tram::Com_bytes::GS;

    _tram+=0x05;//direction reg assign
    _tram+=VCHAR(_out.direction_reg.begin(),_out.direction_reg.end());
    _tram+=(byte)Tram::Com_bytes::GS;

    _tram+=0x06;//order reg value
    for(auto & val : _out.order_reg)
        _tram+=Tram::cast_to_vchar<float>(val);
    _tram+=(byte)Tram::Com_bytes::GS;

    _tram+=0x07;//Proportional reg value
    for(auto & val : _out.proportional_reg)
        _tram+=Tram::cast_to_vchar<float>(val);
    _tram+=(byte)Tram::Com_bytes::GS;

    _tram+=0x08;//integral reg value
    for(auto & val : _out.intergrate_reg)
        _tram+=Tram::cast_to_vchar<float>(val);
    _tram+=(byte)Tram::Com_bytes::GS;

    _tram+=0x09;//derivate reg value
    for(auto & val : _out.derivate_reg)
        _tram+=Tram::cast_to_vchar<float>(val);
    _tram+=(byte)Tram::Com_bytes::GS;
    _tram+=(byte)Tram::Com_bytes::EOT;
}

void th_lp_wr(bool & Cs,Serial & com ,bool & sync_serial ,Uart_atemga_tram & var)
{
    int len(0);

    std::stringstream flux;

    while(Cs)
    {
        std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(3000));

        flux.clear();

        try
        {
            len=com.availble();

            if(len<=0 || sync_serial)
                continue;

            Error::add_to_log("serial bytes received: "+ ss_cast<int,std::string>(len));

            Tram caster;

            sync_serial=true;

            for(;len>0;len--)
                caster+=com.read_byte();

            com.flush();

            std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(50));

            extract_input(caster,var.in);

            caster.clear();

            //insert_output(caster,var.out);

            //com.write(caster);
            //com.flush();

            std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(50));

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
