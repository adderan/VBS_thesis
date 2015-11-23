#!/usr/bin/python
import os

eventString = ""
start = 0
end = 5
for i in range(start, end):
    eventString += "--e smparton/event.%i.root " % i

cmd = "bin/WWReconstructTest %s --histogram output/smparton.root" % eventString


os.system(cmd)
