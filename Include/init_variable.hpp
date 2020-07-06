#ifndef INIT_VARIABLE_HPP_INCLUDED
#define INIT_VARIABLE_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>

void init_display_var(std::map<std::string,std::string> & DV)
{
    DV["ANALOG0"]="12";
    DV["ANALOG1"]="15";
    DV["ANALOG2"]="12";
    DV["ANALOG3"]="11";
    DV["ANALOG4"]="10";
    DV["ANALOG5"]="83";

    DV["ANALOG0.NAME"]="sonde tmp batterie";
    DV["ANALOG1.NAME"]="sonde tmp avant lunette";
    DV["ANALOG2.NAME"]="sone tmp milieux lunette";
    DV["ANALOG3.NAME"]="sonde tmps APN";
    DV["ANALOG4.NAME"]="sonde tmp interne";
    DV["ANALOG5.NAME"]="sonde humidité interne";

    DV["INFO1.NAME"]="point de rosé";
    DV["INFO1.VALUE"]="-1000";
    DV["INFO2.NAME"]="% probabilistique de couverture nuageuse";
    DV["INFO2.VALUE"]="-1000";

    DV["OUTPUT0"]="green";
    DV["OUTPUT1"]="black";
    DV["OUTPUT2"]="green";
    DV["OUTPUT3"]="black";
    DV["OUTPUT4"]="black";
    DV["OUTPUT5"]="green";

    DV["OUTPUT0.NAME"]="resistence batterie";
    DV["OUTPUT1.NAME"]="resistence avant lunette";
    DV["OUTPUT2.NAME"]="resistence milieux lunette";
    DV["OUTPUT3.NAME"]="resistence APN";
    DV["OUTPUT4.NAME"]="ventilateur";
    DV["OUTPUT5.NAME"]="peltier";

    DV["APN.STAT"]="indeterminé";
    DV["APN.ISO"]="indeterminé";
    DV["APN.ISO.VALUE"]="indeterminé";
    DV["APN.FRAMES"]="indeterminé";
    DV["APN.APERTURE"]="indeterminé";
    DV["APN.APERTURE"]="indeterminé";
    DV["APN.APERTURE.VALUE"]="indeterminé";
    DV["APN.EXPOSURE"]="indeterminé";
    DV["APN.DIRECTORIE"]="indeterminé";
    DV["APN.ACTION.VALUE"]="1";
    DV["APN.ACTION.NAME"]="Start shoot";
    DV["APN.COUNTER"]="NC";
}

void init_input_var(std::map<std::string,std::string> & IV)
{
    IV["APN.ISO"]="/";
    IV["APN.FRAMES"]="/";
    IV["APN.APERTURE"]="/";
    IV["APN.EXPOSURE"]="/";
    IV["APN.ACTION.VALUE"]="0";

    IV["STEPPER.STAT"]="0";
    IV["STEPPER.DIR"]="1";
    IV["STEPPER.SPEED"]="1";
    IV["STEPPER.ACTION.VALUE"]="0";

    IV["SHUTDOWN"]="0";
}
void init_list(std::map<std::string,std::vector<std::string>> & LV)
{
    LV["ISO"]=std::vector<std::string>(0);
    LV["APERTURE"]=std::vector<std::string>(0);
    LV["RAW"]=std::vector<std::string>(0);
}
void init_var(std::map<std::string,std::string> & DV,std::map<std::string,std::string> & IV,std::map<std::string,std::vector<std::string>> & LV)
{
    init_display_var(DV);
    init_input_var(IV);
    init_list(LV);
}



#endif // INIT_VARIABLE_HPP_INCLUDED
