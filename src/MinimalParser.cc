#include <iostream>
#include <fstream>
#include <stdlib.h>


#include "MinimalParser.h"

#include "TObjArray.h"
#include "TObjString.h"

#include "RooMsgService.h"
#include "RooRealVar.h"
#include "RooStringVar.h"
#include "RooArgSet.h"

// To check if the parameter exists with the RooFit bug
const double CHECKDOUBLE = 181983181983.181983181983;
const TString CHECKSTRING("Gallia est omnis divisa in partes tres");

/*----------------------------------------------------------------------------*/

MinimalParser::MinimalParser (const char* cfg_name):
    TNamed("TheParser","TheParser"),
    m_cfg_name(cfg_name),
    m_is_verbose(true){

    if (m_cfg_name.Contains(".py"))
    { 
        std::cout << "Using as full pyconf with file: " << m_cfg_name << std::endl;

        TString new_cfg_name=m_cfg_name;
        new_cfg_name+=".cfg";

        TString command("python ");
        command+= m_cfg_name+" > "+new_cfg_name+"\n";

	m_cfg_name = new_cfg_name;
        std::cout << "Using as ini cfg " << m_cfg_name << std::endl;

        system(command.Data());

        }

    m_sections = getvString("__internal__.sections");

    };

/*----------------------------------------------------------------------------*/
double MinimalParser::m_read_double(TString& val){

    double ret_val=CHECKDOUBLE;

    if (m_is_verbose)
        std::cout << "[MinimalParser::m_read_double] " 
                  << "String to convert to double: " << val.Data();

    if (val == ""){
        if (m_is_verbose)
            std::cout << " --> String is empty. Returning fail value.\n";
        return ret_val;
        }

    else if (val[0] != '@'){
        if (m_is_verbose)
            std::cout << " --> Converting string directly to double.\n";
        ret_val = val.Atof();
        }

    else{
        if (m_is_verbose)
            std::cout << " --> Seeking for variable in config.\n";
        val.Remove(0,1);
        TObjArray* arr = val.Tokenize(".");
        TString section(static_cast<TObjString*>((*arr)[0])->String());
        TString par_name(static_cast<TObjString*>((*arr)[1])->String());
        read(section,par_name,ret_val);
        delete arr;
        }
    if (m_is_verbose)
        std::cout << "[MinimalParser::m_read_double] " 
                  << "Returning : " << ret_val << std::endl;

    return ret_val;

    }

/*----------------------------------------------------------------------------*/

bool MinimalParser::read (const char* section_name, const char* par_name, double& val){

//     val = CHECKDOUBLE;
// 
//     RooRealVar val_rrv(par_name,par_name,CHECKDOUBLE);
//     RooMsgService::instance().setGlobalKillBelow(RooMsgService::WARNING);
//     RooArgSet(val_rrv).readFromFile(m_cfg_name.Data(),
//                                               0,
//                                               section_name,
//                                               0);
//     RooMsgService::instance().setGlobalKillBelow(RooMsgService::INFO);
// 
// 
//     val=val_rrv.getVal();
// 
    //std:: cout << val << " and " << CHECKDOUBLE <<  " and " << fabs(CHECKDOUBLE -val)<<std:: endl;


    TString valstr("");
    bool retval = read(section_name,par_name,valstr,true);
    if (retval == false)
        return retval;

    val = m_read_double(valstr);

    if ( fabs(val - CHECKDOUBLE) < 0.0001){ // i.e. FAIL!
        m_print_error(par_name,section_name,m_cfg_name.Data());
        val=0;
        return false;
        }

    if (m_is_verbose)
        std::cout << "[MinimalParser::read] Value of val = " << val << std::endl;

    return retval;
    };


/*----------------------------------------------------------------------------*/

bool MinimalParser::read (const char* section_name, const char* par_name, int& val){

    double vald;
    bool retval = read(section_name, par_name, vald);
    if (m_is_verbose)
        std::cout << "[MinimalParser::read] Casting "
                  << section_name << "." << par_name << " to integer...\n";
    val = (int) vald;
    return retval;
    };

/*----------------------------------------------------------------------------*/

