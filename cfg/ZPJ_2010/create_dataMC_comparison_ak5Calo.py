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



import DataMC_Comparison_Base

process = DataMC_Comparison_Base.GetDataMCBaseConfig()

process.general.algos=["ak5Calo"]
process.general.good_algos=["anti-kt 0.5"]

process.general.info_string= multiline_text(ZPJBase.GetCutsText1(),
				      ZPJBase.GetCutsText2(),
                                "Calorimeter __ALGO__ jets",
                                 "__CORR__")

process.update()
process.dump_cfg()

