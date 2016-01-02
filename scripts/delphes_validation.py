#!/usr/bin/python
import os
import parameters


os.system("%s %s %s %s" % (parameters.delphes, parameters.mscard, parameters.smearedMSFile, parameters.muonFile))
os.system("%s %s %s %s" % (parameters.delphes, parameters.idcard, parameters.smearedIDFile, parameters.muonFile))
os.system("%s %s %s %s" % (parameters.delphes, parameters.electroncard, parameters.smearedElectronFile, parameters.electronFile))
