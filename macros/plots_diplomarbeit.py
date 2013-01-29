import plotbase
import copy

plots = [
    'main']


def main(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/basic"

    plotbase.plotdatamc.datamcplot('npv', files, local_opt)
    plotbase.plotdatamc.datamcplot('jet1pt', files, local_opt)
    plotbase.plotdatamc.datamcplot('jet2pt', files, local_opt)
    plotbase.plotdatamc.datamcplot('jet1eta', files, local_opt)
    plotbase.plotdatamc.datamcplot('jet1phi', files, local_opt)
    plotbase.plotdatamc.datamcplot('zmass', files, local_opt)
    plotbase.plotdatamc.datamcplot('zpt', files, local_opt)
    plotbase.plotdatamc.datamcplot('METphi', files, local_opt)

    # PU rew
    local_opt.out = "out/diplomarbeit/basic/PU-unrew"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_noPUrew/out/closure.root"]
    plotbase.plotdatamc.datamcplot('npv', files, local_opt)

    #MET phi
    local_opt.out = "out/diplomarbeit/basic/METphi-uncorr"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534_METphi/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_METphi/out/closure.root"]

    plotbase.plotdatamc.datamcplot('METphi', files, local_opt)


    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/fractions"
    plotbase.plotfractions.fractions(files, local_opt, over='zpt')
    plotbase.plotfractions.fractions(files, local_opt, over='jet1eta')

