#ifndef ERROR_HPP_INCLUDED
#define ERROR_HPP_INCLUDED

#include <exception>
#include <string>
#include "utility.hpp"
#include <chrono>
#include <ctime>
#include <fstream>

#define log_path ".log"

class Error
{
public:
    enum class level{WARNING,ERROR,FATAL_ERROR};

    Error(int numero, std::string const& phrase,level _level) throw();

    std::string what() throw();

    std::string get_str(void) const;

    std::string get_class(void) const;

    level get_level(void)  const;

    void set_log_stat(bool const &);

    int get_num(void)  const;

    static void add_to_log(std::string const& msg)
    {
        std::fstream Of(log_path,std::ios::out | std::ios::app);

        std::time_t end_time=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        if(Of && !Of.fail() && !Of.bad())
            Of << std::ctime(&end_time) << ": " << msg << "\n";
    }

    virtual ~Error() throw();

protected:

    std::string level_to_str(void);

    int m_numero;
    std::string m_str,m_class;
    level m_level;
    bool m_log_stat;
};

#endif // ERROR_HPP_INCLUDED
