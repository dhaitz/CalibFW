import ZPJConfiguration
import ZPJLocalConfig
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
p.general.source_type=["data"]
p.general.source_number=[0]

p.general.source_files=[ZPJLocalConfig.GetLocalRootFilePath() + "mu_data_2010a+b_l3.root"]

p.general.correction_level = 3
p.general.algos=["ak5PF"]
p.general.good_algos=["anti-kt 0.5"]
p.general.global_prefix = "JetResponse_PF_data_"
p.general.info_string= multiline_text("p_{T}^{#mu}>15 GeV , |#eta^{#mu}|<2.3 , |M-M_{Z}|<20 GeV",
                                "|#eta^{jet}|<1.3 , |#Delta #phi (Z,jet) - #pi|<0.2 , p_{T}^{jet2}/p_{T}^{Z}< 0.2",
                                "Data Particle Flow jets",
                                "__CORR__")

p.update()
p.dump_cfg()

