CFLAGS=-Wall -Werror `root-config --cflags --glibs`
LDFLAGS=`root-config --glibs`
CXX=g++

srcPath = src
headerPath = inc
binPath = bin
libPath = lib

sources = $(wildcard $(srcPath)/*.cpp)
headers = $(wildcard $(headerPath)/*.hh)
objects = $(sources:$(srcPath)/%.cpp=$(libPath)/%.o)
executables = ${binPath}/ClassifyJets

all: ${executables}

${binPath}/ClassifyJets: ${srcPath}/ClassifyJets.cpp ${headers}
	$(CXX) ${CFLAGS} -o $(binPath)/ClassifyJets ${srcPath}/ClassifyJets.cpp

#${libPath}/%.o: ${srcPath}/%.cpp ${headers}
#	$(CXX) ${CFLAGS} -I ${headerPath} -c -o $@ $<



