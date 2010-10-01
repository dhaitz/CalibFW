import ZPJProcessDataBase

p = ZPJProcessDataBase.getMCBaseConfig()

_data_path = "/scratch/hh/lustre/cms/user/piparo/ZPJ_2010/Zplusjet_mc/"
_out_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/" 

p.general.tchain_file_path = _data_path + "mc_job*.root" 
p.general.output_path = _out_path + "zplusjet_mc"
p.general.process_algos = ["ak5PF"]

p.update()
p.dump_cfg()
