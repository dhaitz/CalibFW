import plotbase
import copy
import plotdatamc
import getroot
import math
import plotresponse
import plotfractions
import plot2d
import plot_tagging
import fit
import os


def plots_2014_07_03(files, opt):
    """ Plots for JEC presentation 03.07. """

    #### 2D histograms
    for obj, x, nbins in zip(['muon', 'jet', 'electron'],
            [[-2.5, 2.5], [-5.3, 5.3]]*2,
            [400, 1000, 300]):

        changes = {
            'out': 'out/2014_07_03',
            'y': [-3.2, 3.2],
        }
        changes.update({
            'folder': obj + "_all",
            'nbins': nbins,
            'x':x,
            'filename': obj + '_phi_eta',
            'xynames': ['%s eta' % obj, 
                    '%s phi' % obj, obj + 's'],
        })

        if obj is 'electron':
            filenames = ["data_ee_noc", "mc_ee_corr_test"]
        else:
            filenames = ["data_noc", "mc_rundep_noc"]
        files = [getroot.openfile("%s/work/%s.root" % (plotbase.os.environ['EXCALIBUR_BASE'], f), opt.verbose) for f in filenames]
        plot2d.twoD("phi_eta", files, opt, changes = changes)

        if obj is not 'electron':
            changes.update({
                'year': 2011,
                'filename': obj + '_phi_eta_2011',
                'lumi': 5.1,
                'energy': 7,
            })
            filenames = ["data11_noc"]
            files = [getroot.openfile("%s/work/%s.root" % (plotbase.os.environ['EXCALIBUR_BASE'], f), opt.verbose) for f in filenames]
            plot2d.twoD("phi_eta", files, opt, changes = changes)


    ##### PU Jet ID
    filenames = ["dataPUJETID", "data"]
    files = [getroot.openfile("%s/work/%s.root" % (plotbase.os.environ['EXCALIBUR_BASE'], f), opt.verbose) for f in filenames]
    changes = {
        'normalize': False,
        'ratiosubplot': 'True',
        'ratiosubploty': [0.8, 1.2],
        'out': 'out/2014_07_03',
        'x': [30, 250],
        'title': 'data',
        'labels': ['PUJetID applied', 'default'],
    }
    plotdatamc.datamcplot('zpt', files, opt, changes=changes)

    for typ in ['mpf', 'ptbalance']:
        plotresponse.responseratio(files, opt, over='zpt', types=[typ], changes={
            'labels':  ['PUJetID applied', 'default'],
            'out': 'out/2014_07_03',
            'x': [30, 1000],
            'xlog': True,
            })


    ##### timedep
    filenames = ["data", "mc_rundep"]
    files = [getroot.openfile("%s/work/%s.root" % (plotbase.os.environ['EXCALIBUR_BASE'], f), opt.verbose) for f in filenames]
    changes = {
        'out': 'out/2014_07_03',
        'filename': "timedep",
    }
    timedep(files, opt, changes=changes)

    ###### MPF fix
    filenames = [
        "/storage/a/dhaitz/excalibur/artus/mc_rundep_2014-06-18_10-41/out.root",
        "/storage/a/dhaitz/excalibur/artus/mc_rundep_2014-06-06_14-26/out.root"
    ]
    files = [getroot.openfile(f) for f in filenames]
    plotresponse.responseratio(files, opt, over='zpt', types=['mpf'], changes={
        'labels': ['MCRD-fixed', 'MCRD'],
        'xlog': True,
        'filename': "mpf_zpt-fixed",
        'out': 'out/2014_07_03',
        'x': [30, 1000],
        'xticks': [30, 50, 70, 100, 200, 400, 1000],
    })

    # mpf slopes
    filenames = ["data", "mc_rundep"]
    files = [getroot.openfile("%s/work/%s.root" % (plotbase.os.environ['EXCALIBUR_BASE'], f), opt.verbose) for f in filenames]
    changes = {
        'filename': "mpfslopes-fixed",
        'labels': ['data', 'MCRD'],
        'out': 'out/2014_07_03',
        'allalpha': True,
        'selection': 'alpha<0.3',
    }
    mpfslopes(files, opt, changes)


    changes.update({
        'filename': "mpfslopes",
        'labels': ['data', 'MCRD'],
    })
    filenames = [
        '/storage/a/dhaitz/excalibur/artus/data_2014-04-10_21-21/out.root',
        '/storage/a/dhaitz/excalibur/artus/mc_rundep_2014-06-06_14-26/out.root'
    ]
    files = [getroot.openfile(f) for f in filenames]
    mpfslopes(files, opt, changes)

    # SYNC
    os.system("rsync ${EXCALIBUR_BASE}/out/2014_07_03 ekplx26:plots -r")



def timedep(files, opt):
    """ Plots for the time dependence, requested by Mikko 2014-06-25."""

    settings = plotbase.getsettings(opt, quantity="response_run")
    fig, ax = plotbase.newplot()
    factor = 2e4
    methods = ['mpf', 'ptbalance']
    labels = ['MPF', '$p_T$ balance']
    for q, c, l, m, in zip(methods, 
                settings['colors'], labels, settings['markers']):

        slopes, serrs, x = [], [], []
        for eta1, eta2 in zip(opt.eta[:-1], opt.eta[1:]):
            changes = {
                'alleta': True,
                'allalpha': True,
                'selection': 'alpha<0.3 && abs(jet1eta) > %s && abs(jet1eta) < %s' % (eta1, eta2),
                'fit': 'slope',
            }
            rootobject = getroot.histofromfile("%s_run" % q, files[0], settings, changes=changes)

            # get fit parameters
            slope, serr = fit.fitline2(rootobject)[2:4]
            slopes += [slope*factor]
            serrs += [serr*factor]
            changes['x'] = [0, 6]
            x += [getroot.histofromfile("abs(jet1eta)", files[0], settings, changes=changes).GetMean()]

        ax.errorbar(x, slopes, serrs, drawstyle='steps-mid', color=c,
                                            fmt='o', capsize=0, label=l)

    #formatting stuff
    settings['x'] = [0, 5]
    plotbase.setaxislimits(ax, settings)
    plotbase.labels(ax, opt, settings)
    plotbase.axislabels(ax, 'Leading jet $\eta$', 'Response vs run: linear fit slope  (muliplied with 20 000)', settings=settings)
    ax.set_ylim(-0.1, 0.05)
    ax.set_xlim(0, 5.25)
    ax.grid(True)
    ax.set_xticks([float("%1.2f" % eta) for eta in opt.eta])
    for label in ax.get_xticklabels():
        label.set_rotation(45) 
    ax.axhline(0.0, color='black', linestyle='--')

    settings['filename'] = quantity="response_run"
    plotbase.Save(fig, settings)

def npuplot(files, opt):
    """ Plots for the JEC paper that Mikko requested 24.4.: npv and rho in bins of npu."""

    settings = plotbase.getsettings(opt, quantity='npv')
    settings['x'] = [-0.5, 99.5]
    settings['nbins'] = 100
    
    tgraphs = []
    for f in files:
        if files.index(f) == 0: # flag bad runs in data
            runs = "run!=191411 && run!=198049 && run!=198050 && run!=198063 && run!=201727 && run!=203830 && run!=203832 && run!=203833 && run!=203834 && run!=203835 && run!=203987 && run!=203992 && run!=203994 && run!=204100 && run!=204101 && run!=208509"
        else:
            runs = 1
        npuhisto = getroot.histofromfile('nputruth', f, settings)
        for i in range(100):
            if npuhisto.GetBinContent(i) > 0:
                npu = i
        tgraph = ROOT.TGraphErrors()
        for n in range(npu):
            changes = {'selection': 'nputruth>%s && nputruth<%s && %s' % (n-0.5, n+0.5, runs)}
            npv = getroot.histofromfile('npv', f, settings, changes=changes).GetMean()
            npverr = getroot.histofromfile('npv', f, settings, changes=changes).GetMeanError()
            rho = getroot.histofromfile('rho', f, settings, changes=changes).GetMean()
            rhoerr = getroot.histofromfile('rho', f, settings, changes=changes).GetMeanError()
            tgraph.SetPoint(n, npv, rho)
            tgraph.SetPointError(n, npverr, rhoerr)
        tgraphs.append(tgraph)
    settings['root'] = settings['root'] or settings['filename']
    getroot.saveasroot(tgraphs, opt, settings)


