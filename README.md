# ww-scattering

This is a set of ROOT scripts for analyzing vector boson scattering at the LHC, specifically for the process W+ W- > l v j j. 
Contains scripts for:

1. Training a TMVA classifier to identify the tag jets for a particular event (among jets from multiple pileup vertices)
2. Training a TMVA classifier to distinguish the desired WW scattering event from top quark production and W+Jets events that have similar
final-state particles.
3. Creating a histogram of the WW pair invariant mass for a set of events, after selecting events that pass the TMVA event
classifier.

This is heavily based on work done by Niklas Garner in his thesis "Vector Boson Scattering at the High-Luminosity Large Hadron Collider."
