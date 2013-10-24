# Makefile for excalibur:artus (c) 2013
# Joram Berger <joram.berger@cern.ch>
# Dominik Haitz <dhaitz@cern.ch>

ROOTCFLAGS     = $(shell root-config --cflags)
ROOTLDFLAGS    = $(shell root-config --ldflags --libs)
BOOSTPATH      = /wlcg/sw/boost/current/
KAPPAPATH      = ../Kappa/
KAPPATOOLSPATH = ../KappaTools/

PROJECT        = Excalibur
EXECUTABLE     = closure
CXX            = g++
FLAGS          = -O2 -pedantic -Wfatal-errors -l profiler -l tcmalloc #-Wall
CFLAGS         = -c -std=c++0x -g -fPIC -DSTANDALONE $(FLAGS)\
 -Isrc/ -Iexternal/OfflineCorrection/\
 $(ROOTCFLAGS) -I$(BOOSTPATH)/include/ -I$(KAPPATOOLSPATH)/../
LDFLAGS        = $(ROOTLDFLAGS) -lGenVector\
 -L$(BOOSTPATH)/lib/ -lboost_regex\
 -L$(KAPPAPATH)/lib/ -L$(KAPPATOOLSPATH)/lib/ -lKappa -lKRootTools -lKToolbox

OBJECTS = $(patsubst %.cc,%.o,$(wildcard\
	src/*.cc src/*/*.cc external/OfflineCorrection/CondFormats/JetMETObjects/src/*Corr*.cc))

$(EXECUTABLE): $(OBJECTS)
	@echo "Linking" $(EXECUTABLE)":"
	@echo $(CXX) LDFLAGS $(OBJECTS)
	@$(CXX) $(LDFLAGS) $(OBJECTS) -o $@
	@echo "Done"

.cc.o:
	@echo $(CXX) CFLAGS $< -o $@
	@$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	rm -f external/OfflineCorrection/CondFormats/lib/libJetMETObjects.so

purge: clean
	rm -f macros/*.pyc macros/*/*.pyc cfg/closure/*.pyc
	rm -f cfg/closure/*.py.json

check:
	@echo -e "checking COMPILER...   \c" && which $(CXX)
	@echo -e "checking ROOT...       \c" && root-config --version
	@echo -e "checking BOOST...      \c" && ls $(BOOSTPATH)
	@echo -e "checking KAPPA...      \c" && ls $(KAPPAPATH) -d
	@echo -e "checking KAPPATOOLS... \c" && ls $(KAPPATOOLSPATH) -d
	@echo -e "checking PYTHON...     \c" && python --version || echo "  Python is not needed for compiling"
	@echo -e "checking EXECUTABLE... \c" && (ls $(EXECUTABLE) &> /dev/null && ls $(EXECUTABLE)) || echo $(EXECUTABLE) "not yet built"
	@echo $(PROJECT) "is ok."

#TODO: version	path
#NN=`ls /wlcg/sw/boost/current/lib/libboost_regex.so.*` && echo ${NN/*so./}
#&& git --git-dir=$(KAPPAPATH)/../.git log -1 | grep Date

doc:
	@echo "Make" $(EXECUTABLE) "documentation in future"
