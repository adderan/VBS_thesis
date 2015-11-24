#!/usr/bin/python
import parameters
import os

cmd = "bin/TrainEventClassifier --eventTrainFile %s --outputFile output/EventTrainLog.root" % parameters.eventTrainFile

os.system(cmd)
