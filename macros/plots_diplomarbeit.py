import plotbase
import copy

plots = [
    'dipl']


def dipl(files, opt):

    dbasic(files, opt)
    dfrac(files, opt)
    dresp(files, opt)
    dresprun(files, opt)
    dresol(files, opt)
    dpowheg(files, opt)
    dak7(files, opt)
    dl1(files, opt)
    dex(files, opt)
    dintro(files, opt)


def dbasic(files, opt):
    #basic plots
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/basic"

    plotbase.plotdatamc.datamcplot('npv', files, local_opt, changes = {'incut':'allevents'}, rebin=1)
    plotbase.plotdatamc.datamcplot('jet1pt', files, local_opt, changes={'x_limits':[0, 400]})
    plotbase.plotdatamc.datamcplot('jet2pt', files, local_opt, rebin=1, changes={'x_limits':[0, 50]})
    plotbase.plotdatamc.datamcplot('jet1eta', files, local_opt, rebin=2, changes={'x_limits':[-2, 2]}, legloc='lower center')
    plotbase.plotdatamc.datamcplot('jet1phi', files, local_opt, changes={'x_limits':[-3.5, 3.5]}, legloc='lower center')
    plotbase.plotdatamc.datamcplot('zmass', files, local_opt, rebin=2)
    plotbase.plotdatamc.datamcplot('zpt', files, local_opt, changes={'x_limits':[0, 300]})
    plotbase.plotdatamc.datamcplot('zeta', files, local_opt, changes={'x_limits':[-3, 3]}, legloc='lower center', rebin=2)
    plotbase.plotdatamc.datamcplot('zY', files, local_opt, rebin=2, changes={'x_limits':[-2.8, 2.8]}, legloc='lower center')
    plotbase.plotdatamc.datamcplot('METphi', files, local_opt)

    plotbase.plotdatamc.datamcplot('balresp', files, local_opt, fit='gauss', changes={'x_limits':[0.2, 1.8]})
    plotbase.plotdatamc.datamcplot('mpfresp', files, local_opt, fit='gauss', changes={'x_limits':[0.2, 1.8]})

    plotbase.plot2d.twoD("2D_cut-all_npv_zpt", files, local_opt, changes={'x_limits':[0, 40], 'y_limits':[0, 400]})

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

    local_opt.y_limits = [0.96, 1.04, 0.95, 1.01]
    plotbase.plotresponse.mpf_responseratio_zpt(files, local_opt, extrapol='global')
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='global')

    #local_opt.correction = "L1L2L3Res"
    #plotbase.plotresponse.mpf_responseratio_eta(files, local_opt, extrapol='global')
    #plotbase.plotresponse.bal_responseratio_eta(files, local_opt, extrapol='global')

    # L1
    local_opt.out = "out/diplomarbeit/response/L1"
    local_opt.y_limits = [0.91, 1.01, 0.96, 1.01]
    local_opt.correction = ""
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt, extrapol='global')
    local_opt.correction = "L1"
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt, extrapol='global')
    # L2L3
    local_opt.out = "out/diplomarbeit/response/L2L3"

    limits_eta = [0.561, 1.269, 0.66, 1.02]

    local_opt.y_limits = [0.91, 1.05, 0.96, 1.01]
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='global')
    local_opt.y_limits = limits_eta
    plotbase.plotresponse.bal_responseratio_eta(files, local_opt, extrapol='bin')
    local_opt.correction = "L1L2L3"
    local_opt.y_limits = [0.91, 1.05, 0.96, 1.01]
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='global')
    local_opt.y_limits = limits_eta
    plotbase.plotresponse.bal_responseratio_eta(files, local_opt, extrapol='bin')

    local_opt.correction = "L1L2L3Res"
    plotbase.plotresponse.responseratio(files, local_opt, over='jet1eta', 
                                fit=False, types=['balresp'], extrapol='bin', residual_trick = True)



    # CHS
    local_opt.out = "out/diplomarbeit/response/chs"
    local_opt.y_limits = [0.90, 1.05, 0.95, 1.01]

    for corr in ['', 'L1L2L3']:
        for algo in ['AK5PFJets', 'AK5PFJetsCHS']:
            local_opt.correction = corr
            local_opt.algorithm = algo
            plotbase.plotresponse.bal_responseratio_npv(files, local_opt, extrapol='global')

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

    #local_opt.out = "out/diplomarbeit/response/madgraph"
    #plotbase.plotresponse.mpf_responseratio_zpt(files, local_opt, extrapol='global')
    #plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='global')
    
    local_opt.out = "out/diplomarbeit/response/powheg"
    local_opt.files += ["/storage/8/dhaitz/CalibFW/work/mc_powhegSummer12_534/out/closure.root"]
    local_opt.labels = ['data', 'madgraph', 'powheg']
    local_opt.y_limits = [0.951, 1.039, 0.94, 1.04]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]

    plotbase.plotresponse.mpf_responseratio_zpt(files, local_opt, extrapol='global')
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='global')


def dresprun(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/response_run"
    local_opt.y_limits = [0.801, 1.05]
    for corr in ["L1L2L3", "L1L2L3Res"]:
        local_opt.correction = corr
        plotbase.plotresponse.response_run(files, local_opt)
        for eta in plotbase.getroot.etastrings(local_opt.eta):
            plotbase.plotresponse.response_run(files, local_opt, changes = {'var':eta})

def dak7(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.y_limits = [0.961, 1.099, 0.921, 1.019]

    local_opt.out = "out/diplomarbeit/AK7"
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='global')

    local_opt.algorithm = "AK7PFJetsCHS"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534_AK7/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_AK7/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]
    plotbase.plotresponse.bal_responseratio_zpt(files, local_opt, extrapol='global')

def dl1(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.y_limits = [0.941, 1.029, 0.946, 1.009]
    local_opt.out = "out/diplomarbeit/L1Fastjet"
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt, extrapol='global')

    local_opt.y_limits = None
    plotbase.plotdatamc.L1('L1abs_npv', files, local_opt, rebin=1)

    local_opt.out = "out/diplomarbeit/L1Offset"
    local_opt.files = ["/storage/8/dhaitz/CalibFW/work/data_2012_534_L1Offset/out/closure.root",
            "/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_534_L1Offset/out/closure.root"]
    files = [plotbase.getroot.openfile(f, opt.verbose) for f in local_opt.files]
    plotbase.plotresponse.bal_responseratio_npv(files, local_opt, extrapol='global')
    plotbase.plotdatamc.L1('L1abs_npv', files, local_opt, rebin=1)



def dintro(files, opt):
    local_opt = copy.deepcopy(opt)
    local_opt.out = "out/diplomarbeit/intro"
    local_opt.labels = ['Messdaten','Simulation']
    local_opt.y_limits = [0.89, 1.04, 0.95, 1.04]

    local_opt.correction = ""
    #plotbase.plotresponse.responseratio(files, local_opt, over='zpt', 
    #                            fit=True, types=['balresp'], extrapol='global', german=True)
    local_opt.correction = "L1L2L3Res"
    #plotbase.plotresponse.responseratio(files, local_opt, over='zpt', 
    #                           fit=True, types=['balresp'], extrapol='global', german=True)
    local_opt.y_limits = None

    #plotbase.plotfractions.fractions_run_all(files, local_opt, response=True, diff=True, german=True, date=True)

    plotbase.plotresponse.extrapol(files, local_opt, german=True)







