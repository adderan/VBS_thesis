#!/usr/bin/python
import os
import parameters

for backgroundName in parameters.backgroundNames.values():
    filePath = os.path.join(parameters.backgroundEventsDir, backgroundName)
    histogram = os.path.join(parameters.repo, "output/Spectra", backgroundName)
    logFile = os.path.join(parameters.repo, "output/logs", backgroundName)

    cmd = "/export/home/aldenderan/ww-scattering/bin/ReconstructEvents --events %s --jetClassifierWeights %s --eventClassifierWeights %s --histogramFile %s --start %s --stop %s > %s" % (filePath, parameters.jetWeights, parameters.eventWeights, histogram, parameters.experimentStart, parameters.experimentStop, logFile)
    os.system(cmd)


