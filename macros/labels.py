import plotbase
import copy
from dictionaries import d_axes, d_plots

def labels(ax, opt=None, jet=False, bin=None, result=None, legloc='upper right',
           frame=True, sub_plot=False, changes={}, ratiosubplot=False, mc='False', color='black', energy_label=True):
    """This function prints all labels and captions in a plot.

    Several functions are called for each type of label.
    """
    if not ratiosubplot:
        if opt.lumi is not None and mc is not True:
            lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
        statuslabel(ax, opt.status)
        if opt.energy is not None and energy_label==True:
            energylabel(ax, opt.energy)
        #if jet==True:  jetlabel(ax, changes, sub_plot)    # on demand
        if changes.has_key('var') or changes.has_key('bin'): binlabel(ax, bin, changes=changes, color=color)
        if 'incut' in changes: incutlabel(ax, color, changes['incut'])
        resultlabel(ax, result)
        authorlabel(ax, opt.author)
        datelabel(ax, opt.date)
    if legloc is not False:
        legend = ax.legend(loc=legloc, numpoints=1, frameon=frame, fancybox=True, shadow=True)
    return ax


def incutlabel(ax, color='black', incut=''):
    if incut == 'allevents':
        text = r"(before cuts)"
    elif incut == 'zcutsonly':
        text = r"(only $\mu$ and Z cuts applied)"
    elif incut == 'alleta':
        text = r"(jet 1 all $\eta$)"
    else:
        return
    ax.text(0.97, 0.97, text, va='top', ha='right', transform=ax.transAxes, color=color)
    return ax

def eventnumberlabel(ax, opt, events):
    text=""
    for f, l in zip(events, opt.labels):
        text += "\n" + l + " Events: " + str("%1.1e"% f)
    ax.text(0.7,1.01, text, size='xx-small', va='bottom', ha='right',transform=ax.transAxes)
        

def lumilabel(ax, lumi=0.0, xpos=0.00, ypos=1.01):
    if lumi >= 1000.0:
        ax.text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{fb}^{-1}$" %
            (lumi / 1000.0), va='bottom', ha='left', transform=ax.transAxes)
    elif lumi > 0.0:
        ax.text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{pb}^{-1}$" %
            (lumi), va='bottom', ha='left', transform=ax.transAxes)
    return ax


def energylabel(ax, energy, xpos=1.00, ypos=1.01):
    if energy is not None:
        ax.text(xpos, ypos, r"$\sqrt{s} = %u\,\mathrm{TeV}$" % (energy),
            va='bottom', ha='right', transform=ax.transAxes)

def jetlabel_string( changes, opt):
    if changes.has_key('algorithm'): algorithm = changes['algorithm']
    else: algorithm = opt.algorithm
    if changes.has_key('correction'): correction = changes['correction']
    else: correction = opt.correction
    if "L1L2L3Res" in correction:
        corr = r"L1L2L3 Residual corrected"
    elif "L1L2L3" in correction:
        corr = r"L1L2L3 corrected"
    elif "L1L2" in correction:
        corr = r"L1L2 corrected"
    elif "L1" in correction:
        corr = r"L1 corrected"
    else:
        corr = r"uncorrected"
    if "ak5pfjets" in algorithm.lower():
        jet = r"Anti-$k_{T}$ 0.5 PF jets"
    elif "ak7pfjets" in algorithm.lower():
        jet = r"Anti-$k_{T}$ 0.7 PF jets"
    else:
        jet = ""
        corr = ""
    return ( jet , corr )

def jetlabel(ax, changes={}, sub_plot=False, posx=0.05, posy=0.95, opt=None):
    res = jetlabel_string(changes,opt)
    
    if sub_plot: col = 'red'
    else: col='black'
    
    #if "AK5" not in opt.algorithm: ax.text(posx, posy, res[0], va='top', ha='left', transform=ax.transAxes)
    #else: 
    posy=posy+0.07
    if changes.has_key('correction'):
        ax.text(posx, posy-0.07, res[1], va='top', ha='left', transform=ax.transAxes, color=col)
            
    if changes.has_key('algorithm'):
        if "CHS" in changes['algorithm']:
            ax.text(posx, posy - 0.14, r"CHS applied", va='top', ha='left', transform=ax.transAxes, color=col)
        #if "CHS" not in changes['algorithm']:
        #    ax.text(posx, posy - 0.07, r"CHS not applied", va='top', ha='left', transform=ax.transAxes, color=col)

    return ax


def authorlabel(ax, author=None, xpos=0.01, ypos=1.10):
    if author is not None:
        ax.text(xpos, ypos, author, va='top', ha='left',
                transform=ax.transAxes)
    return ax


