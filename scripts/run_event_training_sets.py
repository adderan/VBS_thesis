#!/usr/bin/python

import os
import parameters

smdelphes = os.path.join(parameters.signalEventsDir, '14TeV_201_100K_Events_1500mjj')
ttbar = os.path.join(parameters.backgroundEventsDir, parameters.backgroundNames['ttbar'])
wp3jets = os.path.join(parameters.backgroundEventsDir, parameters.backgroundNames['wp3jets'])
wp4jets = os.path.join(parameters.backgroundEventsDir, parameters.backgroundNames['wp4jets'])

smEventString = ""
for i in range(50):
    smEventString += "--smwwFile %s/event.%i.root " % (smdelphes, i)

cmd = "/export/home/aldenderan/ww-scattering/bin/MakeEventTrainingSets %s --wp3jetsFile %s --wp4jetsFile %s --ttbarFile %s --jetWeights %s --outputFile %s --start %s --stop %s" % (smEventString, wp3jets, wp4jets, ttbar, parameters.jetWeights, parameters.eventTrainFile, parameters.eventTrainStart, parameters.eventTrainStop)

os.system(cmd)
