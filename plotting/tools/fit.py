import getroot

import numpy as np
import array
import ROOT
import numpy
import math
import matplotlib.mlab as mlab

def fit(ax, quantity, rootobject, settings, color='black', label="", index=0, 
                                                                scalefactor=1, offset=None):
    """One of several fits is added to an axis element, fit parameters are added as text element."""
    #TODO: Review this function! Find a better design approach

    if color in ['#CBDBF9', 'lightskyblue']: color = 'blue'

    limits = settings['x']

    if settings['fit']=='vertical':
        if quantity == 'METpt': unit = r' \/ / \/ GeV'
        else: unit = ""

        if 'fitlabel_offset' in settings:
            offset += settings['fitlabel_offset']

        ax.axvline(rootobject.GetMean(), color=color, linestyle='-')
        ax.axvspan(rootobject.GetMean()-rootobject.GetMeanError(), rootobject.GetMean()+rootobject.GetMeanError(), color=color, alpha=0.1)
        ax.text(0.5, 0.97-(index/20.)+offset, r"$\langle \mathrm{%s} \rangle = %1.3f\pm%1.3f" % (label, rootobject.GetMean(), rootobject.GetMeanError())+"\mathrm{%s}$" % unit,
               va='top', ha='right', transform=ax.transAxes, color=color)

        ax.text(0.97, 0.97-(index/20.)+offset, r"$\mathrm{RMS (%s)} = %1.3f$" % (label, rootobject.GetRMS()), va='top', ha='right', transform=ax.transAxes, color=color)

    elif settings['fit'] == 'chi2':
        # fit a horizontal line
        intercept, ierr, chi2, ndf = fitline(rootobject, limits)
        #account for scaling and rebinning:
        intercept = scalefactor * intercept
        ax.axhline(intercept, color=color, linestyle='--')
        ax.axhspan(intercept+ierr, intercept-ierr, color=color, alpha=0.2)

        # and display chi^2
        ax.text(0.97, 0.25-(index/10.), r"y average: %1.3f$\pm$%1.3f" % (intercept, ierr),
        va='top', ha='right', transform=ax.transAxes, color=color)
        ax.text(0.97, 0.20-(index/10.), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
        va='top', ha='right', transform=ax.transAxes, color=color)
         

    elif settings['fit'] == 'gauss' :
        p0, p0err, p1, p1err, p2, p2err, chi2, ndf, conf_intervals = fitline2(rootobject, gauss=True, limits=limits)
        x=numpy.linspace(limits[0], limits[1], 500)
        ymax = max(mlab.normpdf(x, p1, p2))

        ax.plot(x,scalefactor * p0 / ymax * mlab.normpdf(x, p1, p2), color = color)

        ax.text(0.03, 0.97-0.06*(len(settings['files']) - index - 1)+offset, r"$\mathrm{%s:}$" % label,
               va='top', ha='left', transform=ax.transAxes, color=color)
        ax.text(0.25, 0.97-0.06*(len(settings['files']) - index - 1)+offset, r"$\mu = %1.3f\pm%1.5f$" % (p1, p1err),
               va='top', ha='left', transform=ax.transAxes, color=color)
        ax.text(0.65, 0.97-0.06*(len(settings['files']) - index - 1)+offset, r"$\sigma = %1.3f\pm%1.5f$" % (p2, p2err),
               va='top', ha='left', transform=ax.transAxes, color=color)
        if 'fitvalues' not in settings:
            settings['fitvalues'] = [[p1, p1err, p2, p2err]]
        else:
            settings['fitvalues'] += [[p1, p1err, p2, p2err]]
    elif settings['fit'] == 'bw':
        p0, p0err, p1, p1err, p2, p2err, chi2, ndf, conf_intervals = fitline2(rootobject, breitwigner=True, limits=limits)
        par = [p0, p1, p2]
        x=numpy.linspace(limits[0], limits[1], 500)
        y = [scalefactor * bw([i], par) for i in x]
        ax.plot(x, y, color = color)

        for x, text in zip([0.03, 0.25, 0.65], [r"$\mathrm{%s:}$" % label.replace("_", "\ "),
        r"$\mathrm{mean} = %1.2f\pm%1.2f$" % (p1, p1err),
        r"$\mathrm{RMS} = %1.2f\pm%1.2f$" % (p2, p2err),
        ]):
            ax.text(x, 0.92-0.06*(len(settings['files']) - index - 1)+offset, text,
               va='bottom', ha='left', transform=ax.transAxes, color=color)

    else:
        intercept, ierr, slope, serr,  chi2, ndf, conf_intervals = fitline2(rootobject)
        mean = rootobject.GetMean()
        intercept -= offset

        # fit line:
        line_fit = ax.plot(limits[:2],[(intercept+limits[0]*slope)*scalefactor, 
            (intercept+limits[1]*slope)*scalefactor], color = color, linestyle='--')

        # insert a (0, 0) bin because the conf_intervals list also contains an additional (0., conf)-point
        plot = getroot.root2histo(rootobject)
        plot.xc.insert(0, 0.)

        # display confidence intervals
        ax.fill_between(plot.xc, 
              [(intercept+x*slope + c)*scalefactor for x, c in zip(plot.xc, conf_intervals)],
              [(intercept+x*slope - c)*scalefactor for x, c in zip(plot.xc, conf_intervals)], 
              facecolor=color, edgecolor=color, interpolate=True, alpha=0.2)

        if 'fitlabel_offset' in settings:
            offset += settings['fitlabel_offset']

        #display slope:
        if settings['fit'] == 'slope':
            if 'fit_slope_exponent' in settings:
                fit_slope_exponent = settings['fit_slope_exponent']
            else:
                try:
                    fit_slope_exponent = int(math.log10(abs(slope)))-1
                    settings['fit_slope_exponent'] = fit_slope_exponent                
                except:
                    return
            ax.text(0.97, 0.95-(index/10.)+offset, r"$\mathrm{Fit\/slope} = (%1.2f\pm%1.2f) \times 10^{%g}$" % (slope/(10**fit_slope_exponent), serr/(10**fit_slope_exponent), fit_slope_exponent),
               va='top', ha='right', transform=ax.transAxes, color=color,
               size='x-large')
        elif settings['fit'] == 'intercept':
            #display intercept ...
            ax.text(0.97, 0.35-(index/10.)+offset, r"$\mathrm{y(0)} = %1.3f\pm%1.3f$" % (intercept, ierr),
               va='top', ha='right', transform=ax.transAxes, color=color, size='x-large')

            # ... and chi2 (smaller)
            ax.text(0.97, 0.30-(index/10.)+offset, r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
                va='top', ha='right', transform=ax.transAxes, color=color, size='small')
        elif settings['fit'] == 'chi2_linear':
            # display chi2
            ax.text(0.97, 0.20-(index/10.)+offset, r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
                va='top', ha='right', transform=ax.transAxes, color=color, size='x-large')


