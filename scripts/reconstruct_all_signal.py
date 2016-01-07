#!/usr/bin/python
import os
import parameters

for signalName in parameters.badSignalNames:
    folder = os.path.join(parameters.signalEventsDir, signalName)
    histogram = os.path.join(parameters.repo, "output/Spectra", signalName)
    histogram += ".root"
    logFile = os.path.join(parameters.repo, "output/logs/", signalName)
    eventString = ""
    for i in range(50):
        eventString += "--events %s/event.%i.root " % (folder, i)

    cmd = "/export/home/aldenderan/ww-scattering/bin/ReconstructEvents %s --jetClassifierWeights %s --eventClassifierWeights %s --histogramFile %s --start %s --stop %s > %s" % (eventString, parameters.jetWeights, parameters.eventWeights, histogram, parameters.experimentStart, parameters.experimentStop, logFile)

    os.system(cmd)