def datelabel(ax, date='iso', xpos=0.99, ypos=1.10):
    if date is None:
        return ax
    if date == 'now':
        ax.text(xpos, ypos, strftime("%d %b %Y %H:%M", localtime()),
        va='top', ha='right', transform=ax.transAxes)
    elif date == 'today':
        ax.text(xpos, ypos, strftime("%d %b %Y", localtime()),
        va='top', ha='right', transform=ax.transAxes)
    elif date == 'iso':
        ax.text(xpos, ypos, strftime("%Y-%m-%d", localtime()),
        va='top', ha='right', transform=ax.transAxes)
    else:
        ax.text(xpos, ypos, date, va='top', ha='right', transform=ax.transAxes)
    return ax


def binlabel(ax, bin=None, low=0, high=0, xpos=0.03, ypos=0.95, changes={}, color='black'):
    ch_copy = copy.deepcopy(changes)
    if bin is None:
        if 'var' in ch_copy and 'Cut' in ch_copy['var'] and len(ch_copy['var']) > 35:
            ch_copy['var'] = 'var'+ch_copy['var'].split('var')[2]
        if 'bin' in ch_copy:
            ranges = ch_copy['bin'][2:].split('to')
            bin= 'ptz'
            low = int(ranges[0])
            high = int(ranges[1])
        elif 'var' in ch_copy and 'Eta' in ch_copy['var']:
            ranges = ch_copy['var'][11:].replace('_','.').split('to')
            bin = 'eta'
            low = float(ranges[0])
            high = float(ranges[1])
        elif 'var' in ch_copy and 'PtBin' in ch_copy['var']:
            ranges = ch_copy['var'][10:].replace('_','.').split('to')
            bin = 'ptbin'
            low = float(ranges[0])
            high = float(ranges[1])   
        elif 'var' in ch_copy and 'Npv' in ch_copy['var']:
            ranges = ch_copy['var'][8:].split('to')
            bin = 'Npv'
            low = int(ranges[0])
            high = int(ranges[1])
        elif 'var' in ch_copy and 'Cut' in ch_copy['var']:
            ranges = ch_copy['var'][27:].replace('_','.')
            bin ='alpha'
            low = float(ranges)
        else:
            return ax
    if bin == 'ptz':
        text = r"$%u < p_\mathrm{T}^\mathrm{Z} / \mathrm{GeV} < %u$" % (low, high)
    elif bin == 'pthat':
        text = r"$%u < \hat{p}_\mathrm{T} / \mathrm{GeV} < %u$" % (low, high)
    elif bin == 'eta':
        if low == 0:
            text = r"$|\eta_\mathrm{jet}| < %1.3f$" % (high)
        else:
            text = r"$%1.3f < |\eta_\mathrm{jet}| < %1.3f$" % (low, high)
    elif bin == 'alpha':
        text = r"$ \alpha < %1.2f$" % (low)
    elif bin == 'ptbin':
        #text = r"%1.0f < $|\pt_\mathrm{jet}|$ < %1.0f" % (low, high)
        text = r"%1.0f GeV < $p_T^\mathrm{Jet}$ < %1.0f GeV" % (low, high)
    elif bin == 'Npv':
        if low == 0:
            text = r"$ NPV \leq %u$" % (high)
        else:
            text = r"$%u \leq NPV \leq %u$" % (low, high)
    else:
        text = bin
    ax.text(xpos, ypos, text, va='center', ha='left', size='x-large', transform=ax.transAxes, color=color )


def statuslabel(ax, status=None, xpos=0.25, ypos=1.018):
    if status is not None:
        ax.text(xpos, ypos, r"%s" % status, va='bottom', ha='left',
                transform=ax.transAxes)


def resultlabel(ax, text=None, xpos=0.05, ypos=0.05):
    if text is not None:
        if "\n" in text:
            # two lined thingy
            ypos += 0.07
        ax.text(xpos, ypos, text, va='top', ha='left', transform=ax.transAxes)


def axislabel_2d(ax, y_q, y_obj, x_q='pt', x_obj='Z', brackets=False):
    print "Please use axislabels instead of axislabel_2d."
    return axislabels(ax, x_q, y_q, brackets)

