import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = ["ak5PFJetsL1L2L3", "ak5PFJetsL1L2L3NoPU"]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/hauth/ZPJ2011/DYToMuMu_Summer11-PU/DYToMuMu_Summer11-PU_DYToMuMu_M-20_TuneZ2_7TeV-pythia6_*.root"
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "analysis/chs_Summer11_mc_npvrw" #mc_summer11_dy_v10_rw"

# apply weighting to the events
conf["UseWeighting"] = 1
# use the weghting specified in the event
# if you have PtBins, this must be 0 and the above must be 1. Then the Weights are calculated by the 
# resp_cuts code. But please double check if the xsections in resp_cuts are still right.
conf["UseEventWeight"] = 1
conf["UseGlobalWeightBin"] = 1
conf["GlobalXSection"] = 1614.0
conf["EventReweighting"] = 1
#Flat10
#conf["RecovertWeight"] = [0.2715354733, 0.4212003633, 0.9564475568, 1.5586546103, 1.9990027937, 2.1412115719, 1.9891186544, 1.6445764847, 
#1.2329916779, 0.8501083476, 0.5449123796, 0.3629201983, 
#0.2465954906, 0.1740523972, 0.1234214198, 0.0921563301, 0.0701540395, 0.0549648806, 0.0445059793, 0.0385417703, 0.0312118689, 0.0306615240, 
#0.0260844992, 0.0229561025, 0.0205639293]
#NPV in MC
conf["RecovertWeight"] = [0.2634339699, 0.4068300319, 1.0258412624, 1.5039872842, 2.1501353803, 1.9674930073, 1.7357207863, 1.5885466557, 1.2814939016, 0.8379304030, 
0.5751357475, 0.3933389880, 0.2618616395, 0.1928669420, 0.1178827060, 0.0989967695, 0.0707225141, 0.0494813344, 0.0630199599, 0.0275894575, 
0.0189547094, 0.0708500595, 0.0581618600, 0.0115549447, 0.0094252128]
#Recovertices


wghsum = 0
prob = [0.0400676665, 0.0403580090, 0.0807116334, 0.0924154156, 0.0924154156,
        0.0924154156, 0.0924154156, 0.0924154156, 0.0870356742, 0.0767913175,
        0.0636400516, 0.0494955563, 0.0362238310, 0.0249767088, 0.0162633216,
        0.0099919945, 0.0058339324, 0.0032326433, 0.0017151846, 0.0008505404,
        0.0004108859, 0.0001905137, 0.0000842383, 0.0000349390, 0.0000142801]

prob10 = [0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584,
	0.0630151648, 0.0526654164, 0.0402754482, 0.0292988928, 0.0194384503,
	0.0122016783, 0.0072070420, 0.0040036370, 0.0020278322, 0.0010739954,
	0.0004595759, 0.0002229748, 0.0001028162, 0.0000458337152809607]

for i in range(25):
	wgh = conf["RecovertWeight"][i]
	wghsum +=wgh*prob10[i]
print "The sum of weights is", wghsum

conf["ThreadCount"] = 1

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,15,30,60,100,500], conf, True )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
