#ifndef ERROR_HPP_INCLUDED
#define ERROR_HPP_INCLUDED

#include <exception>
#include <string>
#include "utility.hpp"

class Error
{
public:
    enum class level{WARNING,ERROR,FATAL_ERROR};

    Error(int numero, std::string const& phrase,level _level) throw();

    std::string what() throw();

    std::string get_str(void) const;

    std::string get_class(void) const;

    level get_level(void)  const;

    int get_num(void)  const;

    virtual ~Error() throw();

protected:

    int m_numero;
    std::string m_str,m_class;
    level m_level;
};

#endif // ERROR_HPP_INCLUDED
