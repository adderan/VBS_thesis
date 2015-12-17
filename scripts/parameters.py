import os

repo = "/export/home/aldenderan/ww-scattering/"
events = "/export/share/data/nkgarner/"
partonEventsDir = os.path.join(events, "VBFNLO-2.7.0_OUTPUT/14TeV_201_100K_Events_1500mjj")
signalEventsDir = os.path.join(events, "Delphes_OUTPUT/VBFNLO_100K_Events")
backgroundEventsDir = os.path.join(events, "Delphes_OUTPUT/MG5_v2_2_1_100K_Events")

jetWeights = os.path.join(repo, "weights/JetClassification_BDT.weights.xml")
eventWeights = os.path.join(repo, "weights/EventClassification_LD.weights.xml")

jetTrainFile = os.path.join(repo, "output/TrainingSets/JetClassifierTrainingSets.root")
eventTrainFile = os.path.join(repo, "output/TrainingSets/EventClassifierTrainingSets.root")

signalNames = ['14TeV_201_100K_Events_1500mjj', '14TeV_201_100K_Events_4.6e-6CWWW_1500mjj',
        '14TeV_201_100K_Events_4e-6CWWW_1500mjj', '14TeV_201_100K_Events_5.33e-6CWWW_1500mjj',
        '14TeV_201_100K_Events_6.25e-6CWWW_1500mjj', '14TeV_201_100K_Events_8.16e-6CWWW_1500mjj']

backgroundNames = {'ttbar':'14TeV_TTBar_100K_Events_1500mjj.root', 'wp3jets':'14TeV_Wp3Jets_100K_Events_1500mjj.root', 'wp4jets':'14TeV_Wp4Jets_100K_Events_1500mjj.root'}

#Event range allocations
jetTrainStart = 0
jetTrainStop = 1000
eventTrainStart = 1000
eventTrainStop = 50000
experimentStart = 50000
experimentStop = 100000
