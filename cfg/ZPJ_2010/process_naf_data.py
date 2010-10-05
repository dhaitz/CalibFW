import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

_data_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/"

p.general.tchain_file_path = _data_path + "skim_data/data_job*.root" 
p.general.output_path = _data_path + "mu_data"
p.general.write_events = "incut"
p.general.process_algos = ["ak5PF"]

p.general.custom_binning = [0,50,120,220]

p.update()
p.dump_cfg()
