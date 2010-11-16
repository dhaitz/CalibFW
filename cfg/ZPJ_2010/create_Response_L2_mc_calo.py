import ZPJConfiguration
import ResponseBaseConfig
import ZPJBase

# -*- coding: utf-8 -*-
# Here the parameters for all the plots are stated

#-----------------------------------
def multiline_text(line1,line2,line3="",line4=""):
    string = "#scale[.8]{#splitline{#splitline{%s}{%s}}{#splitline{%s}{%s}}}" %(line1,line2,line3,line4)
    return string
#-----------------------------------


p = ResponseBaseConfig.GetResponseBaseConfig() 

# modify how to bin data here
p.general.source_type=["mc","mc","mc","mc","mc"]
p.general.source_number=[0,0,0,0,0]

p.general.algos=["ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5Calo"]
p.general.good_algos=["anti-kt 0.5", "anti-kt 0.7", "kt 0.4", "kt 0.6", "iterativeCone5"]
p.general.global_prefix = "JetResponse_Calo_mc_"
p.general.info_string= multiline_text(ZPJBase.GetCutsText1(),
				      ZPJBase.GetCutsText2(),
				      "MC Calorimeter jets",
                                "__CORR__")

p.update()
p.dump_cfg()