def axislabels(ax, x='z_pt', y='events', brackets=False, labels=['','']):
    """same as the old version, but can handle and and y axis indpendetly

       new idea: improve automatic scaling:
       bottom=0
       top
       autoscaling
       dict = { quantity: axsetting}
    """

    def setxaxis(limits=(0, 200), quantity="x", unit=""):
        ax.set_xlabel(unitformat(quantity, unit, brackets), ha="right", x=1)
        ax.set_xlim(limits)

    def setyaxis(limits=(0, 1), quantity="y", unit="", bottom=None):
        ax.set_ylabel(unitformat(quantity, unit, brackets), va="top", y=1)
        if bottom is not None:
            ax.set_ylim(bottom=bottom)
        else:
            ax.set_ylim(limits)

    for quantity, function in zip([x,y],[[setxaxis, ax.set_xticks, ax.set_xticklabels], [setyaxis, ax.set_yticks, ax.set_yticklabels]]):
    # special formatting options for some quantities ...
        if 'phi' in quantity:
            function[1]([-3.14159265, -1.57079633, 0.0, 1.57079633, 3.14159265])
            function[2]([r"$-\pi$", r"$-\frac{\pi}{2}$", r"$0$", r"$\frac{\pi}{2}$", r"$\pi$"])
            if 'deltaphi' in quantity:
                function[0]((d_axes['deltaphi'][0], d_axes['deltaphi'][1]), d_axes['deltaphi'][2] % (plotbase.nicetext(quantity.replace("deltaphi-","").split("-")[0]), 
                        plotbase.nicetext(quantity.replace("deltaphi-","").split("-")[1])), d_axes['deltaphi'][3]) 
            elif 'phiabsdiff' in quantity:
                function[0]((d_axes['phiabsdiff'][0], d_axes['phiabsdiff'][1]), d_axes['phiabsdiff'][2] % plotbase.nicetext(quantity.replace("phiabsdiff","")),
                        d_axes['phiabsdiff'][3]) 
            elif 'abs' in quantity:
                function[0]((d_axes['absphi'][0], d_axes['absphi'][1]), d_axes['absphi'][2] % 
                        plotbase.nicetext(quantity.replace("abs_","").replace("_phi","")) , d_axes['absphi'][3]) 
            else:
                function[0]((d_axes['phi'][0], d_axes['phi'][1]), d_axes['phi'][2] % plotbase.nicetext(quantity.replace("phi","")) , d_axes['phi'][3]) 
        elif 'eta' in quantity:
            if 'deltaeta' in quantity:
                function[0]((d_axes['deltaeta'][0], d_axes['deltaeta'][1]), d_axes['deltaeta'][2] % (plotbase.nicetext(quantity.replace("deltaeta-","").split("-")[0]),
                         plotbase.nicetext(quantity.replace("deltaeta-","").split("-")[1])), d_axes['deltaeta'][3]) 
            elif 'etaabsdiff' in quantity:
                function[0]((d_axes['deltaeta'][0], d_axes['deltaeta'][1]), d_axes['deltaeta'][2] % plotbase.nicetext(quantity.replace("etaabsdiff","")) , d_axes['deltaeta'][3]) 
            elif 'abseta' in quantity:
                function[0]((d_axes['abseta'][0], d_axes['abseta'][1]), d_axes['abseta'][2] % plotbase.nicetext(quantity.replace("abseta","")) , d_axes['abseta'][3]) 
            else:
                function[0]((d_axes['eta'][0], d_axes['eta'][1]), d_axes['eta'][2] % plotbase.nicetext(quantity.replace("eta","")) , d_axes['eta'][3]) 
        elif 'deltar' in quantity:
            function[0]((d_axes['deltar'][0], d_axes['deltar'][1]), d_axes['deltar'][2] % (plotbase.nicetext(quantity.replace("deltar-","").split("-")[0]), 
                    plotbase.nicetext(quantity.replace("deltar-","").split("-")[1])), d_axes['deltar'][3]) 
        elif 'events' == quantity:
            function[0](bottom=0.0, quantity="Events")
        elif 'cut' in quantity:
            function[0]((d_axes['cut'][0], d_axes['cut'][1]), d_axes['cut'][2] % plotbase.nicetext(quantity.replace("cut-","")), d_axes['cut'][3]) 
        elif 'fraction' in quantity and 'MET' not in quantity:
            function[0]((d_axes[quantity[4:-8]][0], d_axes[quantity[4:-8]][1]), d_axes[quantity[4:-8]][2] % plotbase.nicetext(quantity[:4]), d_axes[quantity[4:-8]][3])
        elif quantity == 'ratio':
            function[0]((d_axes['ratio'][0], d_axes['ratio'][1]), d_axes['ratio'][2] % (labels[0], labels[1]), d_axes['ratio'][3]) 
        elif 'filters' in quantity:
            function[1](range(13))
            function[2](["ECAL DeadCellB", "ECAL DeadCellT", "Beam Halo", "Beam Scraping", "ECAL Laser", "ECAL Supercrystal", "ECAL Noise", "Muon Greedy", "HCAL Laser", "HCAL Noise", "Muon Inconsistent", "Tracking Failure", "All Filters"], rotation = 90)
            function[0]((d_axes[quantity][0], d_axes[quantity][1]), d_axes[quantity][2], d_axes[quantity][3])
        elif quantity in d_axes:     # if no special options, read from dictionary
            function[0]((d_axes[quantity][0], d_axes[quantity][1]), d_axes[quantity][2], d_axes[quantity][3])
        else:
            print '"'+quantity + '" is not defined and therefore directly written to label.'
            function[0](quantity=quantity)
    return ax

