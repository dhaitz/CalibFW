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
p.general.algos=["ak7Calo","ak7Calo"]
p.general.good_algos=["Binned MC", "Binned Data"]
p.general.global_prefix = "JetResponse_ak7Calo_MC_data_compare_"
p.general.info_string= multiline_text(ZPJBase.GetCutsText1(),
				      ZPJBase.GetCutsText2(),
				      "anti-kt 0.7 Calorimeter jets",
                                "__CORR__")

p.update()
p.dump_cfg()

