#include "Error.hpp"

Error::Error(int numero, std::string const& _str,level _level) throw():m_numero(numero),m_str(_str),m_level(_level),m_class("Error"),m_log_stat(true)
{

}
std::string Error::level_to_str(void)
{
    switch(this->m_level)
    {
        case level::WARNING: return "WARNING"; break;
        case level::ERROR: return "ERROR"; break;
        case level::FATAL_ERROR: return "FATAL ERROR"; break;
        default : return "UNKNOW";
    }

    return "UNKNOW";
}
std::string Error::what() throw()
{
    std::string str_error("");
    std::string str("");

    switch(this->m_level)
    {
        case level::WARNING: str= _print<unix_color::GREEN>("WARNING"); break;
        case level::ERROR: str= _print<unix_color::BLUE>("ERROR"); break;
        case level::FATAL_ERROR: str= _print<unix_color::RED>("FATAL ERROR"); break;
        default : str="WARNING";
    }

    str_error="("+this->m_class+") ";
    str_error+=str+" [";
    str_error+=ss_cast<int,std::string>(this->m_numero)+"]: "+this->m_str;

    return str_error;
}
std::string Error::get_str(void) const
{
    return this->m_str;
}

std::string Error::get_class(void) const
{
    return this->m_class;
}

int Error::get_num(void) const
{
    return this->m_numero;
}

void Error::set_log_stat(bool const & _stat)
{
    this->m_log_stat=_stat;
}

Error::level Error::get_level(void) const
{
    return this->m_level;
}

Error::~Error() throw()
{
    if(!this->m_log_stat)
        return ;

    std::fstream Of(log_path,std::ios::out | std::ios::app);

    std::time_t end_time=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    if(Of && !Of.fail() && !Of.bad())
        Of << std::ctime(&end_time) <<"(" << this->m_class<< ")[" <<this->m_numero << "][" << this->level_to_str()<< "] " << this->m_str << "\n";
}
