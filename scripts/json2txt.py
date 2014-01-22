#!/usr/bin/env python

import sys
import json

if len(sys.argv) < 2 or sys.argv[1] == "-h":
	print "Usage: json2txt.py pileupjson.txt [-d] > output.txt"
	exit(0)

first = 1
if len(sys.argv) > 2 and sys.argv[2] == "-d":
    first = 3

with open(sys.argv[1]) as f:
    values = json.load(f)

for run, lumis in values.iteritems():
    for lumi in lumis:
            #if lumi[0] % 100 == 0:
            #    print run, lumi
            print run, lumi[0], " ".join([str(num) for num in lumi[first:]])