def mpfslopes(files, opt):
    """ Plot the slope of a linear fit on MPF vs NPV, in Z pT bins."""
    quantity="mpf_npv"
    settings = plotbase.getsettings(opt, quantity=quantity)
    settings['special_binning'] = True

    fig, ax = plotbase.newplot()

    for f, c, l, m, in zip(files, settings['colors'], settings['labels'], 
            settings['markers']):
        slopes, serrs, x = [], [], []

        # iterate over Z pT bins
        for ptlow, pthigh in zip(opt.zbins[:-1], opt.zbins[1:]):
            changes = {'selection':'zpt>%s && zpt<%s' % (ptlow, pthigh)}
            rootobject = getroot.histofromfile(quantity, f, settings, changes=changes)

            # get fit parameters and mean Z pT; append to lists
            slope, serr = fit.fitline2(rootobject)[2:4]
            slopes += [slope]
            serrs += [serr]
            x += [getroot.histofromfile("zpt", f, settings, changes=changes).GetMean()]

        ax.errorbar(x, slopes, serrs, drawstyle='steps-mid', color=c,
                                            fmt='o', capsize=0, label=l)

    #formatting stuff
    settings['x'] = [30, 100]
    settings['y'] = [0.95, 1.05]
    plotbase.setaxislimits(ax, settings)
    plotbase.labels(ax, opt, settings)
    ax.set_xscale('log')
    settings['xticks'] = [30, 50, 70, 100, 200, 400 ,1000]
    plotbase.axislabels(ax, 'zpt', 'slope from fit on MPF vs NPV', settings=settings)
    ax.set_ylim(-0.003, 0.003)
    ax.grid(True)

    plotbase.Save(fig, settings)


def pileup(files, opt):

    for ptlow, pthigh in zip(opt.zbins[:-1], opt.zbins[1:]):
        plotresponse.responseratio(files, opt, over='npv', types=['mpf'], changes={
            'allalpha':True,
            'selection':'alpha<0.3 && zpt>%s && zpt<%s' % (ptlow, pthigh),
            'filename': "mpf_npv_%s-%s" % (ptlow, pthigh)
            }
        )

def emucomparison(files, opt):
    values = []
    valueerrs = []

    for filenames in [['data', 'mc'], ['data_ee', 'mc_ee']]:
        files = [getroot.openfile("%s/work/%s.root" % (plotbase.os.environ['EXCALIBUR_BASE'], f), opt.verbose) for f in filenames]

        for quantity in ['mpf', 'ptbalance']:
            settings = plotbase.getsettings(opt, None, None, quantity)
            settings['nbins'] = 40
            settings['correction'] = 'L1L2L3'
            if 'ee' in filenames[0]:
                if settings['selection']:
                    settings['selection'] = 'abs(epluseta<1.0) && abs(eminuseta)<1.0 && %s' % settings['selection']
                else:
                    settings['selection'] = 'abs(epluseta<1.0) && abs(eminuseta)<1.0'

            datamc = []
            rootobjects = []
            fitvalues = []
            for f in files:
                rootobjects += [getroot.histofromfile(quantity, f, settings)]
                p0, p0err, p1, p1err, p2, p2err, chi2, ndf, conf_intervals = fit.fitline2(rootobjects[-1],
                                     gauss=True, limits=[0, 2])
                fitvalues += [p1, p1err]

            ratio = fitvalues[0] / fitvalues[2]
            ratioerr = math.sqrt(fitvalues[1] ** 2 + fitvalues[3] ** 2)

            values.append(ratio)
            valueerrs.append(ratioerr)
    fig, ax = plotbase.newplot()

    ax.errorbar(range(4), values, valueerrs, drawstyle='steps-mid', color='black',
                                                    fmt='o', capsize=0,)

    ax.set_xticks([0, 1, 2, 3])
    ax.set_xticklabels(['Zmm\nMPF', 'Zmm\npT balance', 'Zee\nMPF', 'Zee\npT balance'])
    ax.set_xlim(-0.5, 3.5)
    ax.set_ylim(0.96, 1.001)
    ax.axhline(1.0, color='black', linestyle=':')

    ax.set_ylabel('Jet response Data/MC ratio', ha="right", x=1)

    plotbase.Save(fig, settings)


