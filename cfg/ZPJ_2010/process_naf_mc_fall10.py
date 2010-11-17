import ZPJProcessDataBase

p = ZPJProcessDataBase.getMCBaseConfig()

#_data_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/skim_zjet_fall10/"
_data_path="/scratch/hh/lustre/cms/user/hauth/ZPJ2010/archive/2010_11_16/skim_zjet_fall10/"
_out_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/" 

p.general.tchain_file_path = _data_path + "mc_job*.root" 
p.general.output_path = _out_path + "zplusjet_mc_fall10"
#p.general.process_algos = ["ak5PF"]

p.update()
p.dump_cfg()
