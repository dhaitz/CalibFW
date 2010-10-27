import ZPJConfiguration
import ResponseBaseConfig

# -*- coding: utf-8 -*-
# Here the parameters for all the plots are stated

#-----------------------------------
def multiline_text(line1,line2,line3="",line4=""):
    string = "#scale[.8]{#splitline{#splitline{%s}{%s}}{#splitline{%s}{%s}}}" %(line1,line2,line3,line4)
    return string
#-----------------------------------


p = ResponseBaseConfig.GetResponseBaseConfig() 

# modify how to bin data here
p.general.source_files=[ "/local/scratch/hauth/data/ZPJ2010/zplusjet_mc_fall10_weighted.root", "/local/scratch/hauth/data/ZPJ2010/zplusjet_mc_fall10.root"]
p.general.source_number=[0, 1]
p.general.source_type=["mc","mc"]
p.general.pt_bins=[0.0, 25.0, 30.0, 36.0, 43.0, 51.0, 61.0, 73.0, 87.0, 104.0, 124.0, 148.0, 177.0, 212.0, 254.0, 304.0, 364.0]
p.general.algos=["ak5PF","ak5PF"]#["ak5PF", "ak7PF", "kt4PF", "kt6PF", "iterativeCone5PF"]
p.general.good_algos=["MC Fall 10 weighted", "MC Fall 10 PtHat bins"]#["anti-kt 0.5 PF", "anti-kt 0.7 PF", "kt 0.4 PF", "kt 0.6 PF", "iterativeCone5 PF"]
p.general.global_prefix = "JetResponse_ak5PF_MC_data_compare_"
p.general.info_string= multiline_text("p_{T}^{#mu}>15 GeV , |#eta^{#mu}|<2.3 , |M-M_{Z}|<20 GeV",
                                "|#eta^{jet}|<1.3 , |#Delta #phi (Z,jet) - #pi|<0.2 , p_{T}^{jet2}/p_{T}^{Z}< 0.2",
                                "anti-kt 0.5 Particle Flow jets",
                                "__CORR__")

p.update()
p.dump_cfg()

