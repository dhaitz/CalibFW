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
p.general.source=["mc","data"]
p.general.algos=["ak5Calo","ak5Calo"]#["ak5PF", "ak7PF", "kt4PF", "kt6PF", "iterativeCone5PF"]
p.general.good_algos=["Binned MC", "Binned Data"]#["anti-kt 0.5 PF", "anti-kt 0.7 PF", "kt 0.4 PF", "kt 0.6 PF", "iterativeCone5 PF"]
p.general.global_prefix = "JetResponse_ak5Calo_MC_data_compare_"
p.general.info_string= multiline_text("p_{T}^{#mu}>15 GeV , |#eta^{#mu}|<2.3 , |M-M_{Z}|<20 GeV",
                                "|#eta^{jet}|<1.3 , |#Delta #phi (Z,jet) - #pi|<0.2 , p_{T}^{jet2}/p_{T}^{Z}< 0.2",
                                "anti-kt 0.5 Calo jets",
                                "__CORR__")

p.update()
p.dump_cfg()

