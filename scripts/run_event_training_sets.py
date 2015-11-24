#!/usr/bin/python
import os
import parameters

smEventString = ""
for i in range(parameters.eventTrainStart, parameters.eventTrainStop):
    smEventString += "--smwwFile smdelphes/event.%i.root " % i

cmd = "bin/MakeEventTrainingSets %s --wpjetsFile wpjetsdelphes --ttbarFile ttbardelphes --nSMEvents %i --nWPJetsEvents %i --nTTBarEvents %i --jetWeights %s --outputFile %s" % (smEventString, parameters.nTrainSMEvents, parameters.nTrainWPJetsEvents, parameters.nTrainTTBarEvents, parameters.jetWeights, parameters.eventTrainFile)

os.system(cmd)
