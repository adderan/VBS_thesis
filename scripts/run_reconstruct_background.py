#!/usr/bin/python

import parameters
import os


cmd = "/export/home/aldenderan/ww-scattering/bin/ReconstructEvents --events %s --jetClassifierWeights %s --eventClassifierWeights %s --nEvents %s --histogramFile %s" % (parameters.ttbar, parameters.jetWeights, parameters.eventWeights, parameters.nTTBarExperimentEvents, parameters.ttbarHistogram)

os.system(cmd)