def electrons(files, opt):
    """ Standard set of plots for the dielectron analysis. """

    filenames = ['data_ee', 'mc_ee']
    files = [getroot.openfile("%s/work/%s.root" % (plotbase.os.environ['EXCALIBUR_BASE'], f), opt.verbose) for f in filenames]
    base_changes = {
            'out': 'out/ee2014',
            'folder': 'zcuts',          # no additional restrictions on jets
            'normalize': False,         # no normalizing to check if the lumi reweighting works
            'factor': 1.,            # on the fly lumi reweighting
            'efficiency': 1.,           # no trigger reweighting for electrons
            'ratiosubplot': True,
    }

    # zmass with fit
    changes = {
        'legloc': 'center right',
        'nbins': 50,
        'fit': 'gauss'
    }
    changes.update(base_changes)
    plotdatamc.datamcplot('zmass', files, opt, changes=changes)

    #electron quantities
    for charge in ['plus', 'minus']:
        changes = {
            'x': [0, 150],
            'nbins': 40,
        }
        changes.update(base_changes)
        plotdatamc.datamcplot('e%spt' % charge, files, opt, changes=changes)
        changes['x'] = [-2.5, 2.5]
        plotdatamc.datamcplot('e%seta' % charge, files, opt, changes=changes)
        changes['x'] = None
        plotdatamc.datamcplot('e%sphi' % charge, files, opt, changes=changes)

    changes['legloc'] = 'center right'

    changes['filename'] = 'zmass_barrel'
    changes['selection'] = 'abs(epluseta)<1.0 && abs(eminuseta)<1.0'
    changes['title'] = '|eta(e)| < 1.0'
    changes['fit'] = 'gauss'
    plotdatamc.datamcplot('zmass', files, opt, changes=changes)

    changes['filename'] = 'zmass_endcap'
    changes['selection'] = 'abs(epluseta)>1.0 && abs(eminuseta)>1.0'
    changes['title'] = '|eta(e)| > 1.0'
    changes['fit'] = 'gauss'
    plotdatamc.datamcplot('zmass', files, opt, changes=changes)

    #electron quantities
    for charge in ['plus', 'minus']:
        changes = {
            'x': [0, 150],
            'nbins': 40,
        }
        changes.update(base_changes)
        plotdatamc.datamcplot('e%spt' % charge, files, opt, changes=changes)
        changes['x'] = [-2.5, 2.5]
        plotdatamc.datamcplot('e%seta' % charge, files, opt, changes=changes)
        changes['x'] = None
        plotdatamc.datamcplot('e%sphi' % charge, files, opt, changes=changes)

    # Z pT in rapidity bins
    rapbins = ['abs(zy)<1', 'abs(zy)>1 && abs(zy)<2', 'abs(zy)>2 && abs(zy)<3']
    raplabels = ['|Y(Z)|<1', '1<|Y(Z)|<2', '2<|Y(Z)|<3']
    rapname = ['0zy1', '1zy2', '2zy3']
    for rbin, rlabel, rname in zip(rapbins, raplabels, rapname):
        changes = {
            'selection': rbin,
            'filename': 'zpt-%s' % rname,
             'x': [30, 750],
             'log': True,
             'title': rlabel,
             'nbins': 40,
        
        }
        changes.update(base_changes)
        plotdatamc.datamcplot('zpt', files, opt, changes=changes)

    #electron quantities
    for charge in ['plus', 'minus']:
        changes = {
            'x': [0, 150],
            'nbins': 40,
        }
        changes.update(base_changes)
        plotdatamc.datamcplot('e%spt' % charge, files, opt, changes=changes)
        changes['x'] = [-2.5, 2.5]
        plotdatamc.datamcplot('e%seta' % charge, files, opt, changes=changes)
        changes['x'] = None
        plotdatamc.datamcplot('e%sphi' % charge, files, opt, changes=changes)



    # npv
    changes = {
        'folder': 'all',
    }
    changes.update(base_changes)
    changes['folder'] = 'all'

    plotdatamc.datamcplot('npv', files, opt, changes=changes)
    changes['noweighting'] = True
    changes['factor'] = 3503.71 / 30459503 * 1000
    changes['filename'] = 'npv_noweights'

    plotdatamc.datamcplot('npv', files, opt, changes=changes)
    changes['noweighting'] = True
    changes['factor'] = 3503.71 / 30459503 * 1000
    changes['filename'] = 'npv_noweights'
    plotdatamc.datamcplot('npv', files, opt, changes=changes)

    # z pt and rapidity
    changes = {
        'nbins': 40,
    }
    changes.update(base_changes)
    plotdatamc.datamcplot('zy', files, opt, changes=changes)
    plotdatamc.datamcplot('zeta', files, opt, changes=changes)
    changes['x'] = [30, 750]
    changes['log'] = True
    plotdatamc.datamcplot('zpt', files, opt, changes=changes)


    #powheg comparison
    filenames = ['data_ee', 'mc_ee', 'mc_ee_powheg']
    files = [getroot.openfile("%s/work/%s.root" % (plotbase.os.environ['EXCALIBUR_BASE'], f), opt.verbose) for f in filenames]

    changes = {
        'log': True,
        'x': [30, 750],
        'nbins': 40,
        'filename': 'zpt_mad-pow',
        'labels': ['data', 'madgraph', 'powheg'],

    }
    changes.update(base_changes)
    plotdatamc.datamcplot('zpt', files, opt, changes=changes)

    changes = {
        'nbins': 40,
        'filename': 'zmass_mad-pow',
        'labels': ['data', 'madgraph', 'powheg'],
    }
    changes.update(base_changes)
    plotdatamc.datamcplot('zmass', files, opt, changes=changes)

    files = files[::2]
    filenames = filenames[::2]
    changes = {
        'log':True,
        'x': [30, 750],
        'nbins': 40,
        'filename': 'zpt_pow',
        'labels':['data', 'powheg'],
    }
    changes.update(base_changes)
    plotdatamc.datamcplot('zpt', files, opt, changes=changes)
    
    #backgrounds
    filenames = ['data_ee', 'mc_ee', 'background_ee']
    files = [getroot.openfile("%s/work/%s.root" % (plotbase.os.environ['EXCALIBUR_BASE'], f), opt.verbose) for f in filenames]
    changes = {
        'log': True,
        'x': [30, 750],
        'filename': 'zpt_backgrounds',
        'labels': ['data', 'MC', 'backgrounds'],
        'markers': ['o', 'f', 'f'],
        'stacked': True,
        'ratiosubplot': False,

    }
    changes.update(base_changes)
    changes['ratiosubplot'] = False
    plotdatamc.datamcplot('zpt', files, opt, changes=changes)

    changes.pop('x', None)
    changes['filename'] = 'zmass_backgrounds'
    changes['log'] = False
    changes['ratiosubplot'] = False
    plotdatamc.datamcplot('zmass', files, opt, changes=changes)


    # sync the plots
    import subprocess
    subprocess.call(['rsync out/ee2014 dhaitz@ekplx26:plots/ -u -r --progress'], shell=True)

    """
    merlin 2D_zmass_zpt --files $DATAEE $ARGS -x 0 50 --nbins 100 -y 80 100 -o $OUT


    merlin eemass -o $OUT --files $DATAEE $ARGS --nbins 100 -x 0 120  -C lightskyblue -m f --folder all
    merlin eemass -o $OUT --files $DATAEE $ARGS --nbins 100 -x 0 15 --filename eemass_low -C lightskyblue -m f --folder all
    merlin 2D_zpt_zy -o $OUT --files $DATAEE $ARGS -y 0 100 --nbins 100
    """

def an(files, opt):
    """ Plots for the 2014 Z->mumu JEC AN."""
    """

    #MET
    for quantity in ['METpt', 'METphi']:
        plotdatamc.datamcplot(quantity, files, opt, changes = {'title': 'CMS preliminary'})


    plotdatamc.datamcplot("npv", files, opt, changes = {'folder': 'all', 'title': 'CMS preliminary'})
    for n in ['1', '2']:
        for quantity in ['pt', 'eta', 'phi']:
            plotdatamc.datamcplot('mu%s%s' % (n, quantity), files, opt, changes = {'title': 'CMS preliminary'})
            if n is '2' and quantity is 'eta':
                plotdatamc.datamcplot('jet%s%s' % (n, quantity), files, opt, changes = {'nbins': 10, 'correction': 'L1L2L3', 'title': 'CMS preliminary'})
            else:
                plotdatamc.datamcplot('jet%s%s' % (n, quantity), files, opt, changes = {'correction': 'L1L2L3', 'title': 'CMS preliminary'})

    for quantity in ['zpt', 'zeta', 'zy', 'zphi', 'zmass']:
        plotdatamc.datamcplot(quantity, files, opt, changes = {'title': 'CMS preliminary'})

    #response stuff
    plotresponse.responseratio(files, opt, over='zpt', types=['mpf'],
                     changes={'y': [0.98, 1.03, 0.96, 1.03], 'x': [0, 400, 0, 400]})
    plotresponse.responseratio(files, opt, over='jet1abseta', types=['mpf'],
                     changes={'y': [0.95, 1.1, 0.93, 1.1]})
    plotresponse.responseratio(files, opt, over='npv', types=['mpf'],
                     changes={'y': [0.95, 1.05, 0.92, 1.03], 'x': [0, 35, 0, 35]})

    plotresponse.responseratio(files, opt, over='zpt', types=['ptbalance'],
                     changes={'y': [0.93, 1.01, 0.96, 1.03], 'x': [0, 400, 0, 400]})
    plotresponse.responseratio(files, opt, over='jet1abseta', types=['ptbalance'],
                     changes={'y': [0.91, 1.01, 0.93, 1.1]})
    plotresponse.responseratio(files, opt, over='npv', types=['ptbalance'],
                     changes={'y': [0.91, 1.01, 0.92, 1.03], 'x': [0, 35, 0, 35]})
    """

    for q in ['mpf', 'ptbalance']:
        plotdatamc.datamcplot(q, files, opt, changes={'title': 'CMS preliminary', 
                                                        'correction': 'L1L2L3',
                                                        'legloc': 'center right',
                                                        'nbins': 100,
                                                        'fit': 'gauss'})

    plotresponse.extrapol(files, opt, changes={'title': 'CMS preliminary',
                                        'save_individually': True,
                                        'correction': 'L1L2L3'})
    """
    plotfractions.fractions(files, opt, over='zpt', changes={'x': [0, 400], 'title': 'CMS preliminary'})
    plotfractions.fractions(files, opt, over='jet1abseta', changes = {'title': 'CMS preliminary'})
    plotfractions.fractions(files, opt, over='npv', changes = {'title': 'CMS preliminary'})

    for changes in [{'rebin':10, 'title':'|$\eta^{\mathrm{jet}}$|<1.3'},
                {'alleta':True, 'rebin':10,
                'selection':'jet1abseta>2.5 && jet1abseta<2.964',
                'title':'2.5<|$\eta^{\mathrm{jet}}$|<2.964'}]:
        if 'alleta' in changes:
            opt.out += '/ECOT'
            opt.user_options['out'] += '/ECOT'
            plotfractions.fractions_run(files, opt, diff=True, response=True, changes=changes, nbr=6)
            plotfractions.fractions_run(files, opt, diff=False, response=True, changes=changes, nbr=6)
            plotfractions.fractions_run(files, opt, diff=True, response=False, changes=changes, nbr=6)
            plotresponse.response_run(files, opt, changes=changes)
            opt.out = opt.out[:-5]
            opt.user_options['out'] = opt.user_options['out'][:-5]
        else:
            plotfractions.fractions_run(files, opt, diff=True, response=True, changes=changes)
            plotfractions.fractions_run(files, opt, diff=False, response=True, changes=changes)
            plotfractions.fractions_run(files, opt, diff=True, response=False, changes=changes)
            plotresponse.response_run(files, opt, changes=changes)
        changes['y'] = [0.84, 1.2]

    plot2d.twoD("qgtag_btag", files, opt, 
                changes = {'title': 'CMS Preliminary', 'nbins':50}
                )


    plot_tagging.tagging_response(files, opt)
    plot_tagging.tagging_response_corrected(files, opt)
    """

    ## MCONLY
    if len(files) > 1:
        files = files[1:]
    """

    # PF composition as function of mc flavour
    flavour_comp(files, opt, changes={'title': 'CMS Simulation','mconly':True})

    # response vs flavour
    for var in [True, False]:
        plotresponse.response_physflavour(files, opt,
            changes={'title': 'CMS Simulation','mconly':True},
            add_neutrinopt=var,
            restrict_neutrals=var,
            extrapolation=var)

    plotfractions.flavour_composition(files, opt, changes={'title': 'CMS Simulation','mconly':True})
    plotfractions.flavour_composition_eta(files, opt, changes={'title': 'CMS Simulation','mconly':True, 'selection': 'zpt>95 && zpt<110'})

    changes = {'cutlabel' : 'ptetaalpha',
                'labels'  : ['Pythia 6 Tune Z2*', 'Herwig++ Tune EE3C'],
                'y'       : [0.98, 1.05],
                'markers' : ['o', 'd'],
                'colors'  : ['red', 'blue'],
                'title'   : 'CMS Simulation',
                'mconly'  : True,
                'legloc'  : 'lower left',
                'filename': 'recogen_physflavour_pythia-herwig'}
    files += [getroot.openfile("/storage/a/dhaitz/excalibur/work/mc_herwig/out/closure.root")]
    plotdatamc.datamcplot("recogen_physflavour", files, opt, changes=changes)
    """


