import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

_data_path = "/local/scratch/hauth/data/ZPJ2010/"

p.general.tchain_file_path = _data_path + "mu_data_hlt.root" 
p.general.output_path = _data_path + "mu_data_hlt_out.root"
p.general.write_events = "incut"
p.general.process_algos = ["ak5PF"]

p.general.custom_binning = [0,50,120,220]
p.general.use_hlt = 1
p.update()
p.dump_cfg()
