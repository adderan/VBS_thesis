#!/usr/bin/python
import os
import parameters

for backgroundName in parameters.backgroundNames.values():
    filePath = os.path.join(parameters.backgroundEventsDir, backgroundName)
    histogram = os.path.join(parameters.repo, "output/Spectra", backgroundName)

    cmd = "/export/home/aldenderan/ww-scattering/bin/ReconstructEvents --events %s --jetClassifierWeights %s --eventClassifierWeights %s --histogramFile %s --start %s --stop %s" % (filePath, parameters.jetWeights, parameters.eventWeights, histogram, parameters.experimentStart, parameters.experimentStop)
    os.system(cmd)


