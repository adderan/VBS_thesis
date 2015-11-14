binPath = bin
libPath = lib

libsources = $(wildcard $(libPath)/*.cpp)
headers = $(wildcard $(libPath)/*.h)
executables = ${binPath}/MatchJets ${binPath}/TrainJetClassifier ${binPath}/TestJetClassifier ${binPath}/MakeEventTrainingSets
submodules = delphes ExRootAnalysis


CFLAGS=-g -O0 -Wall -Werror `root-config --cflags --glibs` -I ${libPath} -I delphes -I ExRootAnalysis/ExRootAnalysis -L delphes/ -L ExRootAnalysis -Wl,-rpath,${PWD}/delphes -Wl,-rpath,${PWD}/ExRootAnalysis
CXX=g++

all: ${submodules} dict ${executables}

${binPath}/MatchJets: src/MatchJets.cpp ${headers} ${libsources}
	$(CXX) ${CFLAGS} -o $(binPath)/MatchJets src/MatchJets.cpp ${libsources} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/TrainJetClassifier: src/TrainJetClassifier.cpp ${headers} ${libsources}
	${CXX} ${CFLAGS} -o ${binPath}/TrainJetClassifier src/TrainJetClassifier.cpp ${libsources} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/TestJetClassifier: test/TestJetClassifier.cpp ${headers} ${libsources}
	${CXX} ${CFLAGS} -o ${binPath}/TestJetClassifier test/TestJetClassifier.cpp ${libsources} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/MakeEventTrainingSets: src/MakeEventTrainingSets.cpp ${headers} ${libsources}
	${CXX} ${CFLAGS} -o ${binPath}/MakeEventTrainingSets src/MakeEventTrainingSets.cpp ${libsources} -lDelphes -lExRootAnalysis -lTMVA

delphes:
	cd delphes && make

ExRootAnalysis:
	cd ExRootAnalysis && make

dict: ${sources}
	cd ${libPath} && rootcint tmp.cpp -c WWScatteringEvent.h TrainJet.h && mv tmp.cpp wwscatteringdict.cpp
