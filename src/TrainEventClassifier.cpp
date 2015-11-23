#include <iostream>
#include <getopt.h>

#include "TMVA/Factory.h"

#include "WWScatteringEvent.h"


int main(int argc, char **argv) {
    char *eventTrainingFileName = NULL;
    char *outputFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"eventTrainFile", required_argument, 0, 'a'},
            {"outputFile", required_argument, 0, 'b'}
        };
        c = getopt_long(argc, argv, "ab:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                eventTrainingFileName = optarg;
                break;
            case 'b':
                outputFileName = optarg;
                break;
        }
    }
}

