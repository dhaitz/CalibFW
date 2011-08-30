AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}


ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs --glibs)
ROOTGLIBS     = $(shell root-config --glibs)
ROOTLDFLAGS   = $(shell root-config --ldflags)

LIB_PATH  = -L../KappaTools/lib/ -L../Kappa/lib/

KAPPA_PATH 	  = $(top_srcdir)/../Kappa
KAPPATOOLS_PATH 	  = $(top_srcdir)/../KappaTools
KAPPA_ROOT_PATH = $(top_srcdir)/..

#CXXFLAGS = -O3

AM_CXXFLAGS = 	-fopenmp -pedantic -Werror -std=c++0x \
				-I$(top_srcdir)/include $(ROOTCFLAGS) -I$(KAPPA_PATH) \
				-I$(KAPPATOOLS_PATH) -I$(KAPPA_ROOT_PATH)# $(AM_CXXFLAGS) -O3
AM_LDFLAGS 	= $(ROOTLIBS) -l RooFit -l RooFitCore -l Minuit -l EG -l GenVector -l Foam -l Kappa -l KRootTools -l KToolbox \
			  $(LIB_PATH)
			
#CXX_CFLAGS = -Iinclude/ 

bin_PROGRAMS = resp_cuts
resp_cuts_SOURCES = src/resp_cuts.cpp src/DrawBase.cc src/EventData.cc src/EventPipelineRunner.cc \
					src/GlobalInclude.cc src/ZJetPipeline.cc



#dist_noinst_SCRIPTS = autogen.sh