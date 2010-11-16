#!/usr/bin/python
# -*- coding: utf-8 -*-

import csv
import os

intLumis = {}
intLumisDeliv = {}

def loadFile(inputFile):
	global intLumis
	spamReader = csv.reader(open(inputFile), delimiter=',', quotechar='|')
	for row in spamReader:
		if row[0]=="run" or not row[0].isdigit():# or row[2]=="N/A":
			continue
		if not row[0] in intLumis:
			intLumis[row[0]]=0.
			intLumisDeliv[row[0]]=0.
		intLumis[row[0]]=intLumis[row[0]]+float(row[3])/1000.
		intLumisDeliv[row[0]]=intLumisDeliv[row[0]]+float(row[2])/1000.

def getAllIntLumis():
	tmpSum = 0
	ret = {}
	allRuns = intLumis.keys()
	allRuns.sort()
	for run in allRuns:
		tmpSum+=intLumis[run]
		ret[run]=tmpSum
	return ret

def getAllIntLumisDeliv():
	tmpSum = 0
	ret = {}
	allRuns = intLumisDeliv.keys()
	allRuns.sort()
	for run in allRuns:
		tmpSum+=intLumisDeliv[run]
		ret[run]=tmpSum
	return ret

def getIntLumi(run):
	return intLumis[run]

def getIntLumiUpTo(run):
	ret = 0.
	allRuns = intLumis.keys()
	allRuns.sort()
	for run in allRuns[0:allRuns.index(run)+1]:
		ret+=intLumis[run]
		#print run, ret
	return ret

def getTotalIntLumi():
	ret = 0.
	allRuns = intLumis.keys()
	allRuns.sort()
	for run in allRuns:
		ret+=intLumis[run]
		#print run, ret
	return ret

filenames = os.listdir("../../json")
filenames.sort()
for f in filenames:
	if f.startswith("lumi_"):
		print f
		loadFile(os.path.join("../../json",f))

print getIntLumiUpTo('144114')
print getIntLumiUpTo('147116')-getIntLumiUpTo('144114')
print getIntLumiUpTo('148058')-getIntLumiUpTo('147116')
print getIntLumiUpTo('148864')-getIntLumiUpTo('148058')

print getTotalIntLumi()
#print getIntLumi('140387')
#print getIntLumiUpTo('140387')


