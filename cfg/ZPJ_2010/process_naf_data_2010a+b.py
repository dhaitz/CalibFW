import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

_data_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/"

p.general.tchain_file_path = _data_path + "skim_data_2010a+b/data_*.root" 
p.general.output_path = _data_path + "mu_data_2010a+b"
p.general.write_events = "incut"
#p.plot_cuteff = 1
#p.general.process_algos = ["ak5PF"]

p.general.custom_binning = [0,30,60,100,140,220]

p.update()
p.dump_cfg()