bool MinimalParser::read (const char* section_name,
                          const char* par_name,
                          TString& val,
                          bool numeric_conversion){

    val = CHECKSTRING;

    RooStringVar val_rsv(par_name,par_name,CHECKSTRING.Data());
    //RooMsgService::instance().setGlobalKillBelow(RooMsgService::WARNING);
    RooArgSet(val_rsv).readFromFile(m_cfg_name.Data(), 0, section_name, 0);
    //RooMsgService::instance().setGlobalKillBelow(/*RooMsgService*/::INFO);

    val=val_rsv.getVal();

    if (val == CHECKSTRING){ // i.e. FAIL!
        m_print_error(par_name,section_name,m_cfg_name.Data());
        val="";
        return false;
        }

    // Check string format
    if (not numeric_conversion and
        (val[0] !='\"' or val[val.Length()-1] !='\"')){

        std::cerr << "Error in formatting!\n" 
                  << " - Config = " << m_cfg_name.Data() << std::endl
                  << " - Section = " << section_name << std::endl
                  << " - Parameter = " << par_name << std::endl;

        val = "";

        return false;
        }

    // Remove the trail and beginning '"'
    if (not numeric_conversion){
        val.Remove(0,1);
        val.Remove(val.Length()-1,1);
        }

    return true;
    };

/*----------------------------------------------------------------------------*/

bool MinimalParser::read (const char* section_name, 
                          const char* par_name, 
                          std::vector<TString>& val,
                          bool numeric_conversion){


    TString vector_as_string= CHECKSTRING;

    RooStringVar val_rsv(par_name,par_name,"");
    //RooMsgService::instance().setGlobalKillBelow(RooMsgService::WARNING);
    RooArgSet(val_rsv).readFromFile(m_cfg_name.Data(), 0, section_name, 0);
    //RooMsgService::instance().setGlobalKillBelow(RooMsgService::INFO);

    vector_as_string= val_rsv.getVal();

    if (vector_as_string == CHECKSTRING){ // i.e. FAIL!
        m_print_error(par_name,section_name,m_cfg_name.Data());
        return false;
        }

    // Check vstring format 1
    if (not numeric_conversion and
        (vector_as_string[0] !='\"' or 
        vector_as_string[vector_as_string.Length()-1] !='\"')){

        std::cerr << "Error in vstring formatting! Wrong delimiters.\n" 
                  << " - Config = " << m_cfg_name.Data() << std::endl
                  << " - Section = " << section_name << std::endl
                  << " - Parameter = " << par_name << std::endl;

        val.clear();

        return false;
        }

    // Remove the trail and beginning '"'
    if (not numeric_conversion){
        vector_as_string.Remove(0,1);
        vector_as_string.Remove(vector_as_string.Length()-1,1);
        }

    // Check number of " and ,
    if (not numeric_conversion and
        (vector_as_string.CountChar('\"') != vector_as_string.CountChar(',')*2)){

        std::cerr << "Error in vstring formatting!Wrong separators.\n" 
                  << " - Config = " << m_cfg_name.Data() << std::endl
                  << " - Section = " << section_name << std::endl
                  << " - Parameter = " << par_name << std::endl;

        val.clear();

        return false;

        }

    //std::cout << "String read = " << vector_as_string.Data() << std::endl;
    // Split string and fill vector
    TObjArray* arr;
    if (numeric_conversion)
        arr = vector_as_string.Tokenize(",");
    else
        arr = vector_as_string.Tokenize("\",\"");

    for (int i=0;i< arr->GetEntries();++i)
        val.push_back ( (static_cast<TObjString*>((*arr)[i]))->String());
    delete arr;

    return true;
    };

/*----------------------------------------------------------------------------*/

bool MinimalParser::read (const char* section_name, 
                          const char* par_name, 
                          std::vector<double>& val){

    std::vector<TString> stringv;
    bool retval = read(section_name,par_name,stringv, true);

    if (retval)
        for (unsigned i=0;i< stringv.size();++i)
            val.push_back( m_read_double(stringv[i]));

    return retval;
    };

/*----------------------------------------------------------------------------*/

