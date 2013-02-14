import plotbase
import copy

plots = [
    'dipl']


def dipl(files, opt):

    #dbasic(files, opt)
    #dfrac(files, opt)
    dresp(files, opt)
    dresprun(files, opt)
    #dresol(files, opt)
    dpowheg(files, opt)
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

    plotbase.plot2d.twoD("2D_cut-all_npv_zpt", files, local_opt, x_limits=[0, 40], y_limits=[0, 400])

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


    local_opt.out = "out/diplomarbeit/fractions_run"
    plotbase.plotfractions.fractions_diff_run_all(files, local_opt)
    plotbase.plotfractions.fractions_diff_run(files, local_opt)
    plotbase.plotfractions.fractions_run_response_diff_all(files, local_opt)
    plotbase.plotfractions.fractions_run_response_diff(files, local_opt)

    #extrapolation
def dex(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/extrapolation"
    local_opt.cut = [0.35]
    plotbase.plotresponse.extrapol(files, local_opt)

    local_opt.correction = "L1L2L3Res"
    plotbase.plotresponse.extrapol(files, local_opt)

    local_opt.out = "out/diplomarbeit/extrapolation/Summer12"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534_Summer12/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_Summer12/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]
    plotbase.plotresponse.extrapol(files, local_opt)




def dresp(files, opt):
    local_opt = copy.deepcopy(opt)

    local_opt.out = "out/diplomarbeit/response"
    plotbase.plotresponse.mpf_responseratio_zpt(files, local_opt, extrapol='globalfactor')
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='globalfactor')

    local_opt.correction = "L1L2L3Res"
    plotbase.plotresponse.mpf_responseratio_eta(files, local_opt, extrapol='globalfactor')
    plotbase.plotresponse.bal_responseratio_eta(files, local_opt, extrapol='globalfactor')

    # L1
    local_opt.out = "out/diplomarbeit/response/L1"
    local_opt.correction = ""
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt)
    local_opt.correction = "L1"
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt)
    # L2L3
    local_opt.out = "out/diplomarbeit/response/L2L3"
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt)
    plotbase.plotresponse.bal_responseratio_eta(files, local_opt)
    local_opt.correction = "L1L2L3"
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='globalfactor')
    plotbase.plotresponse.bal_responseratio_eta(files, local_opt, extrapol='globalfactor')



    # CHS
    local_opt.out = "out/diplomarbeit/response/chs"

    for corr in ['', 'L1L2L3']:
        for algo in ['AK5PFJets', 'AK5PFJetsCHS']:
            local_opt.correction = corr
            local_opt.algorithm = algo
            plotbase.plotresponse.bal_responseratio_npv(files, local_opt, extrapol=False)

    # resolution
def dresol(files, opt):
    local_opt = copy.deepcopy(opt)
    #local_opt.colors = ['black', 'blue']
    local_opt.out = "out/diplomarbeit/resolution"
    plotbase.plot_resolution_new.mpf_resolution_zpt(files, local_opt)
    plotbase.plot_resolution_new.bal_resolution_zpt(files, local_opt)

    # powheg
def dpowheg(files, opt):
    local_opt = copy.deepcopy(opt)

    local_opt.out = "out/diplomarbeit/response/madgraph"
    plotbase.plotresponse.mpf_responseratio_zpt(files, local_opt, extrapol='globalfactor')
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='globalfactor')

    local_opt.out = "out/diplomarbeit/response/powheg"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_powhegSummer12_534/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]

    plotbase.plotresponse.mpf_responseratio_zpt(files, local_opt, extrapol='globalfactor')
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='globalfactor')


def dresprun(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/response_run"
    plotbase.plotresponse.response_run(files, local_opt)
    for eta in plotbase.getroot.etastrings(local_opt.eta):
        plotbase.plotresponse.response_run(files, local_opt, changes = {'var':eta})

def dak7(files, opt):
    local_opt = copy.deepcopy(opt)

    local_opt.out = "out/diplomarbeit/AK7"
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='globalfactor')

    local_opt.algorithm = "AK7PFJetsCHS"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534_AK7/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_AK7/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='globalfactor')

def dl1(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/L1Fastjet"
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt, extrapol='globalfactor')
    plotbase.plotdatamc.L1('L1abs_npv', files, local_opt, rebin=1)

    local_opt.out = "out/diplomarbeit/L1Offset"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534_L1Offset/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_L1Offset/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt, extrapol='globalfactor')
    plotbase.plotdatamc.L1('L1abs_npv', files, local_opt, rebin=1)






