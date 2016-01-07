#!/usr/bin/python

import parameters
import os
import subprocess

smName = parameters.signalNames[0]
signalNames = parameters.signalNames[1:]
spectraDir = "/export/home/aldenderan/ww-scattering/output/Spectra/"
resultsDir = "/export/home/aldenderan/ww-scattering/output/HypoTestResults/"

sm = spectraDir + smName + ".root"
signals = [spectraDir + name + ".root" for name in signalNames]

ttbar = os.path.join(spectraDir, parameters.backgroundNames["ttbar"])
wp3jets = os.path.join(spectraDir, parameters.backgroundNames["wp3jets"])
wp4jets = os.path.join(spectraDir, parameters.backgroundNames["wp4jets"])

opsString = "--opsww %s --opsww %s --opsww %s --opsww %s" % (signals[0], signals[1], signals[2], signals[3])

cmd = "/export/home/aldenderan/ww-scattering/bin/HypothesisTest --ttbar %s --wp3jets %s --wp4jets %s --smww %s --output %s %s" % (ttbar, wp3jets, wp4jets, sm, "output/HypoTestResults/results.root", opsString)

os.system(cmd)
