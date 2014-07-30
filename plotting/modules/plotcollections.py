import plotbase, plot1d, plotresponse, plotfractions, plot2d, plot_tagging
import getroot, fit
import math, copy, os



def e07hlt(files, opt):
    files = [getroot.openfile("work/mc_ee_corr.root")]
    plot2d.twoD("hlt_eeta_ept", files, opt, changes = {
        'folder': 'all',
        'channel': 'ee',
        'y': [-2.5, 2.5],
        'x': [20, 150],
        'z': [0.6, 1],
        'nbins': 50,
        'labels': ['Madgraph'],
        'xynames': ['Leading electron $p_T$',
                    'Leading electron $\eta$',
                    'HLT efficiency'],
        'nolumilabel': True,
    })

def e07lhe(files, opt):
    files = [getroot.openfile("work/mc_ee_corr.root")]*2
    changes = {
        'folder': 'zcuts',
        'channel': 'ee',
        'x': [-2.5, 2.5],
        'nbins': 50,
        'title': 'Madgraph',
        'ratiosubploty': [0.9, 1.1],
        'nolumilabel': True,
    }
    changes.update({
        'labels': ['Reco', 'Gen'],
        'xaxis': ['zy', 'genzy'],
    })
    plot1d.plot1dratiosubplot("zy", files, opt, changes = changes)
    
    changes.update({
        'labels': ['Gen', 'LHE'],
        'xaxis': ['genzy', 'lhezy'],
        'filename': 'genzy',
    })
    plot1d.plot1dratiosubplot("zy", files, opt, changes = changes)

    plot1d.plot1d("zmass", files, opt, changes = {
        'folder': 'zcuts',
        'channel': 'ee',
        'x': [81, 101],
        'nolumilabel': True,
        'nbins': 50,
        'labels': ['Reco', 'Gen', 'LHE'],
        'xaxis': ['zmass', 'genzmass', 'lhezmass'],
    })

def e07y(files, opt):
    """ Plot y and -y histograms, to check rapidity assymetry. """
    for files, filename, nolumilabel, title in zip(
            [[getroot.openfile("work/data_ee_corr.root")]*2, [getroot.openfile("work/mc_ee_corr.root")]*2],
            ['zy_data', 'zy_mc'],
            [False, True],
            ['data', 'Madgraph'],
        ):
        plot1d.plot1dratiosubplot("zy", files, opt, changes = {
            'folder': 'zcuts',
            'channel': 'ee',
            'x': [-2.8, 2.8],
            'ratiosubploty': [0.9, 1.1],
            'nolumilabel': nolumilabel,
            'nbins': 50,
            'labels': ['y', '-y'],
            'xaxis': ['zy', '-zy'],
            'filename': filename,
            'legloc': 'lower center',
            'title': title,
        })
        
def e07geny(files, opt):
    """ Plot y and -y gen histograms, to check rapidity assymetry. """
    for a, title in zip(['mc_ee', 'mc_ee_powheg'], ['Madgraph', 'Powheg']):
        for i in ['genzy', 'lhezy']:
            plot1d.plot1dratiosubplot(i, [], opt, changes = {
                'folder': 'zcuts',
                'channel': 'ee',
                'x': [-2.8, 2.8],
                'ratiosubploty': [0.9, 1.1],
                'nolumilabel': True,
                'nbins': 25,
                'labels': ['y', '-y'],
                'xaxis': [i, '-%s' % i],
                'legloc': 'lower center',
                'title': title,
                'files': ["work/%s_corr.root" % a]*2,
                'filename': "%s_%s_assym" % (i, a),
            })

def e07etaphi(files, opt):
    """ 2D electron eta phi distribution. """
    files = [getroot.openfile("work/%s_noc.root" % s) for s in ['data_ee_corr', 'mc_ee_corr']]
    plot2d.twoD("phi_eta", files, opt, changes = {
        'folder': 'electron_all',
        'channel': 'ee',
        'y': [-3.2, 3.2],
        'x': [-2.5, 2.5],
        'nbins': 500,
        'xynames': ['eta', 'phi', 'electrons']
    })

