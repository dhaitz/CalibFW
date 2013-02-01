import plotbase
import copy

plots = [
    'dipl']


def dipl(files, opt):

    dbasic(files, opt)
    dfrac(files, opt)
    dresp(files, opt)
    dak7(files, opt)
    dl1(files, opt)
    dex(files, opt)


def dbasic(files, opt):
    #basic plots
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/basic"

    plotbase.plotdatamc.datamcplot('npv', files, local_opt, changes = {'incut':'allevents'}, rebin=1)
    plotbase.plotdatamc.datamcplot('jet1pt', files, local_opt, x_limits = [0, 400])
    plotbase.plotdatamc.datamcplot('jet2pt', files, local_opt, rebin =2)
    plotbase.plotdatamc.datamcplot('jet1eta', files, local_opt, rebin=2, x_limits=[-2, 2], legloc='lower center')
    plotbase.plotdatamc.datamcplot('jet1phi', files, local_opt, x_limits=[-3.5, 3.5], legloc='lower center')
    plotbase.plotdatamc.datamcplot('zmass', files, local_opt, rebin=2)
    plotbase.plotdatamc.datamcplot('zpt', files, local_opt)
    plotbase.plotdatamc.datamcplot('zeta', files, local_opt, x_limits=[-3, 3], legloc='lower center', rebin=2)
    plotbase.plotdatamc.datamcplot('zY', files, local_opt, rebin=2, x_limits=[-3, 3], legloc='lower center')
    plotbase.plotdatamc.datamcplot('METphi', files, local_opt)

    plotbase.plotdatamc.datamcplot('balresp', files, local_opt, fit='gauss')
    plotbase.plotdatamc.datamcplot('mpfresp', files, local_opt, fit='gauss')

    # PU rew
    local_opt.out = "out/diplomarbeit/basic/PU-unrew"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_noPUrew/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]
    plotbase.plotdatamc.datamcplot('npv', files, local_opt, changes = {'incut':'allevents'}, rebin=1)

    #MET phi
    local_opt.out = "out/diplomarbeit/basic/METphi-uncorr"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534_METphi/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_METphi/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]

    plotbase.plotdatamc.datamcplot('METphi', files, local_opt)

def dfrac(files, opt):
    #fractions
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/fractions"
    plotbase.plotfractions.fractions(files, local_opt, over='zpt')
    plotbase.plotfractions.fractions(files, local_opt, over='jet1eta')


    #extrapolation
def dex(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/extrapolation"
    local_opt.cut = [0.35]
    plotbase.plotresponse.extrapol(files, local_opt)

    local_opt.correction = "L1L2L3"
    plotbase.plotresponse.extrapol(files, local_opt)

def dresp(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/response"
    plotbase.plotresponse.mpf_responseratio_zpt(files, local_opt)
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt)

    # powheg
    local_opt.out = "out/diplomarbeit/response/powheg"

    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_powhegSummer12_534/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]

    plotbase.plotresponse.mpf_responseratio_zpt(files, local_opt)
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt)


def dak7(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/AK7"
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt)

    local_opt.algorithm = "AK7PFJetsCHS"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534_AK7/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_AK7/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt)

def dl1(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/L1Fastjet"
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt)
    plotbase.plotdatamc.L1('L1abs_npv', files, local_opt, rebin=1)

    local_opt.out = "out/diplomarbeit/L1Offset"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534_L1Offset/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_L1Offset/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt)
    plotbase.plotdatamc.L1('L1abs_npv', files, local_opt, rebin=1)






