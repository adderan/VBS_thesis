#!/bin/sh

source scripts/parameters.sh

gdb -ex=r --args bin/MakeEventTrainingSets --ttbarFile ${backgroundsDir}/14TeV_TTBar_100K_Events_1500mjj.root --wpjetsFile ${backgroundsDir}/14TeV_Wp3Jets_100K_Events_1500mjj.root --smwwFile ${eventsDir}/VBFNLO_100K_Events/14TeV_201_100K_Events_1500mjj/event.1.root --weightsFile weights/JetMatching_BDTG.weights.0.xml --outputFile output/event_training.root --nEvents 1000
