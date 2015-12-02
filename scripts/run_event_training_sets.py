#!/usr/bin/python

import os
import parameters

smEventString = ""
for i in range(50):
    smEventString += "--smwwFile %s/event.%i.root " % (parameters.smdelphes, i)

cmd = "/export/home/aldenderan/ww-scattering/bin/MakeEventTrainingSets %s --wpjetsFile %s --ttbarFile %s --jetWeights %s --outputFile %s --start %s --stop %s" % (smEventString, parameters.wpjets, parameters.ttbar, parameters.jetWeights, parameters.eventTrainFile, parameters.eventTrainStart, parameters.eventTrainStop)

os.system(cmd)
