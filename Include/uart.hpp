#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

#include "utility.hpp"
#include "Error.hpp"
#include "hardware_defined.hpp"
#include "tram.hpp"

#include <inttypes.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>


class Serial : public Nocopyable
{
public:

    class Serial_Error :public Error
    {
        public:
        Serial_Error(int numero, std::string const& _str,level _level)throw():Error(numero,_str,_level){this->m_class="Serial_Error";}
        virtual ~Serial_Error(){}
    };

    Serial(std::string const & , Baud_Rate const & );

    void flush (void);

    void write(std::string const & );

    void write(VCHAR const & );

    void write(Tram const & );

    byte read_byte(void);

    int availble(void);

    ~Serial();

private:

    speed_t cast_baud_rate(Baud_Rate const & __speed);

    void throwing(void);

    bool m_file_desciptor_check,m_baud_rate_check;

    int m_file_desciptor,m_stat;

    speed_t m_speed;

};
