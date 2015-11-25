#!/usr/bin/python 

import os
import parameters
import sys

eventString = ""
for i in range(parameters.experimentStart, parameters.experimentStop):
    eventString += "--events %s/event.%i.root " % (parameters.smdelphes, i)

cmd = "/export/home/aldenderan/ww-scattering/bin/ReconstructEvents %s --jetClassifierWeights %s --eventClassifierWeights %s --nEvents %s --histogramFile %s" % (eventString, parameters.jetWeights, parameters.eventWeights, parameters.nSignalExperimentEvents, parameters.smSignalHistogram)

os.system(cmd)
