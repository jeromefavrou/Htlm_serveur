#ifndef INIT_CONFIG_HPP_INCLUDED
#define INIT_CONFIG_HPP_INCLUDED

#define DEFAUT_CLT_IP "127.0.0.1"
#define DEFAUT_CLT_PORT 6789
#define DEFAUT_SRV_PORT 8080

#include <string>
#include <fstream>

using std::string;
using std::ifstream;
using std::ofstream;

struct conf_com
{
    string clt_ip;
    unsigned int clt_port;
    unsigned int srv_port;
};
struct Config
{
    string save_data_path;
    struct conf_com com;
};

struct Config init_config(string const & file)
{
    struct Config param;

    param.save_data_path= "";
    param.com.clt_ip= DEFAUT_CLT_IP;
    param.com.clt_port= DEFAUT_CLT_PORT;
    param.com.srv_port= DEFAUT_SRV_PORT;

    ifstream If(file);
    try
    {
        if(If.bad() || If.fail())
            throw Error(0,"config file cannot be opened",Error::level::WARNING);

        string word;

        while(If.good() && !If.eof())
        {
            If >> word;

            if(If.eof())
                break;

            if(word == "clt_ip")
                If >> param.com.clt_ip;

            else if(word == "clt_port")
                If >> param.com.clt_port;

            else if(word == "srv_port")
                If >> param.com.srv_port;

            else if(word == "data_save_path")
            {
                If >> param.save_data_path;

                ofstream Of_check(param.save_data_path+"/.check");

                try
                {
                    if(Of_check.bad() || Of_check.fail())
                        throw Error(0,"have not right for write into \"" +param.save_data_path + "\"",Error::level::ERROR);

                    Of_check.close();

                    ifstream If_check(param.save_data_path+"/.check");

                    if(If_check.bad() || If_check.fail())
                        throw Error(0,"have not right for read \"" +param.save_data_path + "\"",Error::level::ERROR);

                    free_cmd("rm http_srv",false);
                    free_cmd("rm -r -d " + param.save_data_path + "/http_srv",false);

                    free_cmd("mkdir " + param.save_data_path + "/http_srv",false);
                    free_cmd("ln -s -r " + param.save_data_path + "/http_srv http_srv" ,false);
                    free_cmd("rm "+ param.save_data_path+"/.check",false);

                    param.save_data_path =  "RAW.html";

                }
                catch(Error & e)
                {
                    std::cerr << e.what() <<std::endl;

                    param.save_data_path="";
                }



            }

            else
            {
                Error::add_to_log("config parameter \""+ word  + "\" uncorrect  ->  ignored");
                continue;
            }

        }
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;
        Error::add_to_log("\t default parameter load");
    }

    return param;
}



#endif // INIT_CONFIG_HPP_INCLUDED
