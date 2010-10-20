import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

_data_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/"

p.general.tchain_file_path = _data_path + "skim_data_2010a+b/data_*.root" 
p.general.output_path = _data_path + "kristov_mu_data_2010a+b"
p.general.write_events = "all"

p.update()
p.dump_cfg()