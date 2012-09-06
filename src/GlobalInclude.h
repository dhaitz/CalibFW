#pragma once

#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <exception>

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

extern std::ofstream * g_logFile;

#define FLOAT_COMPARE( first, second ) (  TMath::Abs( (first - second) / second) < 0.02 )
#define CALIB_LOG( out )  {std::cout << out << std::endl;}
#define CALIB_LOG_FILE( out ) {std::cout << out << std::endl; (*g_logFile) << out << std::endl;}
#define CALIB_LOG_FATAL( out ) { std::cout << "FATAL: " << out << std::endl; \
				(*g_logFile) << "FATAL:" << out << std::endl; \
				g_logFile->close(); std::cout.flush(); assert(false);throw 23; }\

typedef std::vector<std::string> stringvector;
typedef std::vector<double> doublevector;

// Implements a property with Get and Set Methods

#define IMPL_PROPERTY(TYPE, SNAME) 					\
private: 											\
TYPE m_##SNAME; 									\
public: 											\
TYPE Get##SNAME ( ) const { return m_##SNAME; }			\
TYPE const& GetRef##SNAME ( ) const { return m_##SNAME; }			\
TYPE const* GetPt##SNAME ( ) const { return &m_##SNAME; }			\
void Set##SNAME ( TYPE val) { m_##SNAME = val; }


#define IMPL_PROPERTY_READONLY(TYPE, SNAME) 					\
private: 											\
TYPE m_##SNAME; 									\
public: 											\
TYPE Get##SNAME ( ) const { return m_##SNAME; }			\
private:											\
void Set##SNAME ( TYPE val) { m_##SNAME = val; }


/* Obtain a backtrace and print it to stdout. */
void print_trace(void);
void backtrace_assert(bool val);
