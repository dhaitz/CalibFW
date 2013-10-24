import plotbase
import plotdatamc
import getroot
import plot2d
import numpy as np
import copy

taggers = ["qgtag",# "qgmlp", "trackcountinghigheffbjettag",
                #"trackcountinghighpurbjettag", "jetprobabilitybjettag",
                #"jetbprobabilitybjettag", "softelectronbjettag",
                #"softmuonbjettag", "softmuonbyip3dbjettag",
                #"softmuonbyptbjettag", "simplesecondaryvertexbjettag",
                #"combinedsecondaryvertexbjettag", 
                "btag"]
flavourdef = 'physflavour'
selections = ['(%s >0 && %s < 4)' % (flavourdef, flavourdef),
        '(%s==4)' % flavourdef, '(%s==5)' % flavourdef, '(%s==21)' % flavourdef]
titles = ['uds', 'c', 'b', 'gluon']
titles_extended = titles + ['qg-mix', 'lc-mix']
zonelabels = ['uds   ', 'c     ', 'b     ', 'gluon ']
zonelabels_extended = zonelabels + ['qg-mix', 'lc-mix']
response = 'mpf'

zones = ['(btag<0.3 && qgtag>0.9   && btag>-1 && qgtag>-1)', # uds
        '(btag<0.9 && btag>0.7     && btag>-1 && qgtag>-1)', # c
        '(btag>0.9   && btag>-1 && qgtag>-1)',               # b
        '(btag<0.3 && qgtag<0.1   && btag>-1 && qgtag>-1)'   # g
        ]
zones_extended = zones + [
        '(btag<0.3 && qgtag>0.1 && qgtag<0.9 && btag>-1 && qgtag>-1)', #qg mix
        '(btag>0.3 && btag<0.7     && btag>-1 && qgtag>-1)']           # lc mix
colors = ['#236BB2', '#CC2828', '#458E2F', '#E5AD3D']

def tagging(files, opt):
    """Do all the plots in the tagging module."""
    tagging_truthfraction(files, opt)
    tagging_truthflavour(files, opt)
    tagging_2D(files, opt)
    tagging_mpf(files, opt)
    tagging_stacked(files, opt)
    tagging_response(files, opt)
    tagging_response_corrected(files, opt)


def tagging_truthfraction(files, opt):
    """Simple plots: Fraction of MC-Truth b/uds vs. btag/qgtag."""

    for tagger, flavour, selection in zip(['qgtag', 'btag'], ['uds', 'b'], 
                                ['(%s<4)' % flavourdef,'(%s==5)' % flavourdef]):
       changes = {'selection':'%s>0' % flavourdef,
                'xynames':[tagger, "Fraction of %s" % flavour],
                'filename':"_".join([flavour, tagger]),
                'legloc':'None',
                'lumi':0}
       quantity = "_".join([selection, tagger])
       plotdatamc.datamcplot(quantity, files[1:], opt, changes=changes)


def tagging_truthflavour(files, opt):
    """Simple plots: Average tagger value vs. MC Truth flavour."""
    for tagger in taggers:
        changes = {'selection':'%s > -1' % tagger, 'legloc':'None', 'colors':['maroon'],
                    'filename':'tagger-TRUTHtest_%s' % tagger,
                'lumi':0}
        plotdatamc.datamcplot('%s_%s' % (tagger, flavourdef), files[1:], opt, changes=changes)

def tagging_2D(files, opt):
    """2D plots of the btag and qgtag distribution."""
    for rebin in [1, 10]:
        for selection, title in zip(selections, titles):
            changes = {#'selection':'%s' % selection,
                         'labels':['Fraction of %s' % title],
                        'filename':"2D_%s_%s" % (title, rebin), 'rebin':rebin,
                    'lumi':0}
            plot2d.twoD('%s_qgtag_btag' % selection, files[1:], opt, changes=changes)

    
