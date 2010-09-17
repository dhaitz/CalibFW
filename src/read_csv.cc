#include "read_csv.h"

ReadCsv::ReadCsv( string sFileName )
{
    this->m_sFileName = sFileName;
}

void ReadCsv::Tokenize(const string& str,
                       vector<string>& tokens,
                       const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

vector<string> ReadCsv::ReadColumn(int colnum)
{
    std::vector<string> col;
    std::vector<string> parts;

    std::ifstream in(this->m_sFileName.c_str() , ifstream::in);
    string s;
    if (!in || in.bad()) return vector<string>();// lines;

    while (!in.eof()) {
        getline(in, s);

        parts.clear();
        Tokenize(s, parts);

        if ( parts.size() > 0 )
        {
            col.push_back( parts[ colnum ] );
        }
    }
    in.close();

    return col;
}

