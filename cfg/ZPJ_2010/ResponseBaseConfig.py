import ZPJConfiguration

# -*- coding: utf-8 -*-
# Here the parameters for all the plots are stated

#-----------------------------------
def multiline_text(line1,line2,line3="",line4=""):
    string = "#scale[.8]{#splitline{#splitline{%s}{%s}}{#splitline{%s}{%s}}}" %(line1,line2,line3,line4)
    return string
#-----------------------------------


def GetResponseBaseConfig():
    process = ZPJConfiguration.configuration("Plots_AN_PAS")
    p=process

    #the_input_file="/local/scratch/hauth/data/ZPJ2010/zplusjet_mc_custom_binning.root"
    the_mc_input_file="/local/scratch/hauth/data/ZPJ2010/zplusjet_mc_fall10_weighted_cb.root"
    the_data_input_file="/local/scratch/hauth/data/ZPJ2010/mu_data_2010a+b.root"

    the_lumi=12.31;
    the_pt_bins=[0,30,60,100,140,220]

    # modify how to bin data here
    the_source=[]
    the_algos=[]
    the_good_algos=["Binned MC", "Binned Data"]

    the_info_string= multiline_text("p_{T}^{#mu}>15 GeV , |#eta^{#mu}|<2.3 , |M-M_{Z}|<20 GeV",
                                    "|#eta^{jet}|<1.3 , |#Delta #phi (Z,jet) - #pi|<0.2 , p_{T}^{jet2}/p_{T}^{Z}< 0.2",
                                    "Particle Flow jets",
                                    "__CORR__")

    the_img_formats=["png", "pdf"]

    p.general=ZPJConfiguration.section ("general",
        correction_level = 2,
        cut_end_entries = 0,
        plot_file_postfix = "_data",
        source_type=["mc","data"],
        source_files=[the_mc_input_file, the_data_input_file ],
        source_number=[0, 1],
        lumi=the_lumi,
        algos=the_algos,
        pt_bins=the_pt_bins,
        good_algos=the_good_algos,
        info_string=the_info_string,
        img_formats=the_img_formats )

    return p

