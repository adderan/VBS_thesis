
headerPath = inc
binPath = bin
libPath = lib

sources = $(wildcard $(libPath)/*.cpp)
headers = $(wildcard $(headerPath)/*.h)
executables = ${binPath}/MatchJets
submodules = delphes ExRootAnalysis


CFLAGS=-Wall -Werror `root-config --cflags --glibs` -I ${headerPath} -I delphes -I ExRootAnalysis/ExRootAnalysis -L delphes/ -L ExRootAnalysis -Wl,-rpath,${PWD}/delphes -Wl,-rpath,${PWD}/ExRootAnalysis
CXX=g++

all: ${submodules} ${executables}

${binPath}/MatchJets: event-selection/MatchJets.cpp ${headers} ${sources}
	$(CXX) ${CFLAGS} -o $(binPath)/MatchJets event-selection/MatchJets.cpp ${sources} -lDelphes -lExRootAnalysis

delphes:
	cd delphes && make

ExRootAnalysis:
	cd ExRootAnalysis && make
