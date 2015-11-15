binPath = bin
libPath = lib

libsources = $(wildcard $(libPath)/*.cpp)
headers = $(wildcard $(libPath)/*.h)
executables = ${binPath}/MatchJets ${binPath}/TrainJetClassifier ${binPath}/TestJetClassifier ${binPath}/MakeEventTrainingSets


CFLAGS=-g -O0 -Wall -Werror `root-config --cflags --glibs` -I ${libPath} -I delphes -I ExRootAnalysis/ExRootAnalysis -L delphes/ -L ExRootAnalysis -Wl,-rpath,${PWD}/delphes -Wl,-rpath,${PWD}/ExRootAnalysis
CXX=g++
external = delphes/libDelphes.so ExRootAnalysis/libExRootAnalysis.so

all: delphes ExRootAnalysis dict ${executables}

${binPath}/MatchJets: src/MatchJets.cpp ${headers} ${libsources} ${external}
	$(CXX) ${CFLAGS} -o $(binPath)/MatchJets src/MatchJets.cpp ${libsources} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/TrainJetClassifier: src/TrainJetClassifier.cpp ${headers} ${libsources} ${external}
	${CXX} ${CFLAGS} -o ${binPath}/TrainJetClassifier src/TrainJetClassifier.cpp ${libsources} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/TestJetClassifier: test/TestJetClassifier.cpp ${headers} ${libsources} ${external}
	${CXX} ${CFLAGS} -o ${binPath}/TestJetClassifier test/TestJetClassifier.cpp ${libsources} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/MakeEventTrainingSets: src/MakeEventTrainingSets.cpp ${headers} ${libsources} ${external}
	${CXX} ${CFLAGS} -o ${binPath}/MakeEventTrainingSets src/MakeEventTrainingSets.cpp ${libsources} -lDelphes -lExRootAnalysis -lTMVA

delphes/libDelphes.so:
	cd delphes && make

ExRootAnalysis/libExRootAnalysis.so:
	cd ExRootAnalysis && make

dict: ${sources}
	cd ${libPath} && rootcint tmp.cpp -c WWScatteringEvent.h TrainJet.h && mv tmp.cpp wwscatteringdict.cpp