def paper(files, opt):
    """ Plots for the 2012 JEC paper. """

    for method in ['mpf', 'ptbalance']:
        plot2d.twoD("abs((recogen-%s)/recogen)*abs((recogen-%s)/recogen)_npv_zpt" % (method, method),
                files[1:],
                opt,
                changes={'x': [0, 250],
                            'binroot': True,
                            'z': [0, 0.3],
                            'xynames': ["zpt", "npv", "RMS((recogen-%s)/recogen)" % method],
                            'filename': '2D_RMS_%s' % method,
                            #'labels': ["RMS((recogen-%s)/recogen)" % method],
                            'labels': ["CMS Simulation"],
                            #'cutlabel':'ptetaalpha03',
                            #'cutlabeloffset':-0.2,
                            'selection': 'alpha<0.3',
                            'allalpha': True,
                            'mconly':True,
                }
        )
    local_opt = copy.deepcopy(opt)
    local_opt.out = opt.out + '/flavour_response/'
    for a in [0, 1]:
        for b in [0, 1]:
            for c in [0, 1]:
                plotresponse.response_physflavour(files[1:], local_opt,
                    changes={'title': 'CMS Simulation','mconly':True},
                    add_neutrinopt=a,
                    restrict_neutrals=b,
                    extrapolation=c)

    plotresponse.extrapol(files, opt, changes={'title': 'CMS preliminary',
                                        'save_individually': True})

    # alpha dependencies
    changes = {'allalpha': True,
                'y': [0, 400],
                'markers': ['o', 'd'],
                'colors': ['black', 'blue']}
    plotdatamc.datamcplot("zpt_alpha", files, opt, changes=changes)
    changes['labels'] = ['MC']
    changes['y'] = [0.98, 1.1]
    changes['mconly'] = [True]
    plotdatamc.datamcplot("recogen_alpha", files[1:], opt, changes=changes)

    # HERWIG
    changes = {'cutlabel' : 'ptetaalpha',
                'labels'  : ['Pythia 6 Tune Z2*', 'Herwig++ Tune EE3C'],
                'y'       : [0.98, 1.05],
                'markers' : ['o', 'd'],
                'colors'  : ['red', 'blue'],
                'title'   : 'CMS Simulation',
                'mconly'  : True,
                'legloc'  : 'lower left',
                'filename': 'recogen_physflavour_pythia-herwig'}
    files2 = files[1:] + [getroot.openfile("/storage/a/dhaitz/excalibur/work/mc_herwig/out/closure.root")]
    plotdatamc.datamcplot("recogen_physflavour", files2, opt, changes=changes)

    flavour_comp(files[1:], opt, changes={'title': 'CMS Simulation',
            'cutlabel': 'ptetaalpha',
            'cutlabeloffset': 0.07,
            'mconly'  : True,
            'legloc': '0.05,0.5'})


def eleven(files, opt):
    """ Summary of the plots for the response studies with 2011 rereco. """

    runrange = [160000, 183000]
    plotdatamc.datamcplot('npv', files, opt, changes={'rebin': 1})
    plotdatamc.datamcplot('zmass', files, opt, changes={'fit': 'vertical', 'legloc': 'center right'})
    plotresponse.extrapol(files, opt)

    plotresponse.responseratio(files, opt, over='zpt', types=['mpf'],
                     changes={'y': [0.98, 1.03, 0.96, 1.03], 'uncertaintyband': True, 'x': [0, 400, 0, 400]})
    plotresponse.responseratio(files, opt, over='jet1abseta', types=['mpf'],
                     changes={'y': [0.95, 1.1, 0.93, 1.1], 'uncertaintyband': True})
    plotresponse.responseratio(files, opt, over='npv', types=['mpf'],
                     changes={'y': [0.95, 1.05, 0.92, 1.03], 'uncertaintyband': True, 'x': [0, 18, 0, 18]})

    plotresponse.responseratio(files, opt, over='zpt', types=['ptbalance'],
                     changes={'y': [0.93, 1.01, 0.96, 1.03], 'x': [0, 400, 0, 400], 'uncertaintyband': True})
    plotresponse.responseratio(files, opt, over='jet1abseta', types=['ptbalance'],
                     changes={'y': [0.91, 1.01, 0.93, 1.1], 'uncertaintyband': True})
    plotresponse.responseratio(files, opt, over='npv', types=['ptbalance'],
                     changes={'y': [0.91, 1.01, 0.92, 1.03], 'x': [0, 18, 0, 18], 'uncertaintyband': True})

    plotdatamc.datamcplot('npv_run', files, opt, changes={'x': runrange,
                'y': [0, 15], 'run': True, 'fit': True})

    plotfractions.fractions(files, opt, over='zpt', changes={'x': [0, 400]})
    plotfractions.fractions(files, opt, over='jet1abseta')
    plotfractions.fractions(files, opt, over='npv', changes={'x': [-0.5, 24.5]})

    for changes in [{'x': runrange, 'rebin':10, 'title':'|$\eta^{\mathrm{jet}}$|<1.3'},
                {'x': runrange, 'alleta':True, 'rebin':10,
                'selection':'jet1abseta>2.5 && jet1abseta<2.964',
                'title':'2.5<|$\eta^{\mathrm{jet}}$|<2.964'}]:

        if 'alleta' in changes:
            opt.out += '/ECOT'
            opt.user_options['out'] += '/ECOT'
            plotfractions.fractions_run(files, opt, diff=True, response=True, changes=changes, nbr=6)
            plotfractions.fractions_run(files, opt, diff=False, response=True, changes=changes, nbr=6)
            plotfractions.fractions_run(files, opt, diff=True, response=False, changes=changes, nbr=6)
        else:
            plotfractions.fractions_run(files, opt, diff=True, response=True, changes=changes)
            plotfractions.fractions_run(files, opt, diff=False, response=True, changes=changes)
            plotfractions.fractions_run(files, opt, diff=True, response=False, changes=changes)
        changes['y'] = [0.84, 1.2]
        plotresponse.response_run(files, opt, changes=changes)


