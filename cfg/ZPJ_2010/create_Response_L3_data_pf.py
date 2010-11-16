import ZPJConfiguration
import ZPJLocalConfig
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
p.general.source_type=["data"]
p.general.source_number=[0]

p.general.source_files=[ZPJLocalConfig.GetLocalRootFilePath() + "mu_data_2010a+b_l3.root"]

p.general.correction_level = 3
p.general.algos=["ak5PF"]
p.general.good_algos=["anti-kt 0.5"]
p.general.global_prefix = "JetResponse_PF_data_"
p.general.info_string= multiline_text(ZPJBase.GetCutsText1(),
				      ZPJBase.GetCutsText2(),
				      "Data Particle Flow jets",
                                "__CORR__")

p.update()
p.dump_cfg()

