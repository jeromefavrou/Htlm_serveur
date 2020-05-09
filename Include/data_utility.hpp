#ifndef DATA_UTILITY_HPP_INCLUDED
#define DATA_UTILITY_HPP_INCLUDED

#include <fstream>
#include <vector>
#include <map>

#define HEADER_HTML_FILE "Var/HEADER.html"
#define STYLE_CSS_FILE "Var/STYLE.css"
#define FOOTER_HTML_FILE "Var/FOOTER.html"
#define INDEX_HTML_FILE "Var/INDEX.html"
#define NAV_HTML_FILE "Var/NAV.html"


VCHAR binary_download(std::string const & file)
{

    std::fstream If(file,std::ios::in | std::ios::binary);

    if(!If || If.fail() || If.bad())
        throw Error(1,"telechargement de \""+file+"\" impossible",Error::level::ERROR);

    If.seekg (0, If.end);
    long int length = If.tellg();
    If.seekg (0, If.beg);

    VCHAR buffer=VCHAR(length,0);

    If.read((char*)buffer.data(),length);

    return buffer;
}

void replace_var(std::string & line, std::map<std::string,std::string> & device)
{
    int s(0);
    bool start(false);

    for(auto i=0u;i<line.size();i++)
    {
        if(line[i]=='&' && !start)
        {
            start=true;
            s=i;
        }
        else if(line[i]=='&' && start)
        {
            line.replace(line.begin()+s, line.begin()+i+1, device[std::string(line.begin()+s+1, line.begin()+i)] );

            s=0;
            start=false;
            i=s;
        }
    }
}

std::string generate_list(std::string const & name, std::map<std::string,std::vector<std::string>> & LV)
{
    std::string buffer{""},line{""},line2{""};


    std::fstream If("Var/"+name+".list",std::ios::in);

    try
    {
        if(!If || If.fail() ||If.bad())
            throw Error(6,"telechargement de \"Var"+name+".list\" impossible",Error::level::ERROR);

        while(std::getline(If,line))
        {
            for(auto z=0u;z<LV[name].size();z++)
            {
                line2=line;

                int s(0);
                bool start(false);

                for(auto i=0u;i<line2.size();i++)
                {
                    if(line2[i]=='&' && !start)
                    {
                        start=true;
                        s=i;
                    }
                    else if(line2[i]=='&' && start)
                    {
                        line2.replace(line2.begin()+s, line2.begin()+i+1, LV[name][z] );

                        s=0;
                        start=false;
                        i=s;
                    }
                }

                buffer+=line2;
            }
        }

        return buffer;
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        return "";
    }

    return "";
}

void replace_list(std::string & line, std::map<std::string,std::vector<std::string>> & LV)
{
    int s(0);
    bool start(false);

    for(auto i=0u;i<line.size();i++)
    {
        if(line[i]=='$' && !start)
        {
            start=true;
            s=i;
        }
        else if(line[i]=='$' && start)
        {
            line.replace(line.begin()+s, line.begin()+i+1, generate_list(std::string(line.begin()+s+1, line.begin()+i),LV));

            s=0;
            start=false;
            i=s;
        }
    }
}

void extract_var(std::stringstream & rep,std::map<std::string,std::string> & rep_var)
{
    std::string line;

    while(getline(rep,line))
    {
       for(auto &i:line)
       {
            if(i=='&' || i=='=')
                i=' ';
       }


        std::stringstream buf(line);

        std::string word;

        while(buf >> word)
        {
            auto it_search=rep_var.find(word);

            if(it_search!=rep_var.end())
            {
                buf >> word;
                it_search->second=word;
            }
        }
    }
}


std::string generate_html(std::string const & body , std::map<std::string,std::string> device,std::map<std::string,std::vector<std::string>> & LV)
{

    std::string html_file("<!DOCTYPE html>\n<html>\n<head>\n"),line("");

    std::fstream Head_if(HEADER_HTML_FILE,std::ios::in);
    std::fstream Style_if(STYLE_CSS_FILE,std::ios::in);
    std::fstream Footer_if(FOOTER_HTML_FILE,std::ios::in);
    std::fstream Nav_if(NAV_HTML_FILE,std::ios::in);
    std::fstream Body_if("Var/"+body,std::ios::in);

    try
    {
        if(!Head_if || Head_if.fail() ||Head_if.bad())
            throw Error(404,"telechargement de \""+std::string(HEADER_HTML_FILE)+"\" impossible",Error::level::WARNING);

        while(std::getline(Head_if,line))
            html_file+=line;
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        if(e.get_level()==Error::level::FATAL_ERROR)
            return html_file;

        html_file+="<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n<title>EOS_ASTRO</title>\n";
    }

    try
    {
        if(!Style_if || Style_if.fail() ||Style_if.bad())
            throw Error(404,"telechargement de \""+std::string(STYLE_CSS_FILE)+"\" impossible",Error::level::WARNING);

        html_file+="<style>\n";

        while(std::getline(Style_if,line))
            html_file+=line;

        html_file+="</style>\n";
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        if(e.get_level()==Error::level::FATAL_ERROR)
            return html_file;
    }

    html_file+="</head>\n";
    html_file+="<body>\n";
    try
    {
        if(!Nav_if || Nav_if.fail() ||Nav_if.bad())
            throw Error(404,"telechargement de \""+std::string(NAV_HTML_FILE)+"\" impossible",Error::level::WARNING);

        while(std::getline(Nav_if,line))
            html_file+=line;
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        if(e.get_level()==Error::level::FATAL_ERROR)
            return html_file;
    }

    try
    {
        if(!Body_if || Body_if.fail() ||Body_if.bad())
            throw Error(404,"telechargement de \""+body+"\" impossible",Error::level::ERROR);

        while(std::getline(Body_if,line))
        {
            replace_var(line,device);
            replace_list(line,LV);

            html_file+=line;
        }

    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        if(e.get_level()==Error::level::FATAL_ERROR)
            return html_file;

        try
        {
            Body_if.close();
            Body_if.open(std::string(INDEX_HTML_FILE),std::ios::in);

            if(!Body_if || Body_if.fail() ||Body_if.bad())
                throw Error(404,"telechargement de \""+std::string(INDEX_HTML_FILE)+"\" impossible",Error::level::ERROR);

            while(std::getline(Body_if,line))
            {
                html_file+=line;
            }


            html_file+="<br/><H1><p class=\"error_str\">"+e.get_str()+"</p></H1><br/>";
        }
        catch(Error & _e)
        {
            std::cerr << _e.what() << std::endl;

            if(_e.get_level()==Error::level::FATAL_ERROR)
                return html_file;
        }
    }

    try
    {
        if(!Footer_if || Footer_if.fail() ||Footer_if.bad())
            throw Error(404,"telechargement de \""+std::string(FOOTER_HTML_FILE)+"\" impossible",Error::level::WARNING);

        html_file+="<footer>\n";

        while(std::getline(Footer_if,line))
            html_file+=line;

        html_file+="</footer>\n";
    }
    catch(Error & e)
    {
        std::cerr << e.what() << std::endl;

        if(e.get_level()==Error::level::FATAL_ERROR)
            return html_file;

         html_file+="<footer>\n</footer>\n";
    }

    html_file+="</body>\n</html>\n";
    return html_file;
}

#endif // DATA_UTILITY_HPP_INCLUDED
