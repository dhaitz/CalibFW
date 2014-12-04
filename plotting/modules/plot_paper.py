# -*- coding: utf-8 -*-
"""
    plotting JEC paper module for merlin.

    This module contains the plots for the 2014 JEC paper.
"""

import plotbase
import copy
import plot1d
import getroot
import plotresponse
import plotfractions
import plot2d
import plot_sandbox


def paper(files, opt):
    """ Plots for the 2012 JEC paper. """
    # The Z pT vs alpha plot requested by Viola 23.10.14
    changes={
        'allalpha': True,
        'nbins': 6,
        'x': [0, 0.3],
        'labels': ['Data', 'MC'],
        'y': [0, 250],
        'markers': ['o', '-'],
        'grid': True,
    }
    #plot1d.datamcplot("zpt_alpha", files, opt, changes=changes)


    extrapolationpaper(files, opt)

    npvrhopaper(files, opt)
    npvrhomupaper(files, opt)

    rmspaper(files, opt)
    rmsmupaper(files, opt)

    # The response-vs-flavour plots
    #local_opt = copy.deepcopy(opt)
    filenames = [opt.files[1]]
    local_files, local_opt = plotbase.openRootFiles(filenames, opt)
    local_opt.out = opt.out + '/flavour_response/'
    for a in [0, 1]:
        for b in [0, 1]:
            for c in [0, 1]:
                for d in [0, 1]:
                    plotresponse.response_physflavour(local_files, local_opt,
                        changes={'mconly': True, 'nbins': 25, 'cutlabel': 'eta'},
                        add_neutrinopt=a,
                        restrict_neutrals=b,
                        extrapolation=c,
                        l5=d)

    ## extrapolation plot
    #plotresponse.extrapol(files, opt, changes={'title': 'CMS preliminary',
    #                                    'save_individually': True})

    # alpha dependencies
    changes = {'allalpha': True,
                'y': [0, 400],
                'markers': ['o', 'd'],
                'colors': ['black', 'blue']}
    #plot1d.datamcplot("zpt_alpha", files, opt, changes=changes)
    changes['labels'] = ['MC']
    changes['y'] = [0.98, 1.1]
    changes['mconly'] = [True]
    #plot1d.datamcplot("recogen_alpha", files[1:], opt, changes=changes)

    # HERWIG
    herwigpaper(files, opt)

    plot_sandbox.flavour_comp(files, opt)


def extrapolationpaper(files, opt):
    #new extrapolation plot
    fig, ax1, ax2 = plotbase.newPlot(ratio=True)
    changes = {'subplot': True,
        'y': [0.85, 1.07],
        'fit': True,
        'x': [0, 0.3],
        'legloc': 'lower left',
        'nbins': 6,
        'yname': 'response',
        'allalpha': True,
        'markers': ['D'],
        'ratiosubploty': [0.95, 1.025],
        'ratiosubplotfit': True,
        'ratiosubplotlegloc': 'lower left',
        'colors': ['green'],
        'labels': ['$p_\mathrm{T}^{\mathrm{reco}}$/$p_\mathrm{T}^{\mathrm{ptcl}}$'],
    }
    plot1d.plot1d("recogen_alpha", files[1:], opt, changes=changes, fig_axes = [fig, ax1])

    changes['markers'] = ['o', '*']
    for quantity, color, name in zip(['mpf', 'ptbalance'], [['black', 'blue'], ['red', 'maroon']],
        ['MPF', '$p_\mathrm{T}$ balance']):
        changes['labels'] = ["%s (Data)" % name, "%s (MC)" % name]
        changes['colors'] = color
        changes['ratiosubplotlabel'] = name
        plot1d.plot1dratiosubplot("%s_alpha" % quantity, files, opt, changes=changes,
                fig_axes=[fig, ax1, ax2])

    ax2.set_ylabel("Data / MC")
    settings = plotbase.getSettings(opt, quantity='response_alpha')
    settings['filename'] = plotbase.getDefaultFilename('extrapol', opt, settings)
    plotbase.Save(fig, settings)


