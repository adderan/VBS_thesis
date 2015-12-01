#!/usr/bin/python

import parameters
import os
import sys

if sys.argv[1] == "ttbar":
    nEvents = parameters.nTTBarExperimentEvents
    eventsFile = parameters.ttbar
    histogramFile = parameters.ttbarHistogram
elif sys.argv[1] == "wpjets":
    nEvents = parameters.nWPJetsExperimentEvents
    eventsFile = parameters.wpjets
    histogramFile = parameters.wpjetsHistogram

cmd = "/export/home/aldenderan/ww-scattering/bin/ReconstructEvents --events %s --jetClassifierWeights %s --eventClassifierWeights %s --nEvents %s --histogramFile %s" % (eventsFile, parameters.jetWeights, parameters.eventWeights, nEvents, histogramFile)

os.system(cmd)
