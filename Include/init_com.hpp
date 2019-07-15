#ifndef INIT_COM_HPP_INCLUDED
#define INIT_COM_HPP_INCLUDED

#include "Error.hpp"
#include "serveur.hpp"
#include "Remote_controle_apn.hpp"

#define SCK 0
#define DEFAUT_CLT_IP "127.0.0.1"
#define DEFAUT_CLT_PORT 6789
#define DEFAUT_SRV_PORT 8080


bool init_com(CSocketTCPServeur & server_http,RC_Apn & Client_eso_astro)
{
    try
    {
        server_http.NewSocket(SCK);
        server_http.BindServeur(SCK,INADDR_ANY,DEFAUT_SRV_PORT);
        server_http.Listen(SCK,1);

        Client_eso_astro.tcp_client=true;
        Client_eso_astro.debug_mode=true;
        Client_eso_astro.connect({DEFAUT_CLT_IP,DEFAUT_CLT_PORT});

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