def rootfile(files, opt):
    """Function for the rootfile sent to the JEC group in early August 2013."""

    list_of_quantities = ['ptbalance_alpha', 'mpf_alpha',
        'ptbalance', 'mpf', 'zpt', 'npv', 'zmass', 'zpt_alpha', 'npv_alpha',
    'ptbalance_zpt', 'mpf_zpt',
    'ptbalance_npv', 'mpf_npv',
    ]

    for muon in [["zmumu", "1"], ["zmumu_muoncuts",
            "(mupluspt>25 && muminuspt>25 && abs(mupluseta)<1.0 && abs(muminuseta)<1.0)"]]:
        for alpha in [[0, "alpha<0.2", "alpha0_2"], [1, "alpha<0.3", "alpha0_3"],
                                                [1, "alpha<0.4", "alpha0_4"]]:
            for quantity in list_of_quantities:

                changes = {'rebin': 1,
                        'out': 'out/root/',
                        'allalpha': True,
                        'root': "__".join([quantity, alpha[2]]),
                        'filename': muon[0],
                        'selection': "&&".join([alpha[1], muon[1]]),
                }

                if ("_zpt" in quantity) or ("_npv" in quantity):
                    changes['special_binning'] = True

                if "alpha" in quantity:
                    changes['rebin'] = 10

                plotdatamc.datamcplot(quantity, files, opt, changes=changes)

                changes['ratio'] = True
                changes['labels'] = ['ratio']
                plotdatamc.datamcplot(quantity, files, opt, changes=changes)


def flavour_comp(files, opt, changes=None):
    """Plot the PF composition as a function of the MC truth flavour."""

    quantity = "components_physflavour"
    settings = plotbase.getsettings(opt, changes, settings=None,
                                            quantity=quantity)
    nbr = 5
    labels = ["NHad", r"$\gamma$       ", "CHad", r"$e$       ",
                                        r"$\mu$       ", "HFem", "HFhad"][:nbr]
    labels.reverse()
    colours = ['YellowGreen', 'LightSkyBlue', 'Orange', 'MediumBlue',
                  'Darkred', 'yellow', 'grey'][:nbr]
    colours.reverse()
    components = ["neutralhad", "photon", "chargedhad", "chargedem", "muon",
                                                         "HFem", "HFhad"][:nbr]
    components.reverse()
    names = ["jet1" + component + "fraction" for component in components]

    stacked = []
    for i in range(len(names)):
        stacked += ["(%s)" % "+".join(names[i:])]
    fig, ax = plotbase.newplot()

    changes = {'subplot': True,
                'rebin': 4,
                'xynames': ['physflavour', 'components'],
                'markers': ['f']}

    for n, l, c in zip(stacked, labels, colours):
        changes['labels'] = [l]
        changes['colors'] = [c]

        plotdatamc.datamcplot("%s_physflavour" % n, files, opt,
                    fig_axes=(fig, ax), changes=changes, settings=settings)

    settings['filename'] = plotbase.getdefaultfilename(quantity, opt, settings)
    plotbase.Save(fig, settings)


def ineff(files, opt):
    settings = plotbase.getsettings(opt, changes=None, settings=None, quantity="flavour_zpt")

    fig, ax = plotbase.newplot()
    labels = ["no matching partons", "two matching partons"]
    colors = ['red', 'blue']
    markers = ['o', 'd']
    changes = {'subplot': True,
                'lumi': 0,
                'xynames': ['zpt', 'physflavourfrac'],
                'legloc': 'upper left',
            }

    for n, l, c, m in zip([0, 2], labels, colors, markers):
        quantity = "(nmatchingpartons3==%s)_zpt" % n
        changes['labels'] = [l]
        changes['colors'] = c
        changes['markers'] = m

        plotdatamc.datamcplot(quantity, files, opt, fig_axes=(fig, ax), changes=changes, settings=settings)
    settings['filename'] = plotbase.getdefaultfilename("physflavourfrac_zpt", opt, settings)
    plotbase.Save(fig, settings['filename'], opt)


def flav(files, opt):

    etabins = [0, 1.3, 2.5, 3, 3.2, 5.2]
    etastrings = ['0-1_3', '1_3-2_5', '2_5-3', '3-3_2', '3_2-5_2']
    flavourdefs = ["algoflavour", "physflavour"]
    flavourdefinitions = ["algorithmic", "physics"]

    flist = ["(flavour>0&&flavour<4)", "(flavour==1)", "(flavour==2)", "(flavour==3)",
        "(flavour==4)", "(flavour==5)", "(flavour==21)", "(flavour==0)"]
    q_names = ['uds', 'u', 'd', 's', 'c', 'b', 'gluon', 'unmatched']
    changes = {}

    ############### FLAVOUR NOT 0!!!!!

    # barrel:
    """changes['rebin'] = 1
    changes['filename']="flavour"
    changes['filename']="flavour"
    for f_id, quantity in zip(['uds','c','b','gluon'], flist):
            changes['root']=f_id
            plotdatamc.datamcplot("%s_zpt" % quantity, files, opt, changes=changes)
    """
    for flavourdef, flavourdefinition in zip(flavourdefs, flavourdefinitions):
    # iterate over eta bins:
        for filename, selection in zip(etastrings, getroot.etacuts(etabins)):
            changes['filename'] = "_".join([filename, flavourdefinition])
            changes['alleta'] = True
            changes['selection'] = "%s && %s" % (selection,
                                                                    "alpha<0.2")
            changes['rebin'] = 1

            for f_id, quantity in zip(q_names, flist):

                changes['root'] = f_id

                plotdatamc.datamcplot("%s_zpt" % quantity.replace("flavour",
                                    flavourdef), files, opt, changes=changes)


def gif(files, opt):
    local_opt = copy.deepcopy(opt)
    runlist = listofruns.runlist[::10]
    for run, number in zip(runlist, range(len(runlist))):
        local_opt.lumi = (run - 190456) * 19500 / (209465 - 190456)
        print
        plotbase.plotdatamc.datamcplot('balresp', files, local_opt,
             changes={'var': 'var_RunRange_0to%s' % run}, filename="%03d" % number)


