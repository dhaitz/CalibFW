import ZPJConfiguration

process=ZPJConfiguration.configuration("my_cfg")
p=process

#-------------------------------------------------------------------------------

the_data_ifile_name="/home/piparo/Paper/extrapolation_data.root"
the_pythia_ifile_name="/home/piparo/Paper/extrapolation_dy_pu.root"
the_herwig_ifile_name=the_pythia_ifile_name
the_correction_level="L1"
the_algorithm="ak5"
the_do_herwig = False

#-------------------------------------------------------------------------------

p.make_plots_ak5_L1=ZPJConfiguration.section(\
    "make_plots_ak5_L1",
    data_ifile_name = the_data_ifile_name,
    pythia_ifile_name = the_pythia_ifile_name,
    herwig_ifile_name = the_herwig_ifile_name,
    do_herwig = the_do_herwig,
    correction_level = "L1",
    algorithm = "ak5")

#-------------------------------------------------------------------------------

p.make_plots_ak7_L1=ZPJConfiguration.section(\
    "make_plots_ak7_L1",
    data_ifile_name = the_data_ifile_name,
    pythia_ifile_name = the_pythia_ifile_name,
    herwig_ifile_name = the_herwig_ifile_name,
    do_herwig = the_do_herwig,
    correction_level = "L1",
    algorithm = "ak7")

#-------------------------------------------------------------------------------

p.make_plots_ak5_L1L2L3=ZPJConfiguration.section(\
    "make_plots_ak5_L1L2L3",
    data_ifile_name = the_data_ifile_name,
    pythia_ifile_name = the_pythia_ifile_name,
    herwig_ifile_name = the_herwig_ifile_name,
    do_herwig = the_do_herwig,
    correction_level = "L1L2L3",
    algorithm = "ak5")

#-------------------------------------------------------------------------------

p.make_plots_ak7_L1L2L3=ZPJConfiguration.section(\
    "make_plots_ak7_L1L2L3",
    data_ifile_name = the_data_ifile_name,
    pythia_ifile_name = the_pythia_ifile_name,
    herwig_ifile_name = the_herwig_ifile_name,
    do_herwig = the_do_herwig,
    correction_level = "L1L2L3",
    algorithm = "ak7")

#-------------------------------------------------------------------------------

p.update()
p.dump_cfg()     

#-------------------------------------------------------------------------------                    
                    