def tagging_mpf(files, opt):
    """MPF plots for the 4 tagging zones, simple (data/MC) and with stacked fractions."""

    for l in [selections, titles, colors]:
        l.reverse()

    stacked = []
    for i in range(len(selections)):
        stacked += ["(%s)" % "||".join(selections[i:])]

    # mpf plots for each zone with the flavour composition stacked
    for zone, enriched in zip(zones_extended, titles_extended):

        fig, ax = plotbase.newplot()

        # determine a scalefactor such that the event-sum of the MCs corresponds to data
        settings = plotbase.getsettings(opt, {'selection':'(%s)' % zone}, quantity=response)
        a = getroot.root2histo(getroot.getobjectfromtree(response, files[0], settings), "xx", 1).ysum()
        settings = plotbase.getsettings(opt, {'selection':'(%s && %s)' % (zone, stacked[0])}, quantity=response)
        b = getroot.root2histo(getroot.getobjectfromtree(response, files[1], settings), "xx", 1).ysum()
        scalefactor = a/b
        del settings

        for selection, title, color in zip(stacked, titles, colors):
            changes = {'selection':'%s ' % " && ".join([selection, zone]), 'labels':["%s" % title], 
                        'title':"%s-enriched" % title, 'colors':[color], 'subplot':True,
                        'markers':'f', 'rebin':4, 'legloc':'upper right',
                        'scalefactor':scalefactor
                        }
            plotdatamc.datamcplot(response, files[1:], opt, (fig, ax), changes=changes)


        changes = {'selection':'%s ' % zone, 'subplot':True, 'title':"%s-enriched" % title,
                        'rebin':4, 'legloc':'upper right'}
        plotdatamc.datamcplot(response, files[:1], opt, (fig, ax), changes=changes)
        plotbase.Save(fig, "%s_enriched_stacked_%s" % (response, enriched), opt, settings={'title':enriched})
        

        # simple mpf data vs MC
        changes = {'selection':'%s ' % zone, 'rebin':4, 'fit':'vertical',
                    'legloc':'center right', 'filename':"mpf_enriched_%s" % enriched,
                    }
        plotdatamc.datamcplot(response, files, opt, changes=changes)
    for l in [selections, titles, colors]:
        l.reverse()


        
def tagging_stacked(files, opt):
    """Tagger distribution plots with the flavour composition stacked."""
    for l in [selections, titles, colors]:
        l.reverse()

    stacked = []
    for i in range(len(selections)):
        stacked += ["(%s)" % "||".join(selections[i:])]

    for tagger in ['qgtag', 'btag']:

        fig, ax = plotbase.newplot()
        for selection, title, color in zip(stacked, titles, colors):

            changes = {'selection':'%s ' % selection, 'labels':["%s" % title], 
                        'colors':[color], 'subplot':True,
                        'markers':'f', 'rebin':4, 'legloc':'upper center'}
            plotdatamc.datamcplot(tagger, files[1:], opt, (fig, ax), changes=changes)

        # determine a scalefactor such that the event-sum of the MCs corresponds to data
        settings = plotbase.getsettings(opt, {'selection':'(%s>-1)' % tagger}, quantity='zpt')
        a = getroot.getobjectfromtree('zpt', files[0], settings).GetEntries()
        settings = plotbase.getsettings(opt, {'selection':'(%s && %s>-1)' % (stacked[0], tagger)}, quantity='zpt')
        b = getroot.getobjectfromtree('zpt', files[1], settings).GetEntries()
        scalefactor = (b/a)
        del settings

        changes = {'subplot':True,
                        'rebin':4, 'legloc':'upper center', 'scalefactor':scalefactor}
        plotdatamc.datamcplot(tagger, files[:1], opt, (fig, ax), changes=changes)
        if tagger is 'btag':
            ax.set_ylim(bottom=1)
            ax.set_yscale('log')
        plotbase.Save(fig, "stacked-%s" % tagger, opt)

    
def tagging_response_corrected(files, opt):
    """Same as tagging_response but with an additional PF-based response correction."""
    tagging_response(files, opt, PFcorrection = True)

