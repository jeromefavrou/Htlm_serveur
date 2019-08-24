#include <iostream>
#include <chrono>
#include <thread>

#include "Include/init_variable.hpp"
#include "Include/init_com.hpp"
#include "Include/eos_processing.hpp"
#include "Include/th_com_atmega328.hpp"

#include "data_utility.hpp"

using namespace std;

int main()
{

    bool Cs{true};

    bool sync_eos_client{false},sync_serial{false};

     Uart_atemga_tram data_tram;

     Error::add_to_log("--server starting--");

    std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(3000));//10s

    CSocketTCPServeur server_http;
    RC_Apn Client_eso_astro;
    Serial serial0("/dev/ttyAMA0", BAUD::BR_9600 );

    std::map<std::string,std::string> device,input_var;
    std::map<std::string,std::vector<std::string>> list_var;

    init_var(device,input_var,list_var);

    Cs=init_com(server_http,Client_eso_astro);

    std::thread th_eos_proc(&eos_process,std::ref(Cs),std::ref(sync_eos_client),std::ref(Client_eso_astro),std::ref(device),std::ref(input_var),std::ref(list_var));
    std::thread th_com_atmega(&th_lp_wr,std::ref(Cs),std::ref(serial0),std::ref(sync_serial),std::ref(data_tram));


    while(Cs)
    {
        bool html_data=false;

        try
        {
            server_http.AcceptClient(SCK,0);

            system("clear");

            Tram debuf;
            server_http.Read<2048>(SCK,debuf.get_data());

            std::string line,mot,dest;
            auto rep=debuf.get_ss_data();

            while(std::getline(rep,line))
            {
                cout << line << endl;

                std::stringstream treat;
                treat << line;

                treat >> mot;
                if(mot == "POST" || mot == "GET")
                {
                    treat >> mot;

                    if(mot=="/favicon.ico")
                        break;
                    if(mot=="/")
                        dest="INDEX.html";
                    else
                    {
                        if(mot[mot.size()-1]=='?')
                            dest=std::string(mot.begin()+1,mot.end()-1);
                        else
                            dest=std::string(mot.begin()+1,mot.end());
                    }
                    if(std::string(dest.end()-4,dest.end())=="html")
                        html_data=true;
                    else
                        html_data=false;
                }
            }

            rep=debuf.get_ss_data();
            extract_var(rep,input_var);

            device["ANALOG0"]=ss_cast<float,std::string>(data_tram.in.thermistor[0]);
            device["ANALOG1"]=ss_cast<float,std::string>(data_tram.in.thermistor[1]);
            device["ANALOG2"]=ss_cast<float,std::string>(data_tram.in.thermistor[2]);
            device["ANALOG3"]=ss_cast<float,std::string>(data_tram.in.thermistor[3]);
            device["ANALOG4"]=ss_cast<float,std::string>(data_tram.in.am2320.temperate);
            device["ANALOG5"]=ss_cast<float,std::string>(data_tram.in.am2320.humidity);

            if(device["APN.ACTION.VALUE"]=="1")
            {
                device["APN.ISO"]=input_var["APN.ISO"];
                device["APN.FRAMES"]=input_var["APN.FRAMES"];
                device["APN.APERTURE"]=input_var["APN.APERTURE"];
                device["APN.EXPOSURE"]=input_var["APN.EXPOSURE"];
                device["APN.DIRECTORIE"]=input_var["APN.DIRECTORIE"];
            }


            if(input_var["APN.ACTION.VALUE"]=="1" )
            {
                device["APN.ACTION.NAME"]="Stop shoot";
                device["APN.ACTION.VALUE"]="0";

            }
            else if(input_var["APN.ACTION.VALUE"]=="0" )
            {
                device["APN.ACTION.NAME"]="Start shoot";
                device["APN.ACTION.VALUE"]="1";
            }

            ifstream If("Header.html");
            string buf;

            Tram acceuille;
            acceuille+="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

            if(html_data)
                acceuille+=generate_html(dest,device,list_var);
            else
                acceuille+=binary_download(dest);

            acceuille+="\r\n";

            server_http.Write(0,acceuille.get_c_data());

            server_http.CloseClientSocket(0);
        }
        catch(Error & e)
        {
            std::cerr << e.what() << std::endl;

            if(e.get_level()==Error::level::FATAL_ERROR)
                Cs=false;
        }

        std::this_thread::sleep_for(std::chrono::duration<int,std::milli>(10));
    }

    th_eos_proc.join();
    th_com_atmega.join();


    server_http.CloseSocket(SCK);

    Error::add_to_log("--server ending--");

    return 0;
}
