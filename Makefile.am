AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}


ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs --glibs)
ROOTGLIBS     = $(shell root-config --glibs)
ROOTLDFLAGS   = $(shell root-config --ldflags)

LIB_PATH  = -L../KappaTools/lib/ -L../Kappa/lib/

KAPPA_PATH 	  = $(top_srcdir)/../Kappa
KAPPATOOLS_PATH = $(top_srcdir)/../KappaTools
KAPPA_ROOT_PATH = $(top_srcdir)/..

CMSSW_PATH 	  = external/OfflineCorrection


AM_CXXFLAGS = 	-fopenmp -pedantic -Werror -Wall -Wfatal-errors -std=c++0x \
				 -O3 -march=native -g -DSTANDALONE \
		          -Wclobbered -Wempty-body  -Wignored-qualifiers -Wmissing-field-initializers \
		          		          -Wsign-compare -Wtype-limits  -Wuninitialized \
				-I$(top_srcdir)/src $(ROOTCFLAGS) -I$(KAPPA_PATH) -I$(CMSSW_PATH) \
				-I$(KAPPATOOLS_PATH) -I$(KAPPA_ROOT_PATH)
				
AM_LDFLAGS 	= $(ROOTLIBS) -l RooFit -l RooFitCore -l Minuit -l EG -l GenVector -l Foam -l Kappa -l KRootTools -l KToolbox \
			  $(LIB_PATH)

					
OFFLINE_CORR = $(CMSSW_PATH)/CondFormats/JetMETObjects/src
OFFLINE_CORR_SRC = $(OFFLINE_CORR)/FactorizedJetCorrector.cc $(OFFLINE_CORR)/JetCorrectionUncertainty.cc \
				   $(OFFLINE_CORR)/JetCorrectionUncertainty.cc 

# seems we cant use m4 vars here ??  
CALIB_SRC = src/DrawBase.cc src/EventData.cc \
			src/EventPipelineRunner.cc \
			src/GlobalInclude.cc \
			src/ZJet/ZJetPipeline.cpp  \
			src/ZJet/MetaDataProducer/CorrJetProducer.cpp  \
			src/Pipeline/ProfileConsumerBase.cpp \
			src/ZJet/ZJetMetaData.cpp \
			external/OfflineCorrection/CondFormats/JetMETObjects/src/JetCorrectionUncertainty.cc \ 
			external/OfflineCorrection/CondFormats/JetMETObjects/src/FactorizedJetCorrector.cc \ 
			external/OfflineCorrection/CondFormats/JetMETObjects/src/SimpleJetCorrector.cc \
			external/OfflineCorrection/CondFormats/JetMETObjects/src/JetCorrectorParameters.cc \
			external/OfflineCorrection/CondFormats/JetMETObjects/src/SimpleJetCorrectionUncertainty.cc
					

bin_PROGRAMS = resp_cuts resp_cuts_test
resp_cuts_SOURCES = src/resp_cuts.cpp $(CALIB_SRC)

resp_cuts_test_SOURCES = src/resp_cuts_test.cpp $(CALIB_SRC)



#dist_noinst_SCRIPTS = autogen.sh