#!/usr/bin/python

import parameters
import os
import sys

if sys.argv[1] == "ttbar":
    eventsFile = parameters.ttbar
    histogramFile = parameters.ttbarHistogram
elif sys.argv[1] == "wpjets":
    eventsFile = parameters.wpjets
    histogramFile = parameters.wpjetsHistogram

cmd = "/export/home/aldenderan/ww-scattering/bin/ReconstructEvents --events %s --jetClassifierWeights %s --eventClassifierWeights %s --histogramFile %s --start %s --stop %s" % (eventsFile, parameters.jetWeights, parameters.eventWeights, histogramFile, parameters.experimentStart, parameters.experimentStop)

os.system(cmd)
