import ZPJConfiguration
import ZPJLocalConfig
import ZPJBase

# -*- coding: utf-8 -*-
# Here the parameters for all the plots are stated

#-----------------------------------
def multiline_text(line1,line2,line3="",line4=""):
    string = "#scale[.8]{#splitline{#splitline{%s}{%s}}{#splitline{%s}{%s}}}" %(line1,line2,line3,line4)
    return string
#-----------------------------------


process = DataMC_Comparison_Base.GetDataMCBaseConfig()

process.general.algos=["kt6PF"]
process.general.good_algos=["kt 0.6"]

process.general.info_string= multiline_text(ZPJBase.GetCutsText1(),
				      ZPJBase.GetCutsText2(),
                                "__ALGO__ Particle Flow jets",
                                 "__CORR__")
process.update()
process.dump_cfg()

