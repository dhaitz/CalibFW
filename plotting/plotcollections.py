import plotbase, plotdatamc, plotresponse, plotfractions, plot2d, plot_tagging
import getroot, fit
import math, copy, os


def e07(files, opt):
    """ plots for the electron presentation on XX.07.14. """

    changes = {
        'normalize': False,
        'factor': 0.878,# account for missing data skim
    }
    #momentum corrections


    #electron_id
    files = [getroot.openfile("work/mc_ee_corr_noid_noc.root")]
    """for quantity in ["mvaid", "mvatrigid", "looseid", "mediumid", "tightid"]:
        plotdatamc.datamcplot("%s_deltar" % quantity, files, opt, changes = {
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
    settings = plotbase.getsettings(opt, quantity=quantity)
    fig, ax = plotbase.newplot()

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
        plotdatamc.datamcplot(quantity, files, opt, fig_axes = [fig, ax], changes=changes)

    settings['filename'] = plotbase.getdefaultfilename(quantity, opt, settings)
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
    plotdatamc.datamcplot("(deltar>0.3||deltar<0)_mvaid", files, opt, changes=changes)
    """

    #lhe
    files = [getroot.openfile("work/mc_ee_corr.root")]
    fig, ax, ax2 = plotbase.newplot(ratio = True)
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
        plotdatamc.datamcplot(q, files, opt, changes=changes, fig_axes=[fig, ax])
        

        #rootobjects += [getroot.histofromfile(quantity, files[0], settings)]
        #rootobject = getroot.rootdivision(rootobjects, settings['normalize'])
        #datamc = [getroot.root2histo(rootobject, files[0].GetName(), 1)]
        
        
        
    #plotdatamc.datamcplot("zy/genzy", files, opt, changes=changes, fig_axes=[fig, ax2])
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
    
    settings = plotbase.getsettings(opt, None, None, 'rapidity')
    settings['filename'] = 'rapidity'
    plotbase.Save(fig, settings)