def tagging_response(files, opt, PFcorrection = False):
    """Determine the response for each flavour from the response and
       flavour composition in each tagging zone."""

    fig, ax = plotbase.newplot()
    fig_raw, ax_raw = plotbase.newplot()
    labels = titles
    markers = ['o', 's', 'd', '*']
    colors = ['red', 'black', 'yellowgreen', 'lightskyblue', ]

    response_local = response
    if PFcorrection:
        response_local = '((%s+0.4*jet1neutralhadfraction+2.5*jet1muonfraction)*0.962)' % response

    files.reverse()
    for ffile, name, m, c in zip(files, ['mc', 'data'], markers, colors):
        print name
        if name == 'mc':
            flavours_all = []
        mean_all = []
        mean_error_all = []

        #iterate over the 4 zones:
        for zone, enriched in zip(zones_extended, zonelabels_extended):
            flavours = []
            mean = []
            mean_error = []
            changes = {'x':[-1, 2], 'selection':zone}

            # get the fraction for each TRUTH flavour:
            for quantity, label in zip(selections, zonelabels):
                
                if name == 'mc':
                    changes = {'x':[-1, 2], 'selection':zone}
                    changes['selection'] = '(%s && %s>0)' % (changes['selection'], flavourdef)
                    settings = plotbase.getsettings(opt, changes, quantity=quantity)
                    obj = getroot.getobjectfromtree(quantity, ffile, settings)

                    flavours.append(obj.GetMean())
                    print "   Fraction of %s in %s zone: %1.3f" % (label, enriched,
                                                                 obj.GetMean())

            # get the response
            settings = plotbase.getsettings(opt, changes, quantity=response)
            obj = getroot.getobjectfromtree(response_local, ffile, settings)
            mean.append(obj.GetMean())
            mean_error.append(obj.GetMeanError())
            print "  Response in %s zone: %1.3f" % (enriched, obj.GetMean())
        
            if zones_extended.index(zone) < 4:
                if name == 'mc':
                    flavours_all.append(flavours[:4])
            if zones_extended.index(zone) < 4:
                mean_all.append(mean)
                mean_error_all.append(mean_error)

        # also plot the raw response for each zone
        mean = [i[0] for i in mean_all]
        mean_error = [i[0] for i in mean_error_all]
        ax_raw.errorbar(range(5)[1:], mean, mean_error, drawstyle='steps-mid',
                    color=c, fmt=m, capsize=0 ,label=name)

        # from response and composition, determine the 
        # response for the truth flavours
        yerr = []
        for n, flavour in enumerate(zonelabels):
            a = np.array(flavours_all)
            b = np.array(mean_all)

            y = np.linalg.solve(a, b)
            a = np.linalg.inv(a)

            # for each flavour, vary the response up and calculate again 
            # (to determine the error on the final response value)
            mean_all[n][0] += mean_error_all[n][0]
            y_varied = np.linalg.solve(a, b)
            mean_all[n][0] -= mean_error_all[n][0]

            print "  Response %s %1.3f +/- %1.3f" % (flavour, y[n], 
                                                    abs(y_varied[n]-y[n]))
            yerr.append(abs(y_varied[n]-y[n]))
            if name == 'mc':
                y_mc = copy.deepcopy(y)
        
        y = [i[0] for i in y]
        yerr = [i[0] for i in yerr]
        ax.errorbar(range(5)[1:], y, yerr, drawstyle='steps-mid', color=c, fmt=m, 
                      capsize=0 ,label=name)

    # also add the MC truth values
    y = []
    yerr = []
    for selection in selections:
        changes = {'selection':selection}
        settings = plotbase.getsettings(opt, changes, quantity=response)
        obj = getroot.getobjectfromtree(response_local, files[0], settings)
        y.append(obj.GetMean())
        yerr.append(obj.GetMeanError())
    ax.errorbar(range(5)[1:], y, yerr, drawstyle='steps-mid', color='blue', fmt='o', 
                      capsize=0 ,label='MC TruthFlavour')

    # set the axis labels and limits
    settings = plotbase.getsettings(opt, {'legloc':'lower right'}, quantity = '_'.join([response, flavourdef]))

    labels_enriched = ['%s-\nenriched' % s for s in labels]
    for ax_obj, l, xlabel in zip([ax, ax_raw], [labels, labels_enriched], ['tagflavour', 'zone']):
        plotbase.labels(ax_obj, opt, settings, settings['subplot'])
        plotbase.axislabels(ax_obj, xlabel, settings['xynames'][1], settings=settings)
        plotbase.setaxislimits(ax_obj, settings)
        ax_obj.set_xlim(0, 5)
        ax_obj.set_xticks(range(5)[1:])
        ax_obj.set_xticklabels(l)
        ax_obj.set_ylim(0.85, 1.1)
        ax_obj.axhline(1.0, color='black', linestyle=':')

    settings['filename'] = plotbase.getdefaultfilename("flavour_response_tagged", opt, settings)
    if PFcorrection: settings['filename'] += '_corrected'
    plotbase.Save(fig, settings['filename'], opt)

    del settings['filename']
    settings['filename'] = plotbase.getdefaultfilename("flavour_response_raw-zones", opt, settings)
    if PFcorrection: settings['filename'] += '_corrected'
    plotbase.Save(fig_raw, settings['filename'], opt)

    files.reverse()

