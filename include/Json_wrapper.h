#pragma once

#include "jsonParser.h"
//#include "TObject.h"

class Json_wrapper{

    public:

    Json_wrapper();
    Json_wrapper(const char* json_name);
    bool has(int run, int ls);
    
    
    // returns true if the json file has been loaded successfuly, false otherwise
    bool isValid()
    {
     return m_isValid; 
    }
    
    ~Json_wrapper();
    
    private:    
    
    Json* m_json;
      bool m_isValid;

      // for caching the last event
      int m_lastRun;
      int m_lastLumiSec;
      bool m_lastResult;
    
//ClassDef(Json_wrapper,1) 
    
};
