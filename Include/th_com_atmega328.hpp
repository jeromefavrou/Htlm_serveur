#ifndef TH_COM_ATMEGA328_HPP_INCLUDED
#define TH_COM_ATMEGA328_HPP_INCLUDED

#include "uart.hpp"
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
///*arduino trame -> hm1 tp1 tp2 tp3 tp4 tp5



struct arduino_tram
{
    std::string AM_humidity;
    std::string AM_temperature;
    std::string TERM_temperture_1;
    std::string TERM_temperture_2;
    std::string TERM_temperture_3;
    std::string TERM_temperture_4;

    std::array<std::string,6> DIGI_out;
};

typedef struct arduino_tram Uart_atemga_tram;

void th_lp_wr(bool & Cs,Serial & com ,bool & sync_serial ,Uart_atemga_tram & var)
{
    int len(0);

    std::stringstream flux;

    while(Cs)
    {
        std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(1000));

        flux.clear();

        try
        {
            len=com.availble();

            if(len<0 || !sync_serial)
                continue;

            sync_serial=true;

            for(;len>0;len--)
                flux << com.read_c();

            com.flush();

            std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(50));

            sync_serial=false;

            flux >> var.AM_humidity;
            flux >> var.AM_temperature;
            flux >> var.TERM_temperture_1;
            flux >> var.TERM_temperture_2;
            flux >> var.TERM_temperture_3;
            flux >> var.TERM_temperture_4;
        }
        catch(Error & e)
        {
            sync_serial=false;

            std::cerr << e.what() << std::endl;

            if(e.get_level()==Error::level::FATAL_ERROR)
                Cs=false;

            if(e.get_level()!=Error::level::WARNING)
                break;


        }
    }
}


#endif // TH_COM_ATMEGA328_HPP_INCLUDED
