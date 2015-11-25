#!/usr/bin/python

import os
import parameters

smEventString = ""
for i in range(parameters.eventTrainStart, parameters.eventTrainStop):
    smEventString += "--smwwFile %s/event.%i.root " % (parameters.smdelphes, i)

cmd = "/export/home/aldenderan/ww-scattering/bin/MakeEventTrainingSets %s --wpjetsFile %s --ttbarFile %s --nSMEvents %i --nWPJetsEvents %i --nTTBarEvents %i --jetWeights %s --outputFile %s" % (smEventString, parameters.wpjets, parameters.ttbar, parameters.nTrainSMEvents, parameters.nTrainWPJetsEvents, parameters.nTrainTTBarEvents, parameters.jetWeights, parameters.eventTrainFile)

os.system(cmd)
