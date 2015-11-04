
headerPath = inc
binPath = bin
libPath = lib

sources = $(wildcard $(libPath)/*.cpp)
headers = $(wildcard $(headerPath)/*.h)
executables = ${binPath}/ClassifyJets


CFLAGS=-Wall -Werror `root-config --cflags --glibs` -I ${headerPath}
CXX=g++

all: ${executables}

${binPath}/ClassifyJets: event-selection/ClassifyJets.cpp ${headers} ${sources}
	$(CXX) ${CFLAGS} -o $(binPath)/ClassifyJets event-selection/ClassifyJets.cpp ${sources}

#${libPath}/%.o: ${libPath}/%.cpp ${headers}
#	$(CXX) ${CFLAGS} -I ${headerPath} -c -o $@ $<
