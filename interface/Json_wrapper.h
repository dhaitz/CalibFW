#ifndef __JSON__
#define __JSON__
    
#include "jsonParser.h"
//#include "TObject.h"

class Json_wrapper{

    public:

    Json_wrapper();
    Json_wrapper(const char* json_name);
    bool has(int run, int ls);
    ~Json_wrapper();
    
    private:    
    
    Json* m_json;
    
    
//ClassDef(Json_wrapper,1) 
    
};

#endif
