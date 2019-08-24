#include "uart.hpp"

Serial::Serial(std::string const & __driver_path, Baud_Rate const & __speed):m_speed(-2),m_file_desciptor(-1),m_stat(-1),m_file_desciptor_check(false),m_baud_rate_check(false)
{
    struct termios options;

    this->m_speed=this->cast_baud_rate(__speed);

    this->m_baud_rate_check= this->m_speed == -2 ? false : true ;

    this->m_file_desciptor = open (__driver_path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

    this->m_file_desciptor_check = this->m_file_desciptor==-1 ? false : true ;

    fcntl (this->m_file_desciptor, F_SETFL, O_RDWR) ;

    tcgetattr(this->m_file_desciptor, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, this->m_speed) ;
    cfsetospeed (&options, this->m_speed) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag &= ~TIOCM_DTR;
    options.c_cflag |= CS8 ;
    options.c_cflag &= ~CRTSCTS;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 100 ;	// Ten seconds (100 deciseconds)

    tcsetattr (this->m_file_desciptor, TCSANOW | TCSAFLUSH, &options) ;

    ioctl (this->m_file_desciptor, TIOCMGET, &this->m_stat);

    this->m_stat |= TIOCM_DTR ;
    this->m_stat |= TIOCM_RTS ;

    ioctl (this->m_file_desciptor, TIOCMSET, &this->m_stat);

    std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(10));
}

void Serial::flush (void)
{
    this->throwing();

    tcflush (this->m_file_desciptor, TCIOFLUSH) ;
}

void Serial::write(std::string const & __str)
{
    this->throwing();

    ::write(this->m_file_desciptor,(char*) __str.c_str() , __str.size()*sizeof(char)) ;
}


char Serial::read_c (void)
{
    this->throwing();

    uint8_t x ;

    if (::read (this->m_file_desciptor, &x, 1) != 1)
    return -1 ;

    return static_cast<char>(((int)x) & 0xFF);
}

std::string Serial::read(void)
{
    this->throwing();

    return "";
}


int Serial::availble(void)
{
    this->throwing();

    int result ;

    if (ioctl (this->m_file_desciptor, FIONREAD, &result) == -1)
        return -1 ;

    return  result;
}

speed_t Serial::cast_baud_rate(Baud_Rate const & __speed)
{
    switch ( static_cast<unsigned int>(__speed) )
    {
        case     50:	return       B50 ; break ;
        case     75:	return       B75 ; break ;
        case    110:	return      B110 ; break ;
        case    134:	return      B134 ; break ;
        case    150:	return      B150 ; break ;
        case    200:	return      B200 ; break ;
        case    300:	return      B300 ; break ;
        case    600:	return      B600 ; break ;
        case   1200:	return     B1200 ; break ;
        case   1800:	return     B1800 ; break ;
        case   2400:	return     B2400 ; break ;
        case   4800:	return     B4800 ; break ;
        case   9600:	return     B9600 ; break ;
        case  19200:	return    B19200 ; break ;
        case  38400:	return    B38400 ; break ;
        case  57600:	return    B57600 ; break ;
        case 115200:	return   B115200 ; break ;
        case 230400:	return   B230400 ; break ;

        default:
          return -2 ;
    }

    return -2;
}

void Serial::throwing(void)
{
    if(!this->m_file_desciptor_check)
        throw Serial_Error(1,"this driver_file cannot be opened",Error::level::ERROR);

    if(!this->m_baud_rate_check)
        throw Serial_Error(2,"this baude rate cannot be used ",Error::level::ERROR);
}

Serial::~Serial()
{
    close (this->m_file_desciptor);
}
