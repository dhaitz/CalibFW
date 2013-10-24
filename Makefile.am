AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}


ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs --glibs)
ROOTGLIBS     = $(shell root-config --glibs)
ROOTLDFLAGS   = $(shell root-config --ldflags)

# works on ekpcms5
BOOSTPATH     = /wlcg/sw/boost/current/lib/

LIB_PATH  = -L../KappaTools/lib/ -L../KappaTools/external/lib/ -L../Kappa/lib/ -L$(BOOSTPATH)

KAPPA_PATH 	  = $(top_srcdir)/../Kappa
KAPPATOOLS_PATH = $(top_srcdir)/../KappaTools
KAPPATOOLS_BOOST_PATH = $(top_srcdir)/../KappaTools/external
KAPPA_ROOT_PATH = $(top_srcdir)/..

CMSSW_PATH 	  = external/OfflineCorrection


AM_CXXFLAGS = -pedantic -Wfatal-errors -std=c++0x \
              -O2 -g -fPIC -DSTANDALONE \
		-I$(top_srcdir)/external -I$(top_srcdir)/src $(ROOTCFLAGS) -I$(KAPPA_PATH) -I$(CMSSW_PATH) \
		-I$(KAPPATOOLS_PATH) -I$(KAPPATOOLS_BOOST_PATH) -I$(KAPPA_ROOT_PATH)

PERFTOOLS_FLAGS = -l profiler -l tcmalloc
AM_LDFLAGS 	= $(ROOTLDFLAGS) $(ROOTLIBS) -lboost_regex -lGenVector -lKappa -lKRootTools -lKToolbox \
			  $(LIB_PATH)


OFFLINE_CORR = $(CMSSW_PATH)/CondFormats/JetMETObjects/src
OFFLINE_CORR_SRC = $(OFFLINE_CORR)/FactorizedJetCorrector.cc $(OFFLINE_CORR)/JetCorrectionUncertainty.cc \
				   $(OFFLINE_CORR)/JetCorrectionUncertainty.cc

# seems we cant use m4 vars here ??
CALIB_SRC = src/ZJetEventPipeline/EventData.cc \
			src/EventPipeline/EventPipelineRunner.cc \
			src/GlobalInclude.cc \
			src/ZJetEventPipeline/Pipeline.cc \
            src/ZJetEventPipeline/PipelineInitializer.cc \
			src/ZJetProducer/CorrJetProducer.cc \
			src/ZJetProducer/JetMatcher.cc \
			src/ZJetEventPipeline/MetaData.cc \
			external/OfflineCorrection/CondFormats/JetMETObjects/src/JetCorrectionUncertainty.cc \
			external/OfflineCorrection/CondFormats/JetMETObjects/src/FactorizedJetCorrector.cc \
			external/OfflineCorrection/CondFormats/JetMETObjects/src/SimpleJetCorrector.cc \
			external/OfflineCorrection/CondFormats/JetMETObjects/src/JetCorrectorParameters.cc \
			external/OfflineCorrection/CondFormats/JetMETObjects/src/SimpleJetCorrectionUncertainty.cc


bin_PROGRAMS = closure
closure_SOURCES = src/closure.cc $(CALIB_SRC)

#dist_noinst_SCRIPTS = autogen.sh