def closure(files, opt):
    def divide((a, a_err), (b, b_err)):
        if (b != 0.0):
            R = a / b
        else:
            R = 0
        Rerr = R * math.sqrt((a_err / a) ** 2 + (b_err / b) ** 2)
        return R, Rerr

    def multiply((a, a_err), (b, b_err)):
        R = a * b
        Rerr = R * math.sqrt((a_err / a) ** 2 + (b_err / b) ** 2)
        return R, Rerr
    changes = {}
    changes = plotbase.getchanges(opt, changes)

    #get extrapol factors with alpha 035
    #changes['var']='var_CutSecondLeadingToZPt_0_4'
    #changes['correction']='L1L2L3'
    balresp = (getroot.getobjectfromnick('balresp', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('balresp', files[0], changes, rebin=1).GetMeanError())
    mpfresp = (getroot.getobjectfromnick('mpfresp', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('mpfresp', files[0], changes, rebin=1).GetMeanError())
    genbal = (getroot.getobjectfromnick('genbal', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('genbal', files[0], changes, rebin=1).GetMeanError())

    intercept, ierr, slope, serr, chi2, ndf, conf_intervals = getroot.fitline2(getroot.getobjectfromnick('ptbalance_alpha', files[0], changes, rebin=1))
    balresp_extrapol = (intercept, conf_intervals[0])
    extrapol_reco_factor = divide(balresp_extrapol, balresp)

    intercept2, ierr2, slope2, serr2, chi22, ndf2, conf_intervals2 = getroot.fitline2(getroot.getobjectfromnick('genbalance_genalpha', files[0], changes, rebin=1))
    genbal_extrapol = (intercept2, conf_intervals2[0])
    extrapol_gen_factor = divide(genbal_extrapol, genbal)

    intercept3, ierr3, slope3, serr3, chi23, ndf3, conf_intervals3 = getroot.fitline2(getroot.getobjectfromnick('mpf_alpha', files[0], changes, rebin=1))
    mpf_extrapol = (intercept3, conf_intervals3[0])
    extrapol_mpf_factor = divide(mpf_extrapol, mpfresp)

    #del changes['var']
    #del changes['correction']
    #other quantities with alpha 02
    recogen = (getroot.getobjectfromnick('recogen', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('recogen', files[0], changes, rebin=1).GetMeanError())
    zresp = (getroot.getobjectfromnick('zresp', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('zresp', files[0], changes, rebin=1).GetMeanError())
    balresp = (getroot.getobjectfromnick('balresp', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('balresp', files[0], changes, rebin=1).GetMeanError())
    mpfresp = (getroot.getobjectfromnick('mpfresp', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('mpfresp', files[0], changes, rebin=1).GetMeanError())
    mpfresp_raw = (getroot.getobjectfromnick('mpfresp-raw', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('mpfresp-raw', files[0], changes, rebin=1).GetMeanError())
    genbal = (getroot.getobjectfromnick('genbal', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('genbal', files[0], changes, rebin=1).GetMeanError())
    balparton = (getroot.getobjectfromnick('balparton', files[0], changes, rebin=1).GetMean(), getroot.getobjectfromnick('balparton', files[0], changes, rebin=1).GetMeanError())
    partoncorr = divide(balparton, genbal)

    format = "%1.4f"
    print changes
    print ""
    print (r"balresp reco        %s +- %s" % (format, format)) % balresp
    print (r"mpf                 %s +- %s" % (format, format)) % mpfresp
    print (r"balparton           %s +- %s" % (format, format)) % balparton
    print (r"zresp               %s +- %s" % (format, format)) % zresp
    print (r"recogen             %s +- %s" % (format, format)) % recogen
    print (r"extrapolReco_factor %s +- %s" % (format, format)) % extrapol_reco_factor
    print (r"extrapolGen_factor  %s +- %s" % (format, format)) % extrapol_gen_factor
    print (r"extrapolMPF_factor  %s +- %s" % (format, format)) % extrapol_mpf_factor
    print (r"parton/genjet       %s +- %s" % (format, format)) % divide(balparton, genbal)
    print ""
    print (r"pTgenjet / pTgenZ                %s +- %s" % (format, format)) % genbal

    genbal = multiply(genbal, extrapol_gen_factor)
    print (r"* gen Level extrapolation        %s +- %s" % (format, format)) % genbal

    #genbal = multiply(genbal, partoncorr)
    #print (r"* pTparton/pTgenjet correction   %s +- %s" % (format, format) ) % genbal

    #genbal = divide(genbal, balparton)
    #print (r"* pTparton/pTZ      correction   %s +- %s" % (format, format) ) % genbal

    reco_bal = divide(multiply(genbal, recogen), zresp)
    print (r"* GenToReco for Jet and Z        %s +- %s" % (format, format)) % reco_bal

    print ""
    print (r"pTrecojet / pTrecoZ              %s +- %s" % (format, format)) % balresp

    balresp = multiply(balresp, extrapol_reco_factor)
    print (r"* reco Level extrapolation       %s +- %s" % (format, format)) % balresp

    print ""
    print (r"MPF (typeI)                      %s +- %s" % (format, format)) % mpfresp
    #mpfresp = divide(mpfresp, zresp)
    #print (r"MPF (GenZ)                             %s +- %s" % (format, format) ) % mpfresp

    mpfresp = multiply(mpfresp, extrapol_mpf_factor)
    print (r"MPF (extrapol)                   %s +- %s" % (format, format)) % mpfresp

    print (r"MPF (Raw)                        %s +- %s" % (format, format)) % mpfresp_raw


def extrapola(files, opt):
    fig, ax = plotbase.newplot()
    changes = {}
    changes['var'] = "_var_CutSecondLeadingToZPt_0_3"
    local_opt = copy.deepcopy(opt)

    rebin = 5
    if opt.rebin is not None:
        rebin = opt.rebin

    plotdatamc.datamcplot('ptbalance_alpha', files, local_opt, legloc='upper center',
           changes=changes, rebin=rebin, subplot=True,
           subtext="", fig_axes=(fig, ax), fit='intercept', ratio=False)

    local_opt.colors = ['red', 'maroon']
    plotdatamc.datamcplot('mpf_alpha', files, local_opt, legloc='upper center',
           changes=changes, rebin=rebin, subplot=True, xy_names=['alpha', 'response'],
           subtext="", fig_axes=(fig, ax), fit='intercept', ratio=False, fit_offset=-0.1)

    file_name = plotbase.getdefaultfilename("extrapolation_", opt, changes)
    plotbase.Save(fig, file_name, opt)


# function for comparing old and new corrections
def comparison(datamc, opt):
    """file_names = [
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root',
                '/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root'
                ]"""
    colors = ['red', 'blue', 'blue', 'red']
    markers = ['*', 'o', 'o', '*']
    #labels = [['MC_52xFast', 'data_52xFast'], ['MC_52xOff', 'data_52xOff'], ['MC_53xFast', 'data_53xFast'], ['MC_53xOff', 'data_53xOff']]
    rebin = 1
    import copy
    file_names = [
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root',
                '/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root',
                ]
    labels = [['MC_52xFast', 'data_52xFast'], ['MC_53xFast', 'data_53xFast'], ['MC_52xOff', 'data_52xOff'], ['MC_53xOff', 'data_53xOff']]

    files = []
    for f in file_names:
        files += [getroot.openfile(f, opt.verbose)]
    local_opt = copy.deepcopy(opt)
    local_opt.style = markers
    local_opt.colors = colors
    quantity = 'L1abs_npv'

    # ALL
    fig, axes = plotbase.newplot(subplots=4)
    for a, f1, f2, l in zip(axes, files[::2], files[1::2], labels):
        local_opt.labels = l
        datamcplot(quantity, (f1, f2), local_opt, 'upper center', changes={'correction': ''}, fig_axes=(fig, a),
                    rebin=rebin, subplot=True, subtext="")

    filename = "L1_all__" + opt.algorithm
    plotbase.Save(fig, filename, opt)
    """

    #Fastjet vs Offset
    fig = plotbase.plt.figure(figsize=(14,7))
    axes = [fig.add_subplot(1,2,n) for n in [1,2]]

    local_opt.labels = labels[0]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[0], files[1]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")
    local_opt.labels = labels[1]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[2], files[3]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")
        #53
    local_opt.labels = labels[2]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[4], files[5]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")
    local_opt.labels = labels[3]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[6], files[7]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")

    filename = "L1_Fastjet_vs_Offset__"+opt.algorithm
    plotbase.Save(fig, filename, opt)


    #52X vs 53X
    fig = plotbase.plt.figure(figsize=(14,7))
    axes = [fig.add_subplot(1,2,n) for n in [1,2]]

    local_opt.labels = labels[0]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[0], files[1]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")

    local_opt.labels = labels[2]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[4], files[5]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")

    local_opt.labels = labels[1]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[2], files[3]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")
        #
    local_opt.labels = labels[3]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[6], files[7]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")

    filename = "L1_52X_vs_53X__"+opt.algorithm
    plotbase.Save(fig, filename, opt)

    import plotresponse


    file_names = [
                '/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                ]
    labels = [['data_52xFast', 'MC_52xFast'], [ 'data_53xFast', 'MC_53xFast'], [ 'data_52xOff', 'MC_52xOff'], ['data_53xOff', 'MC_53xOff']]

    files=[]
    for f in file_names:
        files += [getroot.openfile(f, opt.verbose)]

    for over, fit in zip(['zpt', 'jet1eta', 'npv'], [True, False, True]):

        fig, axes= plotbase.newplot(subplots=4)
        fig2, axes2= plotbase.newplot(subplots=4)
        for a1, a2, f1, f2, l in zip(axes, axes2, files[::2], files[1::2], labels):
            local_opt.labels = l
            changes ={}# {'correction':'L1L2L3'}
            plotresponse.responseplot((f1, f2), local_opt, ['bal', 'mpf'], over=over, changes=changes, figaxes=(fig,a1),
                        subplot=True, subtext="")
            plotresponse.ratioplot((f1, f2), local_opt, ['bal', 'mpf'], over=over, changes=changes, figaxes=(fig2 ,a2), fit=fit,
                        subplot=True, subtext="")

        filename = "Response_"+over+"_all__"+opt.algorithm
        plotbase.Save(fig, filename, opt)

        filename = "Ratio_"+over+"_all__"+opt.algorithm
        plotbase.Save(fig2, filename, opt)"""


# function for 2d grid plots
"""def twoD_all_grid(quantity, datamc, opt):
    pt_thresholds = [12, 16, 20, 24, 28, 32, 36]
    var_list = ['var_JetPt_%1.fto%1.f' % (s1, s2) for (s1, s2) in zip(pt_thresholds, [1000, 1000, 1000, 1000, 1000, 1000, 1000])]
    var_list_2 = getroot.npvstrings(opt.npv)

    fig = plt.figure(figsize=(10.*len(var_list), 7.*len(var_list_2)))
    grid = AxesGrid(fig, 111,
                        nrows_ncols = (len(var_list), len(var_list_2)),
                        axes_pad = 0.4,
                        share_all=True,
                        label_mode = "L",
                        #aspect = True,
                        #cbar_pad = 0,
                        #cbar_location = "right",
                        #cbar_mode='single',
                        )

    for n1, var1 in enumerate(var_list):
            for n2, var2 in enumerate(var_list_2):
                    change = {'var':var1+"_"+var2}
                    index = len(var_list_2)*n1 + n2
                    change['incut']='allevents'
                    twoD(quantity, datamc, opt, changes=change, fig_axes = [fig, grid[index]], subplot = True, axtitle = change['var'].replace('var_', ''))

    for grid_element, var_strings in zip(grid, opt.npv):
        text = r"$%s\leq\mathrm{NPV}\leq%s$" % var_strings
        grid_element.text(0.5, 5.5, text, ha='center', va='center', size ='40')

    for grid_element, pt_threshold in zip(grid[::len(var_list_2)], pt_thresholds):
        text = r"$p_\mathrm{T}^\mathrm{Jet1}$"+"\n"+r"$\geq%s\mathrm{GeV}$" % pt_threshold
        grid_element.text(-8.7, 0, text, ha='left', va='center', size ='30')

    #fig.suptitle("%s leading jet $\eta-\phi$ distribution ($before$ cuts) for %s %s" % (opt.labels[0], opt.algorithm, opt.correction), size='50')
    fig.suptitle("%s %s $\eta-\phi$ distribution ($before$ cuts) for %s %s" % (opt.labels[0], quantity[7:-16], opt.algorithm, opt.correction), size='30')

    file_name = "grid_"+opt.labels[0]+"_"+quantity +"_"+opt.algorithm + opt.correction
    fig.set_figwidth(fig.get_figwidth() * 1.2)
    plotbase.Save(fig, file_name, opt, crop=False, pad=1.5)"""


def Fall12(files, opt):
    local_opt = copy.deepcopy(opt)
    filelist = [
                ['/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root'],
                 ['/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root'],
                 ['/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_V4/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_V4/out/closure.root']
                ]
    labellist = [['data_Summer12', 'MC_Summer12'], ['data_Fall12V1', 'MC_Fall12V1'], ['data_Fall12V4', 'MC_Fall12V4']]

    over = 'zpt'
    for over in ['zpt', 'npv', 'jet1eta']:
        fig = plotbase.plt.figure(figsize=[21, 14])
        fig.suptitle(opt.title, size='xx-large')
        for typ, row in zip(['bal', 'mpf'], [0, 4]):
            for filenames, labels, col in zip(filelist, labellist, [0, 1, 2]):

                ax1 = plotbase.plt.subplot2grid((7, 3), (row, col), rowspan=2)
                ax2 = plotbase.plt.subplot2grid((7, 3), (row + 2, col))
                fig.add_axes(ax1)
                fig.add_axes(ax2)

                if over == 'jet1eta' and typ == 'bal':
                    legloc = 'upper right'
                else:
                    legloc = 'lower left'

                local_opt.labels = labels

                files = []
                for f in filenames:
                    files += [getroot.openfile(f, opt.verbose)]

                plotresponse.responseplot(files, local_opt, [typ], over=over, figaxes=(fig, ax1), legloc=legloc, subplot=True)
                plotresponse.ratioplot(files, local_opt, [typ], binborders=True, fit=True, over=over, subplot=True, figaxes=(fig, ax2), ratiosubplot=True)
                fig.subplots_adjust(hspace=0.05)
                ax1.set_xticks([])
                ax1.set_xlabel("")
                ax2.set_yticks([1.00, 0.95, 0.90])
                if col > 0:
                    ax1.set_ylabel("")
                    ax2.set_ylabel("")

        title = ""  # "                               Jet Response ($p_T$ balance / MPF) vs. Z $p_T$, $N_{vtx}$ ,  Jet $\eta$   ("  +opt.algorithm+" "+opt.correction+")"
        fig.suptitle(title, size='x-large')

        file_name = "comparison_ALL_" + over + opt.algorithm + opt.correction
        plotbase.Save(fig, file_name, opt)


def factors(files, opt):
    local_opt = copy.deepcopy(opt)
    filelist = [
                 ['/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root'],

                 ['/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_V4/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_V4/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_V4_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_V4_L1Offset/out/closure.root']
                ]
    labellist = [
                ['data FastJet V1', 'MC FastJet V1', 'data Offset V1', 'MC Offset V1'],
                ['data FastJet V4', 'MC FastJet V4', 'data Offset V4', 'MC Offset V4']]

    """filelistt = [
                 ['/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_V4/out/closure.root'],

                 ['/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_V4/out/closure.root'],

                 ['/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_V4_L1Offset/out/closure.root'],

                 ['/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_V4_L1Offset/out/closure.root']

                ]
    labellistt = ['data FastJet V1', 'data FastJet V4'], ['MC FastJet V1',  'MC FastJet V4'], ['data Offset V1',  'data Offset V4'], ['MC Offset V1','MC Offset V4'
]]

    names = ['dataV1', 'MCV1', 'dataV4', 'MCV4' ]"""

    files = []
    #for sublist in filelist:
    #    rootfiles = [getroot.openfile(f, opt.verbose) for f in sublist]
    #    files.append( rootfiles)

    for sublist in filelist:
        files.append([getroot.openfile(f, opt.verbose) for f in sublist])

    fit = None
    rebin = 1

   # for files, labellist, name in zip(files, labellist, names)
    fig, axes = plotbase.newplot(subplots=2)
    quantity = 'L1abs_npv'
    local_opt.style = ['o', '*', 'o', '*']

    local_opt.labels = labellist[0]
    local_opt.colors = ['blue', 'blue', 'red', 'red']

    plotdatamc.datamcplot(quantity, files[0], local_opt, 'upper center', changes={'correction': ''}, fig_axes=(fig, axes[0]), fit=fit,
                        rebin=rebin, subplot=True, subtext="")

    local_opt.labels = labellist[1]
    plotdatamc.datamcplot(quantity, files[1], local_opt, 'upper center', changes={'correction': ''}, fig_axes=(fig, axes[1]), fit=fit,
                        rebin=rebin, subplot=True, subtext="")

    file_name = "L1_comparison_"  # +name
    plotbase.Save(fig, file_name, opt)


def factors2(files, opt):
    local_opt = copy.deepcopy(opt)

    filelist = [
                 ['/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_V4/out/closure.root'],

                 ['/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_V4/out/closure.root'],

                 ['/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_V4_L1Offset/out/closure.root'],

                 ['/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_V4_L1Offset/out/closure.root']

                ]
    labellistt = [['data FastJet V1', 'data FastJet V4'], ['MC FastJet V1', 'MC FastJet V4'], ['data Offset V1', 'data Offset V4'], ['MC Offset V1', 'MC Offset V4']
]

    names = ['dataV1', 'MCV1', 'dataV4', 'MCV4']

    files = []
    for sublist in filelist:
        rootfiles = [getroot.openfile(f, opt.verbose) for f in sublist]
        files.append(rootfiles)
    #print files

    fit = 'chi2_linear'
    rebin = 1
    fit_offset = -0.1

    for files, labellist, name in zip(files, labellistt, names):
        print labellist
        fig, axes = plotbase.newplot(subplots=2)
        quantity = 'L1abs_npv'
        local_opt.style = ['o', '*', 'o', '*']

        local_opt.labels = [labellist[0]]
        local_opt.colors = ['blue', 'blue', 'red', 'red']

        plotdatamc.datamcplot(quantity, [files[0]], local_opt, 'upper center', changes={'correction': ''}, fig_axes=(fig, axes[0]), fit=fit,
                            rebin=rebin, fit_offset=fit_offset, subplot=True, subtext="")

        local_opt.labels = [labellist[1]]
        plotdatamc.datamcplot(quantity, [files[1]], local_opt, 'upper center', changes={'correction': ''}, fig_axes=(fig, axes[1]), fit=fit,
                            rebin=rebin, fit_offset=fit_offset, subplot=True, subtext="")

        file_name = "L1_comparison_" + name
        plotbase.Save(fig, file_name, opt)

import ROOT


def allpu(files, opt, truth=True):
    print files

    settings = plotbase.getsettings(opt, quantity='npu')
    #print settings
    print settings['folder']
    name = "_".join([settings['folder'], settings['algorithm'] + settings['correction']])
    print name, files[1]
    name = name.replace("Res", "")
    t = files[1].Get(name)
    if not t:
        print "no tree", name, t.GetName()
        exit(1)
    # raw wei data weight
    if truth:
        histos = [getroot.getobject("pileup", files[2])]
    else:
        histos = [getroot.getobject("pileup;2", files[2])]
        histos[-1].Rebin(10)
        print histos[-1].GetNbinsX(), "pu2"
    histos[0].SetTitle("data")
    histos += [ROOT.TH1D("mcraw", "MC", 1600, 0, 80)]
    if truth:
        histos += [ROOT.TH1D("mcraw", "MC", 1600, 0, 80)]
        t.Project("mcraw", "nputruth")
    else:
        histos += [ROOT.TH1D("mcraw", "MC", 80, 0, 80)]
        t.Project("mcraw", "npu")

    if truth:
        histos += [ROOT.TH1D("mcwei", "MC'", 1600, 0, 80)]
        t.Project("mcwei", "nputruth", "weight")
    else:
        histos += [ROOT.TH1D("mcwei", "MC'", 80, 0, 80)]
        t.Project("mcwei", "npu")

    binning = [[0, 1, 2, 3.5, 5], range(45, 80)]
    for h in histos:
        if h.GetNbinsX() > 1000:
            h.Rebin()
        if h.GetNbinsX() > 82:
            print h.GetNbinsX(), ">82! in", h.GetTitle()
        if not truth:
            break
        print "rebin:", binning
        b = binning
        if histos.index(h) == 1:
            b = binning + [range(5, 46)]
        print b
        for l in b:
            for a, b in zip(l[:-1], l[1:]):
                x1 = h.FindBin(a)
                x2 = h.FindBin(b)
                sumh = sum([h.GetBinContent(i) for i in range(x1, x2)]) / (x2 - x1)
                for i in range(x1, x2):
                    h.SetBinContent(i, sumh)
    if truth:
		f = histos[1].Integral() / histos[1].Integral(histos[1].FindBin(8), histos[1].FindBin(40))
		for i in range(3 + 0 * len(histos)):
		    #histos[i].Rebin(4)
		    print i
		    ff = f / histos[i].Integral(histos[i].FindBin(8), histos[i].FindBin(40))
		    ff = 1.0 / histos[i].Integral()
		    histos[i].Scale(ff)

    histos += [histos[0].Clone("dataraw")]
    histos[-1].SetTitle("data/MC")
    histos[-1].Divide(histos[1])
    if len(files) > 3:
        histos += [getroot.getobject("pileup", files[3])]
        histos[-1].SetTitle("weight")

    histos += [histos[2].Clone("rawmc")]
    histos[-1].Divide(histos[1])
    histos[-1].SetTitle("MC'/MC")
    histos += [histos[0].Clone("datamc")]
    histos[-1].Divide(histos[2])
    histos[-1].SetTitle("data/MC'")

    plots = [getroot.root2histo(h) for h in histos]
    fig, ax, ratio = plotbase.newplot(ratio=True)
    fig = plotbase.plt.figure(figsize=[7, 10])
    ax = plotbase.plt.subplot2grid((3, 1), (0, 0), rowspan=2)
    ax.number = 1
    ratio = plotbase.plt.subplot2grid((3, 1), (2, 0))
    ratio.number = 2
    fig.add_axes(ax)
    fig.add_axes(ratio)
    fig.subplots_adjust(hspace=0.05)

    colors = ['black', 'navy', 'red', 'green']

    for p, c in zip(plots[:3], colors):
        ax.errorbar(p.x, p.y, label=p.title, drawstyle='steps-post', color=c, lw=1.6)
    colors[1] = 'gray'
    for p, c in zip(plots[3:], colors):
        r = ratio.errorbar(p.x, p.y, label=p.title, drawstyle='steps-post', color=c, lw=1.6)

    plotbase.labels(ax, opt, settings, settings['subplot'])
    plotbase.axislabels(ax, r"$n_\mathrm{PU}", settings['xynames'][1], settings=settings)
    xaxistext = r"observed number of pile-up interactions $n_\mathrm{PU}$"
    if truth:
        xaxistext = xaxistext.replace("observed", "true")
    plotbase.axislabels(ratio, xaxistext, "ratio", settings=settings)
    print ratio.number, r
    plotbase.setaxislimits(ax, settings)
    plotbase.labels(ratio, opt, settings, settings['subplot'])
    plotbase.setaxislimits(ratio, settings)
    #handles, labels = ratio.get_legend_handles_labels()
    ratio.legend(bbox_to_anchor=[0.8, 1], loc='upper center')
    ax.set_xticklabels([])
    ax.set_xlabel("")

    settings['filename'] = plotbase.getdefaultfilename("npus", opt, settings)

    plotbase.Save(fig, settings)


def pu(files, opt):
    allpu(files, opt)


def puobserved(files, opt):
    allpu(files, opt, False)
