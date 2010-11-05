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
p.general.algos=["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5PF", "iterativeCone5Calo"]
p.general.good_algos=["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5PF", "iterativeCone5Calo"]
p.general.source_type=["data","data","data","data","data","data","data","data","data","data","data","data","data","data","data"]
p.general.source_number=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
p.general.global_prefix = "Response_2010AB_5bins_"
p.general.source_files="/scratch/hh/lustre/cms/user/berger/skim/skim2010AB_5bins.root"
p.general.info_string= multiline_text("p_{T}^{#mu}>15 GeV , |#eta^{#mu}|<2.3 , |M-M_{Z}|<20 GeV",
                                "|#eta^{jet}|<1.3 , |#Delta #phi (Z,jet) - #pi|<0.2 , p_{T}^{jet2}/p_{T}^{Z}< 0.2",
                                "kt 0.4 Particle Flow jets",
                                "__CORR__")
p.general.custom_binning = [0, 35, 60, 100, 170, 250]

p.update()
p.dump_cfg()