bool MinimalParser::read (const char* section_name, 
                          const char* par_name, 
                          std::vector<int>& val){

    std::vector<double> doublev;
    bool retval = read(section_name,par_name,doublev);

    if (retval)
        for (unsigned i=0;i< doublev.size();++i)
            val.push_back( (int) doublev[i]);

    return retval;
    };

/*----------------------------------------------------------------------------*/

void MinimalParser::m_print_error(const char* par_name,
                                  const char* sec_name,
                                  const char* config_name){

        std::cerr << "[MinimalParser] ERROR! " 
                  << sec_name << "." << par_name
                  << " in file " << config_name << " and included not found!"
                  << std::endl;

    };

/*----------------------------------------------------------------------------*/

void MinimalParser::m_split_sec_par(const char* secpar,
                                    TString& sec,
                                    TString& par){

    // loop on the string and when "." found start filling the parname

    TString secpars(secpar);

    sec=par="";
    TString* v[2]={&sec,&par};
    int index=0;
    for (int i=0;i<secpars.Length();++i){

        if (secpars[i]=='.'){
            index=1;
            continue;
            }

        (*v[index])+=secpars[i];

        //std::cout << "Secpars[i] = " << secpars[i] << " " << sec.Data() << std::endl;

        }

//     std::cout << "sec = " << sec.Data() << " "
//               << "par = " << par.Data() << "\n";

    }

/*----------------------------------------------------------------------------*/

bool MinimalParser::has(const char* namesec){

    TString sec,par;

    m_split_sec_par(namesec,sec,par);

    RooStringVar val_rsv(par.Data(),par.Data(),CHECKSTRING.Data());

    //RooMsgService::instance().setGlobalKillBelow(RooMsgService::WARNING);
    RooArgSet(val_rsv).readFromFile(m_cfg_name.Data(),
                                    0,
                                    sec.Data(),
                                    0);
    //RooMsgService::instance().setGlobalKillBelow(RooMsgService::INFO);

    TString val(val_rsv.getVal());

    if (val == CHECKSTRING){ // i.e. FAIL!
        m_print_error(par,sec,m_cfg_name.Data());
        val="";
        return false;
        }

    return true;

    }

/*----------------------------------------------------------------------------*/

int MinimalParser::getInt (const char* namesec){

    TString sec,par;
    m_split_sec_par(namesec,sec,par);

    int i;
    read(sec.Data(),par.Data(),i);
    return i;
    }

/*----------------------------------------------------------------------------*/

std::vector<int> MinimalParser::getvInt (const char* namesec){

    TString sec,par;
    m_split_sec_par(namesec,sec,par);

    std::vector<int> vi;
    read(sec.Data(),par.Data(),vi);
    return vi;
    }

/*----------------------------------------------------------------------------*/

double MinimalParser::getDouble (const char* namesec){

    TString sec,par;
    m_split_sec_par(namesec,sec,par);

    double i;
    read(sec.Data(),par.Data(),i);
    return i;
    }

/*----------------------------------------------------------------------------*/

std::vector<double> MinimalParser::getvDouble (const char* namesec){

    TString sec,par;
    m_split_sec_par(namesec,sec,par);

    std::vector<double> vd;
    read(sec.Data(),par.Data(),vd);
    return vd;
    }


/*----------------------------------------------------------------------------*/

TString MinimalParser::getString (const char* namesec, const char* def_val){

    TString sec,par;
    m_split_sec_par(namesec,sec,par);

    TString s;
    bool read_fail= not read(sec.Data(),par.Data(),s);
    if (read_fail){
        std::cerr << "[MinimalParser] INFO: Filling with default value \"" 
                  << def_val << "\" ...\n";
        s=def_val;
        }
    return s;
    }

/*----------------------------------------------------------------------------*/

std::vector<TString> MinimalParser::getvString (const char* namesec){

    TString sec,par;
    m_split_sec_par(namesec,sec,par);

    std::vector<TString> vs;
    read(sec.Data(),par.Data(),vs);
    return vs;
    }

/*----------------------------------------------------------------------------*/



/// To build the cint dictionaries
ClassImp(MinimalParser)
