#!/usr/bin/env python
# -*- coding: utf-8 -*-

import ROOT
import sys

opt_all = False

def main():
	global opt_all
	if len(sys.argv) < 3:
		print "Usage:", sys.argv[0], "file1.root file2.root"
		print "2 files are required!"
		exit(2)
	inputFile1 = ROOT.TFile(sys.argv[1])
	inputFile2 = ROOT.TFile(sys.argv[2])
	opt_all = (len(sys.argv) > 3 and sys.argv[3] == "-a")
	if compareTree(inputFile1, inputFile2):
		print "The files", inputFile1.GetName(), "and", inputFile2.GetName(), "are"
		print "identical."
		exit(0)
	else:
		print "The files", inputFile1.GetName(), "and", inputFile2.GetName(), "are"
		print "NOT identical."
		exit(1)


def compareBin(histo1, histo2, idx):
	result = True
	if histo1.GetBinCenter(idx) != histo2.GetBinCenter(idx):
		print "center of bin %d not identical: %f vs %f" % (idx, histo1.GetBinCenter(idx), histo2.GetBinCenter(idx))
		if not opt_all:
			return False
		result = False
	if histo1.GetBinWidth(idx) != histo2.GetBinWidth(idx):
		print "widths of bin %d not identical: %f vs %f" % (idx, histo1.GetBinWidth(idx), histo2.GetBinWidth(idx))
		if not opt_all:
			return False
		result = False
	if histo1.GetBinError(idx) != histo2.GetBinError(idx):
		print "errors in bin %d not identical: %f vs %f" % (idx, histo1.GetBinError(idx), histo2.GetBinError(idx))
		if not opt_all:
			return False
		result = False
	if histo1.GetBinContent(idx) != histo2.GetBinContent(idx):
		print "content of bin %d not identical: %f vs %f" % (idx, histo1.GetBinContent(idx), histo2.GetBinContent(idx))
		if not opt_all:
			return False
		result = False
	return result

def compareTH1(directory1, directory2, histoID):
	result = True
	histo1 = directory1.Get(histoID)
	histo2 = directory2.Get(histoID)
	if histo1.GetNbinsX() != histo2.GetNbinsX():
		print "binning not identical"
		if not opt_all:
			return False
		result = False
	for binx in range(0, histo1.GetNbinsX()+2):
		idx = histo1.GetBin(binx)
		if not compareBin(histo1, histo2, idx):
			if not opt_all:
				return False
			result = False
	return result

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
	result = True
	for i in directory2.GetListOfKeys():
		obj = directory1.Get(i.GetName())
		if not obj:
			print "Object %s of type %s in %s is not available in first file" % (i.GetName(), obj.IsA().GetName(), directory2.GetPath().split(":")[1])
			if not opt_all:
				return False
			result = False
	for i in directory1.GetListOfKeys():
		obj = directory1.Get(i.GetName())
		obj2 = directory2.Get(i.GetName())
		if not obj2:
			print "Object %s of type %s in %s is not available in second file" % (i.GetName(), obj.IsA().GetName(), directory1.GetPath().split(":")[1])
			if not opt_all:
				return False
			result = False
			continue
		identified = False
		if obj.IsA().GetName() == "TDirectoryFile":
			identified = True
			if not compareTree(directory1.Get(i.GetName()), directory2.Get(i.GetName())):
				if not opt_all:
					return False
				result = False
		if obj.IsA().GetName().startswith("TH1") or obj.IsA().GetName().startswith("TProfile"):
			identified = True
			if not compareTH1(directory1, directory2, i.GetName()):
				print "problem with %s in %s" % (i.GetName(), directory1.GetPath().split(":")[1])
				if not opt_all:
					return False
				result = False
		if obj.IsA().GetName().startswith("TH2"):
			identified = True
			if not compareTH2(directory1, directory2, i.GetName()):
				print "problem with %s in %s" % (i.GetName(), directory1.GetPath().split(":")[1])
				if not opt_all:
					return False
				result = False
		if not identified:
			print "WARNING: %s of type %s in %s has not been checked" % (i.GetName(), obj.IsA().GetName(), directory1.GetPath().split(":")[1])
	return result



if __name__ == "__main__":
	main()
