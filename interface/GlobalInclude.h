

#ifndef __GLOBALINCLUDE_H__
#define __GLOBALINCLUDE_H__

#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <exception>

std::ofstream * g_logFile;

#define FLOAT_COMPARE( first, second ) (  TMath::Abs( (first - second) / second) < 0.02 )
#define CALIB_LOG( out )  {std::cout << out << std::endl;}
#define CALIB_LOG_FILE( out )  {std::cout << out << std::endl; (*g_logFile) << out << std::endl;}
#define CALIB_LOG_FATAL( out ) {	std::cout << "FATAL: " << out << std::endl; std::cout << "FATAL: " << out << std::endl; \
								(*g_logFile) << "FATAL:" << out << std::endl; g_logFile->close(); std::cout.flush(); throw 23; }\

typedef std::vector<std::string> stringvector;

// Implements a property with Get and Set Methods

#define IMPL_PROPERTY(TYPE, SNAME) 					\
private: 											\
TYPE m_##SNAME; 									\
public: 											\
TYPE Get##SNAME ( ) { return m_##SNAME; }			\
void Set##SNAME ( TYPE val) { m_##SNAME = val; }	\



#endif
