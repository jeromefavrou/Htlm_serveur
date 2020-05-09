#ifndef EOS_PROCESSING_HPP_INCLUDED
#define EOS_PROCESSING_HPP_INCLUDED


#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include "utility.hpp"

#include "Remote_controle_apn.hpp"

bool check_for_start(RC_Apn & clt_eos_astro,std::map<std::string,std::string> & DV)
{
    //separé en deux
    try
    {
        if(ss_cast<std::string,unsigned int>(DV["APN.ISO"])<=0u || std::find(clt_eos_astro.get_parameter(gp2::Conf_param::ISO).begin(),clt_eos_astro.get_parameter(gp2::Conf_param::ISO).end(),DV["APN.ISO"])==clt_eos_astro.get_parameter(gp2::Conf_param::ISO).end())
            throw Error(4,"parametre d' iso incorrecte",Error::level::ERROR);

        if(ss_cast<std::string,float>(DV["APN.APERTURE"])<1.0f || std::find(clt_eos_astro.get_parameter(gp2::Conf_param::APERTURE).begin(),clt_eos_astro.get_parameter(gp2::Conf_param::APERTURE).end(),DV["APN.APERTURE"])==clt_eos_astro.get_parameter(gp2::Conf_param::APERTURE).end())
            throw Error(5,"parametre d' ouverture incorrecte",Error::level::ERROR);

        if(ss_cast<std::string,unsigned int>(DV["APN.FRAMES"])<=0)
            throw Error(6,"parametre nombre de frame incorrecte",Error::level::ERROR);

        if(ss_cast<std::string,unsigned int>(DV["APN.EXPOSURE"])<1)
            throw Error(7,"parametre d' exposition incorrecte",Error::level::ERROR);


        //DV["APN.DIRECTORIE"];

        DV["APN.STAT"]="APN prêt";
        return true;
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        DV["APN.STAT"]=e.get_str();

        std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(3000));

        return false;
    }

    return false;
}

void eos_process(bool & Cs,bool & clt_sync,RC_Apn & clt_eos_astro,std::map<std::string,std::string> & DV,std::map<std::string,std::string> & IV, std::map<std::string,std::vector<std::string>> &LV)
{
    if(!Cs)
        return ;

    bool chk_apn{false},chk_parameter{false},disconnect(true);

    unsigned int Frames_token(0);



    while(Cs)
    {
        auto frame_save=ss_cast<std::string,unsigned int>(DV["APN.FRAMES"]);

        try
        {
            while(clt_sync){;}

            clt_sync=true;
            clt_eos_astro.check_apn();
            clt_sync=false;

            chk_apn=true;
        }
        catch(Error & e)
        {
            std::cerr << e.what() << std::endl;
            DV["APN.STAT"]=e.get_str();

            chk_apn=false;
            clt_sync=false;
            disconnect=true;
            chk_parameter=false;

            std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(5000));
        }

        if(disconnect && chk_apn)
        {
            try
            {
                while(clt_sync){;}

                std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(3000));

                clt_sync=true;
                clt_eos_astro.init_conf_param();
                clt_sync=false;

                disconnect=false;
            }
            catch(Error & e)
            {
                std::cerr << e.what() << std::endl;
                DV["APN.STAT"]=e.get_str();
                clt_sync=false;
                disconnect=true;

                std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(1000));
            }
        }

        if(!chk_parameter && !disconnect && chk_apn)
        {
            while(clt_sync){;}

            clt_sync=true;
            chk_parameter=check_for_start(clt_eos_astro,DV);
            clt_sync=false;

            if(chk_apn)
            {
                LV["ISO"]=clt_eos_astro.get_parameter(gp2::Conf_param::ISO);
                LV["APERTURE"]=clt_eos_astro.get_parameter(gp2::Conf_param::APERTURE);
            }
        }
        else if(chk_parameter && !disconnect)
        {
            DV["APN.COUNTER"]=ss_cast<unsigned int,std::string>(Frames_token);

            if(frame_save>=Frames_token && DV["APN.ACTION.VALUE"]=="0")
            {
                while(clt_sync){;}

                clt_sync=true;

                gp2::Folder_data ls_f,ls_f2;

                clt_eos_astro.Ls_file(ls_f);
                clt_eos_astro.capture_EOS_DSLR(Frames_token==0,"1",DV["APN.ISO"],DV["APN.EXPOSURE"],DV["APN.APERTURE"],"1","9","0","0","4");
                std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(1500));
                clt_eos_astro.Ls_file(ls_f2);


                auto res=gp2::delta_folder(ls_f,ls_f2);

                clt_eos_astro.download(res,"/mnt/usb_disk/http_srv");

                clt_sync=false;

                Frames_token++;

            }
            else if(frame_save<Frames_token)
            {
               // DV["APN.FRAMES"]="0";
                DV["APN.ACTION.NAME"]="Start shoot";
                DV["APN.ACTION.VALUE"]="1";
                IV["APN.ACTION.VALUE"]="0";

                Frames_token=0;
            }
        }

        std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(1000));
    }
}

#endif // EOS_PROCESSING_HPP_INCLUDED
