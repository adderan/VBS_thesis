#!/usr/bin/python 

import os
import parameters
import sys

if sys.argv[1] == "sm":
    eventsFolder = parameters.smdelphes
    histogramFile = parameters.smSignalHistogram
elif sys.argv[1] == "ops":
    eventsFolder = parameters.opsdelphes
    histogramFile = parameters.opsSignalHistogram

eventString = ""
for i in range(50):
    eventString += "--events %s/event.%i.root " % (eventsFolder, i)

cmd = "/export/home/aldenderan/ww-scattering/bin/ReconstructEvents %s --jetClassifierWeights %s --eventClassifierWeights %s --histogramFile %s --start %s --stop %s" % (eventString, parameters.jetWeights, parameters.eventWeights, histogramFile, parameters.experimentStart, parameters.experimentStop)

os.system(cmd)
