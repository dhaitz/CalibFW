#!/usr/bin/env python
# -*- coding: utf-8 -*-

import ROOT
import sys

def main():
	if len(sys.argv) != 3:
		print "Usage:", sys.argv[0], "file1.root file2.root"
		print "2 files are required!"
		exit(2)
	inputFile1 = ROOT.TFile(sys.argv[1])
	inputFile2 = ROOT.TFile(sys.argv[2])

	if compareTree(inputFile1, inputFile2):
		print "identical"
		exit(0)
	else:
		print "-> NOT IDENTICAL <-"
		exit(1)

def compareBin(histo1, histo2, idx):
	if histo1.GetBinCenter(idx) != histo2.GetBinCenter(idx):
		print "center of bin not identical: %f vs %f" % (histo1.GetBinCenter(idx), histo2.GetBinCenter(idx))
		return False
	if histo1.GetBinWidth(idx) != histo2.GetBinWidth(idx):
		print "widths of bin not identical: %f vs %f" % (histo1.GetBinWidth(idx), histo2.GetBinWidth(idx))
		return False
	if histo1.GetBinError(idx) != histo2.GetBinError(idx):
		print "errors in bin not identical: %f vs %f" % (histo1.GetBinError(idx), histo2.GetBinError(idx))
		return False
	if histo1.GetBinContent(idx) != histo2.GetBinContent(idx):
		print "bin content not identical: %f vs %f" % (histo1.GetBinContent(idx), histo2.GetBinContent(idx))
		return False
	return True

def compareTH1(directory1, directory2, histoID):
	histo1 = directory1.Get(histoID)
	histo2 = directory2.Get(histoID)
	if histo1.GetNbinsX() != histo2.GetNbinsX():
		print "binning not identical"
		return False
	#try:
	for binx in range(0, histo1.GetNbinsX()+2):
		idx = histo1.GetBin(binx)
		if not compareBin(histo1, histo2, idx):
			return False
	#except:
	#	print "general error"
	#	return False
	return True

def compareTH2(directory1, directory2, histoID):
	histo1 = directory1.Get(histoID)
	histo2 = directory2.Get(histoID)
	if histo1.GetNbinsX() != histo2.GetNbinsX() or histo1.GetNbinsY() != histo2.GetNbinsY():
		print "binning not identical"
		return False
	try:
		for binx in range(0, histo1.GetNbinsX()+2):
			for biny in range(0, histo1.GetNbinsY()+2):
				idx1 = histo1.GetBin(binx, biny)
				idx2 = histo2.GetBin(binx, biny)
				if idx1 != idx2:
					print "binning not identical"
					return False
				if not compareBin(histo1, histo2, idx1):
					return False
	except:
		print "general error"
		return False
	return True

def compareTH3(directory1, directory2, histoID):
	histo1 = directory1.Get(histoID)
	histo2 = directory2.Get(histoID)
	if histo1.GetNbinsX() != histo2.GetNbinsX() or histo1.GetNbinsY() != histo2.GetNbinsY() or histo1.GetNbinsZ() != histo2.GetNbinsZ():
		print "binning not identical"
		return False
	try:
		for binx in range(0, histo1.GetNbinsX()+2):
			for biny in range(0, histo1.GetNbinsY()+2):
				for binz in range(0, histo1.GetNbinsZ()+2):
					idx1 = histo1.GetBin(binx, biny, binz)
					idx2 = histo2.GetBin(binx, biny, binz)
					if idx1 != idx2:
						print "binning not identical"
						return False
					if not compareBin(histo1, histo2, idx1):
						return False
	except:
		print "general error"
		return False
	return True

def compareTree(directory1, directory2):
	for i in directory2.GetListOfKeys():
		obj = directory1.Get(i.GetName())
		if not obj:
			print "Object %s of type %s in %s is not available in first file" % (i.GetName(), obj.IsA().GetName(), directory2.GetPath().split(":")[1])
			return False
	for i in directory1.GetListOfKeys():
		obj = directory1.Get(i.GetName())
		obj2 = directory2.Get(i.GetName())
		if not obj2:
			print "Object %s of type %s in %s is not available in second file" % (i.GetName(), obj.IsA().GetName(), directory1.GetPath().split(":")[1])
			return False
		identified = False
		if obj.IsA().GetName() == "TDirectoryFile":
			identified = True
			if not compareTree(directory1.Get(i.GetName()), directory2.Get(i.GetName())):
				return False
		if obj.IsA().GetName().startswith("TH1") or obj.IsA().GetName().startswith("TProfile"):
			identified = True
			if not compareTH1(directory1, directory2, i.GetName()):
				print "problem with %s in %s" % (i.GetName(), directory1.GetPath().split(":")[1])
				return False
			#traverseTree(directory.Get(i.GetName()))
		if obj.IsA().GetName().startswith("TH2"):
			identified = True
			if not compareTH2(directory1, directory2, i.GetName()):
				print "problem with %s in %s" % (i.GetName(), directory1.GetPath().split(":")[1])
				return False
		if not identified:
			print "WARNING: %s of type %s in %s has not been checked" % (i.GetName(), obj.IsA().GetName(), directory1.GetPath().split(":")[1])
	return True



if __name__ == "__main__":
	main()
