import os

repo = "/export/home/aldenderan/ww-scattering/"
events = "/export/share/data/nkgarner/"

jetWeights = os.path.join(repo, "weights/JetClassification_BDTG.weights.xml")
eventWeights = os.path.join(repo, "weights/EventClassification_BDTG.weights.xml")

eventTrainFile = os.path.join(repo, "output/EventClassifierTrainingSets.root")
smSignalHistogram = os.path.join(repo, "output/smSignalHist.root")
ttbarHistogram = os.path.join(repo, "output/ttbarHist.root")

smdelphes = os.path.join(events, "Delphes_OUTPUT/VBFNLO_100K_Events/14TeV_201_100K_Events_1500mjj")
smparton = os.path.join(events, "VBFNLO-2.7.0_OUTPUT/14TeV_201_100K_Events_1500mjj/")
ttbar = os.path.join(events, "Delphes_OUTPUT/MG5_v2_2_1_100K_Events/14TeV_TTBar_100K_Events_1500mjj.root")
wpjets = os.path.join(events, "Delphes_OUTPUT/MG5_v2_2_1_100K_Events/14TeV_Wp3Jets_100K_Events_1500mjj.root")

nTrainSMEvents = 2000
nTrainTTBarEvents = 2000
nTrainWPJetsEvents = 2000

nSignalExperimentEvents = 20000
nTTBarExperimentEvents = 20000


#Event file ranges

jetTrainStart = 0
jetTrainStop = 1

eventTrainStart = 1
eventTrainStop = 10

experimentStart = 11
experimentStop = 50