def fitline(rootgraph, limits=[0, 1]):
    if 'Profile' in rootgraph.ClassName():
        rootgraph.Approximate() # call this function so zero-error (one entry) bins don't distort the fit
    fitf = ROOT.TF1("fit1", "1*[0]", *limits)
    fitres = rootgraph.Fit(fitf,"SQN")
    if 'Profile' in rootgraph.ClassName():
        rootgraph.Approximate(0)
    return (fitf.GetParameter(0), fitf.GetParError(0), fitres.Chi2(), fitres.Ndf())

def fitline2(rootgraph, quadratic=False, gauss=False, breitwigner=False, limits = [0, 1000]):
    if 'Profile' in rootgraph.ClassName():
        rootgraph.Approximate() # call this function so zero-error (one entry) bins don't distort the fit
    if breitwigner:
        f =bw
        fitf = ROOT.TF1("fitf", f, limits[0], limits[1], 3)
        fitf.SetParameters(10, 10, 10, 10)
    else:
        if quadratic:
            f = "1*[0]+x*[1]+x*x*[2]"
        elif gauss:
            f = "gaus"
        else:
            f = "1*[0]+x*[1]"
        fitf = ROOT.TF1("fitf", f, *limits)
    fitres = rootgraph.Fit(fitf,"SQN")
    #Get conf intervals as an array and then convert to list
    points = []
    for n in range(1, rootgraph.GetSize() - 1):
        points.append(rootgraph.GetBinCenter(n))
    #add additional points at the beginning and end to extend the fit error bands
    points.insert(0, 0.)
    points.append(2*points[-1]-points[-2])
    x = array.array('d', points)
    y = array.array('d', [0.]*len(points))
    fitres.GetConfidenceIntervals(len(points), 1, 1, x, y, 0.683)
    conf_intervals = [i for i in y]

    if quadratic or gauss or breitwigner:
        return (fitf.GetParameter(0), fitf.GetParError(0),
            fitf.GetParameter(1), fitf.GetParError(1),
            fitf.GetParameter(2), fitf.GetParError(2),
            fitres.Chi2(), fitres.Ndf(), conf_intervals)
    else:
        return (fitf.GetParameter(0), fitf.GetParError(0),
            fitf.GetParameter(1), fitf.GetParError(1),
            fitres.Chi2(), fitres.Ndf(), conf_intervals)

def bw(x, par):
    """Breit-Wigner function. """
    arg1 = 14.0/22.0 # 2 over pi
    arg2 = par[2]*par[2]*par[1]*par[1] #Gamma=par[1]  M=par[2]
    arg3 = ((x[0]*x[0]) - (par[1]*par[1]))*((x[0]*x[0]) - (par[1]*par[1]))
    arg4 = x[0]*x[0]*x[0]*x[0]*((par[2]*par[2])/(par[1]*par[1]))
    #x = array.array('d', par[0]*arg1*arg2/(arg3 + arg4))
    return par[0]*arg1*arg2/(arg3 + arg4)







