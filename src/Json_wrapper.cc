#include "Json_wrapper.h"
#include <iostream>
#include <fstream>
#include <sstream>


Json_wrapper::Json_wrapper():m_json(0){}

//------------------------------------------------------------------------------

Json_wrapper::Json_wrapper(const char* json_name){

    m_json = new Json();
    
    std::ifstream jsonfile(json_name, std::ios::binary );
    if (jsonfile) {
        jsonfile.seekg(0, std::ios::end);
        std::streampos length = jsonfile.tellg();
        jsonfile.seekg(0,std::ios::beg);
        
        std::vector<char>  buffer(length);
        jsonfile.read(&buffer[0],length);
        
        std::istringstream jsonStream;
        jsonStream.rdbuf()->pubsetbuf(&buffer[0],length);
       
        if (Json::parse(jsonStream, *m_json)){
            std::cout<<"Parsed JSON..."<< std::endl;
        }else {std::cout<<"Failed to parse JSON! Exiting... \n"<< std::endl;}
    }
    else { std::cout<< "Could not read JSON file! Exiting... \n";}

    }

//------------------------------------------------------------------------------    
    
Json_wrapper::~Json_wrapper(){
    if (m_json!=NULL)
        delete m_json;
    }        
//------------------------------------------------------------------------------

bool Json_wrapper::has(int run, int ls){

    // If a run selection is there, apply it.
    if (m_json){
        std::stringstream runnumber;
        runnumber << run;
        if (!m_json->has<Array>(runnumber.str()))
	  {
/*	    if ( allowNewerRuns )
	    {
	      
//            std::cout << " the event was in run " << runnumber.str() << " and is NOT in the JSON! \n"; 
	    }
	    else*/
	      return false;
        }else {
            int LumiArray_size = m_json->get<Array>(runnumber.str()).size();
            for (int lumii = 0; lumii < LumiArray_size; ++lumii){
                //int SubLumiArray_size = json->get<Array>(runnumber).get<Array>(lumii).getSize();
                //for (sublumii = 0;sublumii < SubLumiArray_size;++sublumii){
                double lowerlumi = m_json->get<Array>(runnumber.str()).get<Array>(lumii).get<double>(0);
                double upperlumi = m_json->get<Array>(runnumber.str()).get<Array>(lumii).get<double>(1);
                if (ls >= lowerlumi && ls <= upperlumi){
//                    std::cout << " Accepted Event in Run " << runnumber.str() 
//                              << " and Lumi section "<< lowerlumi <<"<=" << lum <<"<="<< upperlumi <<"\n";
                    return true;
                }            
            } return false;
        }
    } 
   return true;



}

//------------------------------------------------------------------------------
