#!/usr/bin/python
import os, parameters

os.system("bin/SmearingValidation --delphes %s --plot %s --resFunction %s" % (parameters.smearedMSFile, parameters.smearedMSPlot, "ms"))
os.system("bin/SmearingValidation --delphes %s --plot %s --resFunction %s" % (parameters.smearedIDFile, parameters.smearedIDPlot, "id"))
os.system("bin/SmearingValidation --delphes %s --plot %s --resFunction %s" % (parameters.smearedElectronFile, parameters.smearedElectronPlot, "electron"))
