#!/usr/bin/python

import parameters
import os
import subprocess
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--prefix", type = str)
args = parser.parse_args()
prefix = args.prefix
print "prefix = " + prefix



smName = parameters.signalNames[0]
spectraDir = os.path.join(prefix, "Spectra/")
resultsDir = os.path.join(prefix, "HypoTestResults/")
print "Spectra dir = " + spectraDir

sm = spectraDir + smName + ".root"
signals = [" --opsww " + spectraDir + name + ".root" for name in parameters.signalNames]

ttbar = os.path.join(spectraDir, parameters.backgroundNames["ttbar"])
wp3jets = os.path.join(spectraDir, parameters.backgroundNames["wp3jets"])
wp4jets = os.path.join(spectraDir, parameters.backgroundNames["wp4jets"])

opsString = signals[1] + signals[2] + signals[3] + signals[4]

cmd = "/export/home/aldenderan/ww-scattering/bin/HypothesisTest --ttbar %s --wp3jets %s --wp4jets %s --smww %s --output %s %s" % (ttbar, wp3jets, wp4jets, sm, os.path.join(resultsDir, "results.root"), opsString)

os.system(cmd)
