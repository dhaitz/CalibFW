import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

_data_path = "/local/scratch/hauth/data/ZPJ2010/skim_data/"

p.general.tchain_file_path = _data_path + "data*_job_Mu_43*.root" 
p.general.output_path = "/local/scratch/hauth/data/ZPJ2010/mu_data"
p.general.write_events = "incut"
p.general.process_algos = ["ak5PF"]

p.general.custom_binning = [0,50,120,220]
p.general.use_hlt = 1
p.general.plot_cuteff = 1
p.update()
p.dump_cfg()