def e07eff(files, opt):
    """ Efficiency for IDs. """
    files = [getroot.openfile("work/mc_ee_corr_noid_noc.root")]*4
    for selection, filename, yname in zip(
        ['deltar<0.3', 'deltar>0.3'],
        ['ideff', 'idfake'],
        ['Efficiency', 'Fake rate'],
    ):
        plot1d.plot1d("id_pt", files, opt, changes = {
            'folder': 'electron_zcuts',
            'channel': 'ee',
            'yaxis': ['mva', 'looseid', 'mediumid', 'tightid'],
            'x': [20, 120],
            'nbins': 50,
            'labels': ['MVA ID', 'Loose cutbased ID', 
                        'Medium cutbased ID', 'Tight cutbased ID'],
            'markers': ['o', '-', '-', '-'],
            'y': [0, 1], 
            'selection': [selection],
            'yname': yname,
            'xname': 'Electron $p_\mathrm{T}$ / GeV',
            'colors': ['black', 'blue', 'red', 'green'],
            'filename': filename,
            'nolumilabel': True,
        })

def e07eeta(files, opt):
    for files, filename, nolumilabel, title in zip(
            [[getroot.openfile("work/data_ee_corr.root")]*2, [getroot.openfile("work/mc_ee_corr.root")]*2],
            ['eeta_data', 'eeta_mc'],
            [False, True],
            ['data', 'Madgraph'],
        ):
        plot1d.plot1dratiosubplot("eta", files, opt, changes = {
            'folder': 'zcuts',
            'channel': 'ee',
            'x': [-2.8, 2.8],
            'ratiosubploty': [0.9, 1.1],
            'nolumilabel': nolumilabel,
            'nbins': 50,
            'labels': ['$\eta (e^{-})$', '$-\eta (e^{-})$'],
            'xaxis': ['eminuseta', '-eminuseta'],
            'filename': filename,
            'title': title,
        })


def e07pu(files, opt):
    plot1d.plot1dratiosubplot('npv', [], opt, changes = {
        'folder': 'all',
        'channel': 'ee',
        'files': ['work/%s_corr.root' % f for f in ['data_ee_corr', 'mc_ee_powheg_corr']]
    })
    plot1d.plot1dratiosubplot('npv', [], opt, changes = {
        'folder': 'all',
        'channel': 'ee',
        'labels': ['data', 'Madgraph'],
        'mcweights': False,
        'filename': 'npv_unweighted',
        'files': ['work/%s_corr.root' % f for f in ['data_ee_corr', 'mc_ee_powheg_corr']]
    })
    
def e07pt(files, opt):
    """ data vs MC for electron pT, raw and corrected. """
    for name, title in zip(['raw', 'corr'], ['raw', 'corrected']):
        plot1d.plot1dratiosubplot('eminuspt', [], opt, changes = {
            'folder': 'zcuts',
            'channel': 'ee',
            'nbins': 50,
            'normalize': False,
            'selection': ['1', 'hlt * sfminus * sfplus'],
            'ratiosubploty': [0.9, 1.1],
            'title': title,
            'filename': 'eminuspt_%s' % name,
            'files': ['work/%s_ee_%s.root' % (f, name) for f in ['data', 'mc']],
            'x': [20, 120],
        })

def e07zmass(files, opt):
    """ data vs MC for Z mass, raw and corrected electrons. """
    for name, title in zip(['raw', 'corr'], ['raw', 'corrected']):
        plot1d.plot1dratiosubplot('zmass', [], opt, changes = {
            'folder': 'zcuts',
            'channel': 'ee',
            'nbins': 50,
            'normalize': False,
            'selection': ['1', 'hlt * sfminus * sfplus'],
            'ratiosubploty': [0.5, 1.5],
            'title': title,
            'filename': 'zmass_%s' % name,
            'files': ['work/%s_ee_%s.root' % (f, name) for f in ['data', 'mc']],
            'x': [81, 101],
        })

