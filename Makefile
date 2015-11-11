
headerPath = inc
binPath = bin
libPath = lib

sources = $(wildcard $(libPath)/*.cpp)
headers = $(wildcard $(headerPath)/*.h)
executables = ${binPath}/MatchJets ${binPath}/TrainJetClassifier
submodules = delphes ExRootAnalysis


CFLAGS=-Wall -Werror `root-config --cflags --glibs` -I ${headerPath} -I delphes -I ExRootAnalysis/ExRootAnalysis -L delphes/ -L ExRootAnalysis -Wl,-rpath,${PWD}/delphes -Wl,-rpath,${PWD}/ExRootAnalysis
CXX=g++

all: ${submodules} jetdict ${executables}

${binPath}/MatchJets: event-selection/MatchJets.cpp ${headers} ${sources}
	$(CXX) ${CFLAGS} -o $(binPath)/MatchJets event-selection/MatchJets.cpp ${sources} -lDelphes -lExRootAnalysis

${binPath}/TrainJetClassifier: event-selection/TrainJetClassifier.cpp ${headers} ${sources}
	${CXX} ${CFLAGS} -o ${binPath}/TrainJetClassifier event-selection/TrainJetClassifier.cpp ${sources} -lDelphes -lExRootAnalysis -lTMVA

delphes:
	cd delphes && make

ExRootAnalysis:
	cd ExRootAnalysis && make

jetdict: ${sources}
	rm ${libPath}/jetdict.cpp
	cd ${headerPath} && rootcint jetdict.cpp -c TrainJet.h
	mv ${headerPath}/jetdict.cpp ${libPath}
