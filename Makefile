binPath = bin
libPath = ${PWD}/lib
tmpPath = ${PWD}/tmp

libsources = $(wildcard $(libPath)/*.cpp)
headers = $(wildcard $(libPath)/*.h)
executables = ${binPath}/MakeJetTrainingSets ${binPath}/TrainJetClassifier ${binPath}/TestJetClassifier ${binPath}/MakeEventTrainingSets ${binPath}/ReconstructEvents ${binPath}/WWReconstructTest ${binPath}/TrainEventClassifier ${binPath}/HypothesisTest


CFLAGS=-g -O0 -Wall -Werror `root-config --cflags --glibs` -I ${libPath} -I delphes -I ExRootAnalysis/ExRootAnalysis -L delphes/ -L ExRootAnalysis -Wl,-rpath,${PWD}/delphes -Wl,-rpath,${PWD}/ExRootAnalysis
CXX=g++
external = delphes/libDelphes.so ExRootAnalysis/libExRootAnalysis.so
dict = ${tmpPath}/wwscatteringdict.cpp

all: ${executables}

${binPath}/MakeJetTrainingSets: src/MakeJetTrainingSets.cpp ${headers} ${libsources} ${external} ${dict}
	$(CXX) ${CFLAGS} -o $(binPath)/MakeJetTrainingSets src/MakeJetTrainingSets.cpp ${libsources} ${dict} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/TrainJetClassifier: src/TrainJetClassifier.cpp ${headers} ${libsources} ${external} ${dict}
	${CXX} ${CFLAGS} -o ${binPath}/TrainJetClassifier src/TrainJetClassifier.cpp ${libsources} ${dict} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/TestJetClassifier: test/TestJetClassifier.cpp ${headers} ${libsources} ${external} ${dict}
	${CXX} ${CFLAGS} -o ${binPath}/TestJetClassifier test/TestJetClassifier.cpp ${libsources} ${dict} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/MakeEventTrainingSets: src/MakeEventTrainingSets.cpp ${headers} ${libsources} ${external} ${dict}
	${CXX} ${CFLAGS} -o ${binPath}/MakeEventTrainingSets src/MakeEventTrainingSets.cpp ${libsources} ${dict} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/ReconstructEvents: src/ReconstructEvents.cpp ${headers} ${libsources} ${external} ${dict}
	${CXX} ${CFLAGS} -o ${binPath}/ReconstructEvents src/ReconstructEvents.cpp ${libsources} ${dict} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/WWReconstructTest: test/WWReconstructTest.cpp ${headers} ${libsources} ${external} ${dict}
	${CXX} ${CFLAGS} -o ${binPath}/WWReconstructTest test/WWReconstructTest.cpp ${libsources} ${dict} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/TrainEventClassifier: src/TrainEventClassifier.cpp ${headers} ${libsources} ${external} ${dict}
	${CXX} ${CFLAGS} -o ${binPath}/TrainEventClassifier src/TrainEventClassifier.cpp ${libsources} ${dict} -lDelphes -lExRootAnalysis -lTMVA

${binPath}/HypothesisTest: src/HypothesisTest.cpp ${headers} ${libsources} ${dict} ${external}
	${CXX} ${CFLAGS} -o ${binPath}/HypothesisTest src/HypothesisTest.cpp ${libsources} ${dict} -lDelphes -lExRootAnalysis -lTMVA -lRooFit -lRooStats

delphes/libDelphes.so:
	cd delphes && make

ExRootAnalysis/libExRootAnalysis.so:
	cd ExRootAnalysis && make

${tmpPath}/wwscatteringdict.cpp: ${libsources} ${headers}
	cd ${libPath} && rootcint -f ${tmpPath}/wwscatteringdict.cpp -c WWScatteringEvent.h TrainJet.h
