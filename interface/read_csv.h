#ifndef __ReadCsv__
#define __ReadCsv__
    
#include <string>
#include <vector>
#include <fstream>

using namespace std;


class ReadCsv
{
  public:
    
    ReadCsv( string sFileName );
  
  void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ");



  vector<string> ReadColumn(int filename);
  
  string m_sFileName;
};
  
  #endif

