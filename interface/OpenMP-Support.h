#pragma once



#include <omp.h>
#include <string>

#include "GlobalInclude.h"

#define TIMING_INIT double __timing_start = 0.0f; std::string __timing_tag = "";

#define TIMING_START(TIMING_TAG) __timing_start = omp_get_wtime(); __timing_tag = TIMING_TAG;

#define TIMING_GET_RESULT_STRING __timing_tag << " : " << std::setprecision( 3 ) << ( (omp_get_wtime() - __timing_start ) * 1000.0f) << " ms"

