import os



jetWeights = "weights/JetClassification_BDTG.weights.xml"
eventWeights = "weights/EventClassification_BDTG.weights.xml"

eventTrainFile = "output/EventClassifierTrainingSets.root"

nTrainSMEvents = 200
nTrainTTBarEvents = 200
nTrainWPJetsEvents = 200

#Event file ranges

jetTrainStart = 0
jetTrainStop = 1

eventTrainStart = 1
eventTrainStop = 10

experimentStart = 11
experimentStop = 50