def e07z(files, opt):
    """ z quantities. """
    files = [getroot.openfile("work/%s.root" % s) for s in ['data_ee_corr', 'mc_ee_corr']]
    base_changes = {
            'folder': 'zcuts',
            'channel': 'ee',
            'nbins': 50,
            'normalize': False,
            'selection': ['1', 'hlt * sfminus * sfplus'],
            'ratiosubploty': [0.8, 1.2],
            'labels': ['data', 'Madgraph'],
    }

    changes = {
        'x': [81, 101],
        'filename': 'final_zmass',
    }
    changes.update(base_changes)
    plot1d.plot1dratiosubplot('zmass', files, opt, changes = changes)

    changes = {
        'x': [30, 300],
        'filename': 'final_zpt',
        'log': True,
    }
    changes.update(base_changes)
    plot1d.plot1dratiosubplot('zpt', files, opt, changes = changes)

    changes = {
        'x': [-2.8, 2.8],
        'filename': 'final_zy',
        #'log': True,
    }
    changes.update(base_changes)
    plot1d.plot1dratiosubplot('zy', files, opt, changes = changes)

    for sel, filename, title in zip(['abs(zy)<1', 'abs(zy)>1 && abs(zy)<2', 'abs(zy)>2'],
        ['zpt01', 'zpt12', 'zpt2'],
        ['$|y_Z|<1$', '$1<|y_Z|<2$', '$2<|y_Z|$']
    ):
        changes = {
            'x': [30, 300],
            'log': True,
        }
        changes.update(base_changes)
        changes.update({
            'title': title,
            'selection': ['%s && %s' % (sel, s) for s in base_changes['selection']],
            'filename': filename,
        })
        plot1d.plot1dratiosubplot('zpt', files, opt, changes = changes)


def e07roc(files, opt):
    """ roc 'curve' with the points from ids. """
    files = [getroot.openfile("work/mc_ee_corr_noid_noc.root")]
    ids = ['mva', "looseid", "mediumid", "tightid"]
    labels = ['MVA', 'Loose', 'Medium', 'Tight']

    changes = {
        'nbins': 1,
        'folder': 'electron_zcuts',
        'rebin': 1,
        'x': [20, 1000],
        'filename': 'roc',
        'legloc': 'lower right',
    }
    quantity = "id"
    settings = plotbase.getSettings(opt, changes, None, quantity)

    eff, fake = [], []
    fig, ax = plotbase.newPlot()

    for ID, c, l in zip(ids, ['black', 'blue', 'red', 'green'], labels):
        quantity = "%s_pt" % ID
        changes['selection'] = ['deltar<0.3']
        settings = plotbase.getSettings(opt, changes, None, quantity)
        histo = getroot.getplotfromtree(quantity, files[0], settings, twoD=False, changes=None)
        eff += [histo.y[0]]

        changes['selection'] = ['deltar>0.3']
        settings = plotbase.getSettings(opt, changes, None, quantity)
        histo = getroot.getplotfromtree(quantity, files[0], settings, twoD=False, changes=None)
        fake += [histo.y[0]]
        

        ax.errorbar(fake[-1], eff[-1], drawstyle='steps-mid', color=c,
                        fmt='o', capsize=0, markersize=10, label = l)
    settings.update({
        'y': [0.7, 1],
        'x': [0, 0.3001],
        'xynames': ['Fake rate', 'Efficiency'],
        'nolumilabel': True,
    })
    plot1d.formatting(ax, settings, opt, None, rootobjects=None)
    plotbase.Save(fig, settings)


