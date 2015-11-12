binPath = bin
libPath = lib

sources = $(wildcard $(libPath)/*.cpp)
headers = $(wildcard $(libPath)/*.h)
executables = ${binPath}/MatchJets ${binPath}/TrainJetClassifier ${binPath}/TestJetClassifier
submodules = delphes ExRootAnalysis


CFLAGS=-g -O0 -Wall -Werror `root-config --cflags --glibs` -I ${libPath} -I delphes -I ExRootAnalysis/ExRootAnalysis -L delphes/ -L ExRootAnalysis -Wl,-rpath,${PWD}/delphes -Wl,-rpath,${PWD}/ExRootAnalysis
CXX=g++

all: ${submodules} jetdict ${executables}

${binPath}/MatchJets: event-selection/MatchJets.cpp ${headers} ${sources}
	$(CXX) ${CFLAGS} -o $(binPath)/MatchJets event-selection/MatchJets.cpp ${sources} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/TrainJetClassifier: event-selection/TrainJetClassifier.cpp ${headers} ${sources}
	${CXX} ${CFLAGS} -o ${binPath}/TrainJetClassifier event-selection/TrainJetClassifier.cpp ${sources} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/TestJetClassifier: test/TestJetClassifier.cpp ${headers} ${sources}
	${CXX} ${CFLAGS} -o ${binPath}/TestJetClassifier test/TestJetClassifier.cpp ${sources} -lDelphes -lExRootAnalysis -lTMVA

delphes:
	cd delphes && make

ExRootAnalysis:
	cd ExRootAnalysis && make

jetdict: ${sources}
	cd ${libPath} && rootcint tmp.cpp -c TrainJet.h && mv tmp.cpp jetdict.cpp
