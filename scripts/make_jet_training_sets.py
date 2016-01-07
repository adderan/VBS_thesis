#!/usr/bin/python

import parameters
import os

delphes0 = os.path.join(parameters.signalEventsDir, "14TeV_201_100K_Events_1500mjj/event.0.root")
parton0 = os.path.join(parameters.partonEventsDir, "event.0.root")
cmd = "bin/MakeJetTrainingSets --inputDelphesFile %s --inputVBFNLOFile %s --outputFile %s --start %s --stop %s" % (delphes0, parton0, parameters.jetTrainFile, parameters.jetTrainStart, parameters.jetTrainStop)

os.system(cmd)
