import ZPJProcessDataBase

p = ZPJProcessDataBase.getMCBaseConfig()

p.general.tchain_file_path = "/scratch/hh/lustre/cms/user/berger/skimFall10herwig/hw_Fall10_skim*.root"
p.general.output_path = "/scratch/hh/lustre/cms/user/berger/skimFall10hwWithGEN"
p.general.write_events = "incut"
#p.general.process_algos = ["ak5PF"]

#p.general.custom_binning = [0,50,120,220]

p.update()
p.dump_cfg()