def e07(files, opt):
    """ plots for the electron presentation on 29.07.14. """

    changes = {
        'normalize': False,
    }
    
    # hlt efficiency
    e07y(files, opt)
    e07geny(files, opt)
    e07eeta(files, opt)
    e07hlt(files, opt)
    e07lhe(files, opt)
    e07etaphi(files, opt)
    e07eff(files, opt)
    e07pu(files, opt)
    e07pt(files, opt)
    e07zmass(files, opt)
    e07z(files, opt)
    e07roc(files, opt)

    #momentum corrections


    #electron_id
    files = [getroot.openfile("work/mc_ee_corr_noid_noc.root")]
    """for quantity in ["mvaid", "mvatrigid", "looseid", "mediumid", "tightid"]:
        plot1d.datamcplot("%s_deltar" % quantity, files, opt, changes = {
        'folder': 'electron_all',
        'nbins': 50,
        'lumi': 0,
        'xynames': ['$\Delta$R(reco, gen)', quantity],
        'x': [0, 0.5],
        'legloc': None,
        })

    quantity = 'mvaid'
    changes = {
        'x': [0, 1.0001],
        'y':[1, 8000000],
        'log': True,
        'lumi':0,
        'folder': 'electron_all',
        'xynames': ['MVA ID','Electrons'],
        'nbins':50,
        'subplot':True,
        'markers': ['f']
    }
    settings = plotbase.getSettings(opt, quantity=quantity)
    fig, ax = plotbase.newPlot()

    for c, l, s  in zip(['#236BB2', '#E5AD3D'],
            #['fake', 'true'],
            #['1', 'deltar < 0.3 && deltar>0']
            ['true', 'fake'],
            ['1', 'deltar > 0.3 || deltar<0']
            ):
        changes.update({
            'labels': [l],
            'colors': [c],
            'selection': s,
        })
        plot1d.datamcplot(quantity, files, opt, fig_axes = [fig, ax], changes=changes)

    settings['filename'] = plotbase.getDefaultFilename(quantity, opt, settings)
    plotbase.Save(fig, settings)

    # fake rate
    changes = {
        'lumi':0,
        'folder': 'electron_all',
        'nbins':50,
        'xynames': ['MVA ID', 'Fake rate'],
        'legloc': None,
        'filename': 'fakerate',
    }
    plot1d.datamcplot("(deltar>0.3||deltar<0)_mvaid", files, opt, changes=changes)
    """

    #lhe
    files = [getroot.openfile("work/mc_ee_corr.root")]
    fig, ax, ax2 = plotbase.newPlot(ratio = True)
    quantity = 'y'
    changes ={
        'folder':'zcuts',
        'x': [-2.6, 2.6],
        #'y': [1000, 800000],
        'subplot': True,
        'nbins':50,
        'normalize': False,
        'xynames': ['Z rapidity', 'events'],
        'log':True,
    }
    for q, c, m, l in zip(
            #['zy', 'genzy', 'lhezy'], 
            [x + quantity for x in ['z', 'genz', 'lhez']],
            ['black', 'lightskyblue', 'FireBrick'],
            ['o', 'f', '-'],
            ['RecoZ', 'GenZ', 'LHE-Z'],
        ):
        changes['labels'] = [l]
        changes['markers'] = [m]
        changes['colors'] = [c]
        plot1d.datamcplot(q, files, opt, changes=changes, fig_axes=[fig, ax])
        

        #rootobjects += [getroot.histofromfile(quantity, files[0], settings)]
        #rootobject = getroot.rootdivision(rootobjects, settings['normalize'])
        #datamc = [getroot.root2histo(rootobject, files[0].GetName(), 1)]
        
        
        
    #plot1d.datamcplot("zy/genzy", files, opt, changes=changes, fig_axes=[fig, ax2])
    """
    harry.py -i ../../../git/CalibFW/work/mc_ee_corr.root  --folder "zcuts_AK5PFJetsCHSL1L2L3" -x --xlims -2.6 2.6 --ratiosubplot -x lhezy genzy -e 8 --yname Events --xname "Z rapidity" --nbins 50 --labels "LHE" "Gen" --legloc "lower center" --filename zy-lhegen
    harry.py -i ../../../git/CalibFW/work/mc_ee_corr.root  --folder "zcuts_AK5PFJetsCHSL1L2L3"  --xlims -2.6 2.6 --ratiosubplot -x zy genzy -e 8 --yname Events --xname "Z rapidity" --nbins 50 --labels "Reco" "Gen" --legloc "lower center" --filename zy-recogen

    #pt
    harry.py -i ../../../git/CalibFW/work/mc_ee_corr.root  --folder "zcuts_AK5PFJetsCHSL1L2L3"  --xlims 30 250 --ratiosubplot -x zpt genzpt -e 8 --yname Events --xname "Z $ p_T$ / GeV" --nbins 50 --labels "Reco" "Gen"  --filename zpt-recogen --log 
    harry.py -i ../../../git/CalibFW/work/mc_ee_corr.root  --folder "zcuts_AK5PFJetsCHSL1L2L3"  --xlims 0 250 --ratiosubplot -x lhezpt genzpt -e 8 --yname Events --xname "Z $ p_T$ / GeV" --nbins 50 --labels "LHE" "Gen"  --filename zpt-lhegen --log

    #mass
    harry.py -i ../../../git/CalibFW/work/mc_ee_corr.root  --folder "zcuts_AK5PFJetsCHSL1L2L3"  --xlims 81 101 --ratiosubplot -x lhezmass genzmass -e 8 --yname Events --xname "Z mass / GeV" --nbins 50 --labels "LHE" "Gen"  --filename zmass-lhegen
    harry.py -i ../../../git/CalibFW/work/mc_ee_corr.root  --folder "zcuts_AK5PFJetsCHSL1L2L3"  --xlims 81 101 --ratiosubplot -x zmass genzmass -e 8 --yname Events --xname "Z mass / GeV" --nbins 50 --labels "Reco" "Gen"  --filename zmass-recogen

    
    """
    
    settings = plotbase.getSettings(opt, None, None, 'rapidity')
    settings['filename'] = 'rapidity'
    plotbase.Save(fig, settings)