def npvrhopaper(files, opt):
    """basic rho and npv plots"""
    for quantity in ['npv', 'rho', 'nputruth']:
        filelist = ['work/data.root', 'work/mc.root'] 
        changes={
                'folder': 'all',
                'labels': ['Data', 'MC'],
                'nbins': 36,
                'x': [-0.5, 35.5],
                'y': [0, 1e6],
                'noautoscale': True,
                'yname': 'Events per bin',
        }
        if quantity == 'nputruth':
            changes['nbins'] = 200
            changes['x'] = [0, 40]
            changes['y'] = [0, 1.3e5]
            filelist.reverse()
            changes['labels'] = ['Data', 'Data']
            changes['markers'] = ['f']
            changes['colors'] = ['SteelBlue']*2
            changes['nolumilabel'] = False
        if quantity == 'rho':
            changes['yname'] = 'Events per GeV'
            changes['y'] = [0, 1.1e6]
        print 'pap', changes
	plot1d.plot1dFromFilenames(quantity, filelist, opt, changes=changes)


def rmspaper(files, opt, yval='npv'):
    for method in ['mpf', 'ptbalance']:
        mlabelstr = "RMS(($R^\mathrm{sim} - R^{%s})/R^\mathrm{sim}$)"
        mlabel = {
            'mpf': mlabelstr % "p_\mathrm{T}",
            'ptbalance': mlabelstr % "\mathrm{MPF}",
            'notfound': "",
        }
        plot2d.twoD("abs((recogen-%s)/recogen)*abs((recogen-%s)/recogen)_%s_zpt" % (method, method, yval),
                files[1:],
                opt,
                changes={
                    'binroot': True,
                    'x': [30, 250],
                    'y': [0.5, 47.5] if yval == 'npv' else [5.0, 50.0],
                    'z': [0, 0.3],
                    'nbins': 22,
                    'nybins': 45,
                    'xynames': ["zpt", yval, mlabel.get(method, mlabel['notfound'])],
                    'filename': '2D_RMS_%s_%s' % (method, yval),
                    #'labels': ["RMS((recogen-%s)/recogen)" % method],
                    'labels': [''],
                    'cutlabel': 'eta',
                    'nolumilabel': True,
                    #'cutlabeloffset':-0.2,
                    'selection': ['alpha<0.3'],
                    'allalpha': True,
                    'mconly': True,
                    #'colormap': 'copper',
                }
        )

def rmsmupaper(files, opt):
    rmspaper(files, opt, 'nputruth')

def herwigpaper(files, opt):
    files2, opt2 = plotbase.openRootFiles(["store/mc_l1.root", "work/mc_herwig.root"], opt)
    for  quantity, suffix in zip(['recogen', '(physl5pt/genjet1pt)'], ["", "_l5"]):
        changes = {'cutlabel': 'eta',
                'labels'  : ['Pythia 6 Tune Z2*', 'Herwig++ Tune EE3C'],
                'y'       : [0.98, 1.05],
                'markers' : ['o', 'd'],
                'colors'  : ['red', 'blue'],
                'normalize': False,
                'mconly'  : True,
                'nolumilabel': True,
                'nbins'   : 25,
                'legloc'  : 'lower left',
                'yname'   : r'Jet Response $p_{\mathrm{T}}^{\mathrm{jet}} / p_{\mathrm{T}}^{\mathrm{ptcl}}$',
                'filename': 'recogen_physflavour_pythia-herwig' + suffix}
        plot1d.datamcplot("%s_physflavour" % quantity, files2, opt2, changes=changes)


def npvrhomupaper(files, opt):
    """npv vs. mu and rho vs. mu profile plots with quadratic fit"""
    for q in ['npv', 'rho']:
        print opt.selection
        changes = {
            'removeruns': True,
            'masked': 0,
            'markers': ['o', 'd'],
            'colors': ['black', 'green'],
            'nbins': 31,
            'folder': 'all',
            'fit': 'quadratic_function',
            'x': [-0.5, 30.5],
            'y': [0, 30],
            'legloc': 'lower right',
        }
        plot1d.datamcplot("%s_nputruth" % q, files, opt, changes=changes)
