import ZPJProcessDataBase

p = ZPJProcessDataBase.getMCBaseConfig()

_data_path = "/local/scratch/hauth/data/skim_zjet_fall10/"
_out_path = "/local/scratch/hauth/data/ZPJ2010/" 

p.general.tchain_file_path = _data_path + "mc_job61*0to15out.root" 
p.general.output_path = _out_path + "local_zplusjet_mc_fall10"
p.general.process_algos = ["ak5PF"]
#p.general.custom_binning = [0,30,60,100,140,220]

p.update()
p.dump_cfg()
