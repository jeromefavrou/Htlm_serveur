#ifndef INIT_COM_HPP_INCLUDED
#define INIT_COM_HPP_INCLUDED

#include "Error.hpp"
#include "serveur.hpp"
#include "Remote_controle_apn.hpp"
#include "init_config.hpp"

#define SCK 0


bool init_com(CSocketTCPServeur & server_http,RC_Apn & Client_eso_astro,struct conf_com const & param)
{
    try
    {
        server_http.NewSocket(SCK);
        server_http.BindServeur(SCK,INADDR_ANY,param.srv_port);
        server_http.Listen(SCK,1);

        Client_eso_astro.tcp_client=true;
        Client_eso_astro.debug_mode=true;
        Client_eso_astro.connect({param.clt_ip,param.clt_port});

        return true;
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        if(e.get_level()!=Error::level::WARNING)
            return false;
    }

    return false;
}

#endif // INIT_COM_HPP_INCLUDED
