import ZPJProcessDataBase

p = ZPJProcessDataBase.getMCBaseConfig()


p.general.tchain_file_path = "/scratch/hh/lustre/cms/user/hauth/ZPJ2010/skim_zjet_fall10/mc_job*out.root"
p.general.output_path = "/scratch/hh/lustre/cms/user/berger/skim/skimFall10py"
p.general.write_events = "incut"

#p.general.custom_binning = [0,50,120,220]

p.update()
p.dump_cfg()
