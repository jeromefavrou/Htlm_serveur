#include "Error.hpp"

Error::Error(int numero, std::string const& _str,level _level) throw():m_numero(numero),m_str(_str),m_level(_level)
{
    this->m_class="Error";
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

Error::level Error::get_level(void) const
{
    return this->m_level;
}

Error::~Error() throw(){}