def getaxislabels_list(quantity, ax=None):
# can we integrate this function somehow into axislabels??
# currently we need one function to change a given ax elemnt and one to simply return limits+label

    # lower limit, upper limit, label, unit
    if 'phi' in quantity:
        if 'deltaphi' in quantity:
           labels_list = [d_axes['deltaphi'][0], d_axes['deltaphi'][1], d_axes['deltaphi'][2] % (plotbase.nicetext(quantity.replace("deltaphi-","").split("-")[0]), 
                        plotbase.nicetext(quantity.replace("deltaphi-","").split("-")[1])), d_axes['deltaphi'][3]]
        elif 'phiabsdiff' in quantity:
             labels_list = [d_axes['phiabsdiff'][0], d_axes['phiabsdiff'][1], d_axes['phiabsdiff'][2] % plotbase.nicetext(quantity.replace("phiabsdiff","")),
                        d_axes['phiabsdiff'][3]]
        elif 'abs' in quantity:
            labels_list = [d_axes['absphi'][0], d_axes['absphi'][1], d_axes['absphi'][2] % 
                        plotbase.nicetext(quantity.replace("abs_","").replace("_phi","")) , d_axes['absphi'][3]]
        else:
                labels_list = [d_axes['phi'][0], d_axes['phi'][1], d_axes['phi'][2] % plotbase.nicetext(quantity.replace("phi","")) , d_axes['phi'][3]]
    elif  quantity.endswith('eta'):
        if 'deltaeta' in quantity:
            labels_list = [d_axes['deltaeta'][0], d_axes['deltaeta'][1], d_axes['deltaeta'][2] % (plotbase.nicetext(quantity.replace("deltaeta-","").split("-")[0]),
                         plotbase.nicetext(quantity.replace("deltaeta-","").split("-")[1])), d_axes['deltaeta'][3]]
        elif 'etaabsdiff' in quantity:
            labels_list = [d_axes['deltaeta'][0], d_axes['deltaeta'][1], d_axes['deltaeta'][2] % plotbase.nicetext(quantity.replace("etaabsdiff","")) , d_axes['deltaeta'][3]]
        elif 'abseta' in quantity:
            labels_list = [d_axes['abseta'][0], d_axes['abseta'][1], d_axes['abseta'][2] % plotbase.nicetext(quantity.replace("abseta","")) , d_axes['abseta'][3]]
        else:
            labels_list = [d_axes['eta'][0], d_axes['eta'][1], d_axes['eta'][2] % plotbase.nicetext(quantity.replace("eta","")) , d_axes['eta'][3]]
    elif 'deltar' in quantity:
        labels_list = [d_axes['deltar'][0], d_axes['deltar'][1], d_axes['deltar'][2] % (plotbase.nicetext(quantity.replace("deltar-","").split("-")[0]), 
                    plotbase.nicetext(quantity.replace("deltar-","").split("-")[1])), d_axes['deltar'][3]]
    elif 'fraction' in quantity and 'MET' not in quantity:
        labels_list = [d_axes[quantity[4:-8]][0], d_axes[quantity[4:-8]][1], d_axes[quantity[4:-8]][2] % plotbase.nicetext(quantity[:4]), d_axes[quantity[4:-8]][3]]
    elif quantity in d_axes:
        labels_list = [d_axes[quantity][0], d_axes[quantity][1], d_axes[quantity][2] , d_axes[quantity][3]]
    else:
        labels_list =  [0,1,quantity, ""]
    return labels_list


def unitformat(quantity="", unit="", brackets=False):
    """Returns a string according to SI standards

       (r"$p_\mathrm{T}$", "GeV") yields "$p_\mathrm{T}$ / GeV"
       brackets are not SI!
       "quantity|unit"
    """

    if unit != "":
        if "/" in quantity:
            quantity = "(%s)" % quantity
        if "/" in unit:
            unit = "(%s)" % unit
        if brackets:        # units with [] (not allowed by SI system!)
            quantity = r"%s [%s]" % (quantity, unit)
        else:                # units with /
           quantity = r"%s / %s" % (quantity, unit)
    #print "The axis legend string is:", repr(quantity)
    return quantity
