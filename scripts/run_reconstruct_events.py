#!/usr/bin/python 

import os
import parameters
import sys

start = 0
end = 20

eventString = ""
for i in range(start, end):
    eventString += "--events smdelphes/event.%i.root " % i

cmd = "bin/ReconstructEvents %s --jetClassifierWeights %s --eventClassifierWeights %s --histogramFile output/smdelphes.root" % (eventString, parameters.jetWeights, parameters.eventWeights)

os.system(cmd)
