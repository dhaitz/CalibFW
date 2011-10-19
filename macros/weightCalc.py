# -*- coding: utf-8 -*-
import argparse
import getROOT

def main():
	op = options()
	data = getDataDistribution(op.files, op.data_histo, op.verbose)
	mc = getMCDistribution(op.mc, op.mc_histo, op.verbose)
	weights = calcWeights(data, mc, op.verbose, not op.no_warning)
	printWeights(weights, op.breaklines)
	if op.check: 
		print "Normalization: before (%1.5f) and after (%1.5f) reweighting." % (
			sum(mc), sum(map(lambda a,b: a*b, mc, weights)) )


def calcWeights(dataDistribution, mcDistribution, verbose=False, warn=True):
	result = []
	# check length and add zeros
	ldiff = len(dataDistribution) - len(mcDistribution)
	dataDistribution += [0.0]*(-ldiff)
	mcDistribution += [0.0]*(+ldiff)
	if verbose:
		print "Weighting factors are calculated for up to", 
		print len(dataDistribution)-1, "pile-up interactions from:"
		print "Data:", dataDistribution
		print "MC:  ", mcDistribution
	assert len(dataDistribution) == len(mcDistribution)
	# divide
	for npu in range(len(dataDistribution)):
		if mcDistribution[npu] == 0:
			result.append(0.0)
			if warn:
				print "WARNING: There are no MC events with", npu, "pile-up interactions!"
		else:
			result.append(dataDistribution[npu] / mcDistribution[npu])
	assert len(result) == len(mcDistribution)
	# normalize (i.e. sum_i result[i]*mcDist[i] == 1)
	result = [a/sum(dataDistribution) for a in result]
	return result


def printWeights(weights, nice=False):
	s = "[" + ", ".join("%1.9f" % w for w in weights) + "]"
	if nice:
		s = "conf[\"RecovertWeight\"] = " + s
		while s.rfind(",",0,80)>0:
			i = max(s.rfind(",",0,80), s.rfind("]",0,80)) + 1
			print "        " + s[0:i]
			s = " " + s[i:]
	else:
		print s


def getDataDistribution(files, histo="pileup", verbose=False):
	result = []
	for f in files:
		rootfile = getROOT.OpenFile(f, verbose)
		dist = getROOT.SafeConvert(rootfile, histo).y
		dist.pop(-1)
		result += [0.0]*(len(dist) - len(result))
		dist += [0.0]*(len(result) - len(dist))
		assert len(dist) == len(result)
		result = map(lambda a,b: a+b, result, dist)
	return result


def getMCDistribution(source, histo="pu", verbose=False):
	result = []
	#	3 cases for source: .py file, .root file, key from dict below
	if source[-3:] == ".py":
		with open(source) as f:
			for line in f:
				if line.find("probValue")>0:
					result=line[line.find("(")+1:line.find(")")].split(",")
		if result == []: 
			print "No values found in this .py file!"
			assert False
		result = [float(s) for s in result]
	elif source [-5:] == ".root":
		rootfile = getROOT.OpenFile(source, verbose)
		result = getROOT.SafeConvert(rootfile, histo).y
		result.pop(-1)
	elif source in std_values.keys():
		result = std_values[source]
	else:
		print "This MC distribution could not be found!"
		assert False
	s = sum(result)
	return [w/s for w in result]


def options():
	parser = argparse.ArgumentParser(
		description="%(prog)s calculates the weights for MC reweighting according to the number of pile-up interactions. It needs at least a root file with the estimated pile-up distribution in data. The Monte-Carlo simulated distribution of the number of pile-up interactions should be provided, too. This is done via the -m option.")
	parser.add_argument('files', metavar='file', type=str, nargs='+',
		help="root file(s) containing the estimated number of pile-up interactions in data. The name of the contained histogram is specified with -D.")
	parser.add_argument('-m','--mc', type=str, default="summer11pythia",
		help="source of the MC pile-up distribution. You can either use a CMSSW mixing module config file from http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/SimGeneral/MixingModule/python/, a resp_cuts or any other root file containing a histogram with the pile-up distribution in MC or a production name from this list: " + str(std_values.keys()) + ". In case you use a root file, you can specify the name of the pile-up histogram with -M. Default is 'summer11'.")
	parser.add_argument('-M','--mc-histo', type=str, default="NoBinning_allevents/pu_ak5PFJetsL1L2L3CHS_hist",
		help="histogram name in the Monte-Carlo pile-up distribution files")
	parser.add_argument('-D','--data-histo', type=str, default="pileup",
		help="histogram name in the file(s) containing pile-up distributions estimated in data")
	parser.add_argument('-v', '--verbose', action='store_true', help="verbosity")
	parser.add_argument('-b', '--breaklines', action='store_true', help="linebreak the output")
	parser.add_argument('-c', '--check', action='store_true', help="check wether the weights are correctly normalized. If no MC events are availabe for some numbers of pile-up interactions, the distribution can not be correctly normalized and the weights do not exactly add up to unity.")
	parser.add_argument('-q', '--no-warning', action='store_true', help="Do not print warnings if the Monte Carlo sample does not contain events for all numbers of pile-up interactions.")
	return parser.parse_args()


std_values = {
	'summer11pythia':
	# /DYToMuMu_M-20_TuneZ2_7TeV-pythia6/Summer11-PU_S3_START42_V11-v2/AODSIM
		[0.07049992413220632, 0.070960486267929349, 0.064393013022483639, 
		0.071806635307978608, 0.064487624624008144, 0.076255165704187014, 
		0.081153547488776026, 0.073620321857957638, 0.068720154948811549, 
		0.071779858439622618, 0.066843989039335222, 0.058434267250997436, 
		0.049538991583137716, 0.036023813561591261, 0.030207877754670331, 
		0.017778055463819987, 0.011880003927274025, 0.007883110044003321, 
		0.002804430679150638, 0.002804430679150638, 0.001728000571239858, 
		0.000199933950391389, 0.000000000000000000, 0.000196363701277257],
	'summer11powheg':
 	# /DYToMuMu_M-20_CT10_TuneZ2_7TeV-powheg-pythia/Summer11-PU_S4_START42_V11-v1/AODSIM
		[0.10278166315726263, 0.061029119810069830, 0.068822525901295561,
		0.069451315755540158, 0.070465203576788926, 0.070445020506373301,
		0.069446332281363460, 0.067167514127214889, 0.064493132710291085,
		0.060025323524028881, 0.054822825657267295, 0.048332348889538676,
		0.041588960807342749, 0.035223195480885948, 0.028763616253052845,
		0.022983658316066635, 0.017889675599502601, 0.013841101178354928,
		0.010082689541145369, 0.007409553992765740, 0.005231152843277586,
		0.003750064317963593, 0.002488996177550720, 0.001668591741212173,
		0.001082410591178329, 0.000714007262666091],
}

if __name__ == "__main__":
	main()
