#ifndef TRAM_HPP_INCLUDED
#define TRAM_HPP_INCLUDED

#include <vector>
#include <string>
#include <sstream>
#include "Error.hpp"

typedef unsigned char byte;

typedef std::vector<byte> VCHAR;

class Tram
{
    public:

        class Erreur : public Error
        {
        public:
            Erreur(int numero, std::string const& _str,level _level)throw():Error(numero,_str,_level){this->m_class="Tram::Error";};
            virtual ~Erreur(){};
        };
        //ascii correspondance

        class Com_bytes
        {
        public:
            static byte constexpr SOH=byte(0x01); //Start of Heading
            static byte constexpr EOT=byte(0x04); //End of Transmission
            static byte constexpr ENQ=byte(0x05); //Enquiry
            static byte constexpr ACK=byte(0x06); //Acknowledge
            static byte constexpr NAK=byte(0x15); //Negative Acknowledge
            static byte constexpr US=byte(0x1F); //Unit Separator
            static byte constexpr GS=byte(0x1D); //Group Separator
            static byte constexpr CS=byte(0x00); //nullprt
        };

        Tram(void);
        Tram(Tram const &);
        Tram(std::string const &);
        Tram(VCHAR const &);

        Tram operator=(Tram const & );
        Tram operator=(std::string const &);
        Tram operator=(VCHAR const &);

        Tram operator+(Tram const &);
        Tram operator+(std::string const &);
        Tram operator+(VCHAR const &);
        Tram operator+(byte const &);

        Tram operator+=(Tram const &);
        Tram operator+=(std::string const &);
        Tram operator+=(VCHAR const &);
        Tram operator+=(byte const &);

        void clear() noexcept;
        size_t size() const noexcept;

        VCHAR  & get_data(void);

        VCHAR get_c_data(void) const;

        std::stringstream get_ss_data(void);

        template<typename T> static VCHAR cast_to_vchar(T const & _type)
        {

            VCHAR tps(sizeof(T));

            long fl=*((long *) &_type);

            for(auto i=0u;i<sizeof(T);i++)
                tps[i]= fl >> 8*(sizeof(T)-1-i) & 0xFF ;

            return tps;
        }
        template<typename T> static T cast_to_type(VCHAR const & _data)
        {
            if(sizeof(T)!=_data.size())
                throw Tram::Erreur(0,"cast_to_type(VCHAR const & _data) sizeof(T)!=_data.size() :"
                +ss_cast<long,std::string>(sizeof(T))+"!="+ss_cast<long,std::string>(_data.size()),Tram::Erreur::level::ERROR);

            T tps(0);

            long fl(0);

            for(auto & d:_data)
                fl = fl << 8 | d;

            tps  = * ( T * ) &fl;

            return tps;
        }

    private:

        VCHAR m_data;

};

#endif // TRAM_HPP_INCLUDED
