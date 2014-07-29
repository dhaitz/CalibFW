# -*- coding: utf-8 -*-
""" interaction with ROOT

    file open, reading of histograms and converting to a Histo class
    as well as the resp_cuts file format specific functions.

    Here is an overview of the most interesting functions besides 'openfile':
    quantity -------------------------getplotfromnick------------------> plot
                              name ----------getplot-------------------> plot
    quantity -getobjectname-> name -getobject-> rootobject -root2histo-> plot
    quantity ------getobjectfromnick----------> rootobject

    quantity:   Short name/nick name (expanded with default values) e.g. z_pt
    name:       name of root object in file, e.g. NoBinning_incut/z_pt_AK5PFJetsL1L2L3
    rootobject: root object (TH1D, TGraphErrors, etc.)
    plot:      getroot Histo class object

    getobjectname expands a short name with default values to the object name in the root file
    getobject yields a root object
    getplot yields a getroot Histo

"""
import ROOT
import cPickle as pickle
import time
import copy
import math
import os
import array
import numpy as np
import plotbase
from labels import getaxislabels_list as getaxis
from dictionaries import ntuple_dict


ROOT.PyConfig.IgnoreCommandLineOptions = True  # prevents Root from reading argv


def ptcuts(ptvalues):
    return ["{0}<=zpt && zpt<{1}".format(*b) for b in zip(ptvalues[:-1], ptvalues[1:])]


def npvcuts(npvvalues):
    return ["{0}<=npv && npv<={1}".format(*b) for b in npvvalues]


def etacuts(etavalues):
    return ["{0}<=jet1abseta && jet1abseta<{1}".format(*b) for b in zip(etavalues[:-1], etavalues[1:])]


def alphacuts(alphavalues):
    return ["alpha<%1.2f" % b for b in alphavalues]


def openfile(filename, verbose=False, exitonfail=True):
    """Open a root file"""
    f = ROOT.TFile(filename)
    try:
        if not f or not f.IsOpen():
            print "Can't open file:", filename
            if exitonfail:
                exit(1)
    except:
        print "Can't open file:", filename
        if exitonfail:
            exit(1)
    if verbose:
        print " * Inputfile:", filename
    return f

try:
    openfile = profile(openfile)
except NameError:
    pass  # not running with profiler


def getplot(name, rootfile, changes=None, rebin=1):
    rootobject = getobject(name, rootfile, changes)
    return root2histo(rootobject, rootfile.GetName(), rebin)


def getplotfromnick(nickname, rootfile, changes, settings):
    objectname = getobjectname(nickname, changes)
    rootobject = getobject(objectname, rootfile, changes)
    return root2histo(rootobject, rootfile.GetName(), rebin)


def getplotfromtree(nickname, rootfile, settings, twoD=False, changes=None):
    rootobject = histofromfile(nickname, rootfile, settings, twoD=False, changes=None)
    histo = root2histo(rootobject, rootfile.GetName(), settings['rebin'])
    rootobject.Delete()
    return histo


#def gethistofromtree(name, tree, settings, changes=None, twoD=False):


def gettreename(settings, parts=['folder', 'algorithm', 'correction'], string="%s_%s%s"):
    """naming scheme

    format the treename from its parts with the format of string
    """
    for p in parts:
        if p not in settings:
            print p, "is not in settings."
            exit(1)
    part = [settings[p] for p in parts]
    name = string % tuple(part)
    return name


def getselection(settings, mcWeights=False, index=0):
    # create the final selection from the settings
    selection = []
    if settings['folder'] == 'incut' and not settings['allalpha']:
        selection += ["(jet2pt/zpt < 0.2)"]  # || jet2pt<12
    if settings['folder'] == 'incut' and not settings['alleta']:
        selection += ["(abs(jet1eta) < 1.3)"]
    if settings['selection']:
        if len(settings['selection']) > index:
            selection += [settings['selection'][index]]
        else:
            selection += [settings['selection'][0]]

    # add weights
    weights = []
    if selection:
        weights = ["(" + " && ".join(selection) + ")"]
    if mcWeights and ('noweighting' not in settings or settings['noweighting']):
        weights += ["weight"]
    if mcWeights:  # add lumi weights always?
        weights += [str(settings['lumi'])]
    if settings['channel'] is 'mm':
        if mcWeights and settings['efficiency']:
            weights += [str(settings['efficiency'])]
        if mcWeights and settings['factor']:
            weights += [str(settings['factor'])]
    selectionstr = " * ".join(weights)

    #create a copy of quantities to iterate over (to replace the from the dict):
    for key in ntuple_dict.keys():
        if key in selectionstr:
            selectionstr = selectionstr.replace(key, dictconvert(key))

    return selectionstr


def getbinning(quantity, settings, axis='x'):
    #missing: guess range from first entries (instead of 0,1)
    # variants: special_binning, float, int, log,
    xmin = settings[axis][0]
    xmax = settings[axis][1]
    nbins = settings['nbins']

    if nbins < 0:
        pass  # set it automatically
    if quantity in ['npv', 'npu', 'jet1nconst']:  # integer binning
        nbins = int(xmax - xmin)

    bins = [xmin + (xmax - xmin) * float(i) / nbins for i in range(nbins + 1)]
    #TODO: better log binning also for y etc.
    #print settings['xlog']
    if settings['xlog']:
        print "LOG bins is not done -> getroot.getbinning"
        xmin = max(xmin, 1.0)
        print xmin, nbins
        #bins = [xmin * (float(xmax) / xmin ) ** (float(i) / nbins) for i in range(nbins + 1)]

    #special binning for certain quantities:
    # No, opt is the wrong place, -> dict
    bin_dict = {
        'zpt': settings['zbins'],
        'jet1abseta': settings['eta'],
        'jet1eta': [-elem for elem in settings['eta'][1:][::-1]] + settings['eta'],
        'npv': [a - 0.5 for a, b in settings['npv']] + [settings['npv'][-1][1] - 0.5]
    }

    if settings['special_binning'] and quantity in bin_dict:
        bins = bin_dict[quantity]

    print "Binning of", axis, ":", nbins, "bins from", xmin, "to", xmax, "for", quantity
    return array.array('d', bins)


def histofromntuple(quantities, name, ntuple, settings, twoD=False, index=0):
    xbins = getbinning(quantities[-1], settings)
    if twoD and len(quantities) > 1:
        ybins = getbinning(quantities[-2], settings, 'y')

    if settings['xaxis'] is not None and len(settings['xaxis']) > index:
        quantities[0] = settings['xaxis'][index]
    if settings['yaxis'] is not None and len(settings['yaxis']) > index:
        quantities[0] = settings['yaxis'][index]

    copy_of_quantities = quantities
    for key in ntuple_dict.keys():
        for quantity, i in zip(copy_of_quantities, range(len(copy_of_quantities))):
            if key in quantity:
                quantities[i] = quantities[i].replace(key, dictconvert(key))
    #TODO with TTree UserInfo: http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=16902
    isMC = bool(ntuple.GetLeaf("npu"))

    variables = ":".join(quantities)
    selection = getselection(settings, isMC, index=index)

    if settings['verbose']:
        plotbase.debug("Creating a plot with the following selection:\n   %s" % settings['selection'])

    # determine the type of histogram to be created
    if len(quantities) == 1:
        roothisto = ROOT.TH1D(name, name, len(xbins) - 1, xbins)
    elif len(quantities) == 2 and not twoD:
        roothisto = ROOT.TProfile(name, name, len(xbins) - 1, xbins)
    elif len(quantities) == 2 and twoD:
        roothisto = ROOT.TH2D(name, name, len(xbins) - 1, xbins, len(ybins) - 1, ybins)
    elif len(quantities) == 3:
        roothisto = ROOT.TProfile2D(name, name, len(xbins) - 1, xbins, len(ybins) - 1, ybins)
    else:
        print "FATAL: could not determine histogram type from", quantities

    if settings['verbose']:
        plotbase.debug("Creating a %s with the following selection:\n   %s" % (roothisto.ClassName(), selection))

    # fill the histogram from the ntuple
    roothisto.Sumw2()
    print "Weights:", selection
    ntuple.Project(name, variables, selection)
    ntuple.Delete()

    if roothisto.ClassName() == 'TH2D':
        print "Correlation between %s and %s in %s in the selected range:  %1.5f" % (
            quantities[1], quantities[0], roothisto.GetName(),  # .split("/")[-3],
            roothisto.GetCorrelationFactor())

    #no no no! not here
    if settings.get('binroot', False):
        for n in range(roothisto.GetSize()):
            if roothisto.GetBinContent(n) > 0:
                a = roothisto.GetBinContent(n)
                b = math.sqrt(a)
                roothisto.SetBinContent(n, b)
                roothisto.SetBinEntries(n, 1)
    return roothisto


def histofromfile(quantity, rootfile, settings, changes=None, twoD=False, index=0):
    """This function returns a root object

    If quantity is an object in the rootfile it is returned.
    If not, the histo is filled from ntuple variables via the histofromntuple function
    """
    settings = plotbase.applyChanges(settings, changes)
    histo = objectfromfile(quantity, rootfile, warn=False)
    if histo:
        return histo

    treename = gettreename(settings)
    ntuple = objectfromfile(treename, rootfile)
    name = quantity + rootfile.GetName()
    name = name.replace("/", "").replace(")", "").replace("(", "")
    #rootfile.Delete("%s;*" % name)
    quantities = quantity.split("_")
    return histofromntuple(quantities, name, ntuple, settings, twoD=twoD, index=index)


try:
    getobjectfromtree = profile(getobjectfromtree)
except NameError:
    pass  # not running with profiler


def dictconvert(quantity):
    if quantity in ntuple_dict:
        return ntuple_dict[quantity]
    else:
        return quantity


def getobjectfromnick(nickname, rootfile, changes, rebin=1, selection=""):
    objectname = getobjectname(nickname, changes)
    return getobject(objectname, rootfile, changes, selection=selection)


def objectfromfile(name, rootfile, exact=False, warn=True):
    """get a root object by knowing the exact name

    'exact' could be used to enforce the loading of exactly this histogram and
    not the MC version without 'Res' but it is not at the moment (strict version)
    """
    oj = rootfile.Get(name)
    if not oj and "Res" in name and not exact:
        oj = rootfile.Get(name.replace("Res", ""))
    if warn and not oj:
        print "Can't load object", name, "from root file", rootfile.GetName()
        exit(0)
    return oj


def getobjectname(quantity='z_mass', change={}):
    """Build the name of a histogram according to Excalibur conventions.

    Every histogram written by Artus has a name like
    'NoBinning_incut/<quantity>_ak5PFJetsCHSL1L2L3'
    This string is returned for each quantity and any changes to the default
    can be applied via the change dictionary.

    Examples for quantities:
        z_pt, mu_plus_eta, cut_all_npv, L1_npv
    """
    # Set default values
    keys = ['bin', 'incut', 'var', 'quantity', 'algorithm', 'correction']
    selection = {'bin': 'NoBinning', 'incut': 'incut', 'var': '',
                 'quantity': '<quantity>', 'algorithm': 'AK5PFJetsCHS',
                 'correction': 'L1L2L3Res'}

    hst = ''
    for k in change:
        if k not in selection:
            print k, "is no valid key. Valid keys are: ", keys
            exit(1)
    # apply requested changes
    selection.update(change)
    # make a prototype name and correct it
    for k in keys:
        hst += selection[k] + '_'
    hst = hst.replace('Jets_', 'Jets').replace('__', '_').replace('_L1', 'L1')[:-1]
    hst = hst.replace('_<quantity>', '/' + quantity)
    return hst


def saveasroot(rootobjects, opt, settings):
    filename = opt.out + "/%s.root" % settings['filename']
    f = ROOT.TFile(filename, "UPDATE")
    for rgraph, name in zip(rootobjects, settings['labels']):
        plotname = settings['root']
        plotname = "_".join([settings['root'], name])
        print "Saving %s in ROOT-file %s" % (plotname, filename)
        rgraph.SetTitle(plotname)
        rgraph.SetName(plotname)
        rgraph.Write()
    f.Close()


def root2histo(histo, rootfile='', rebin=1):
    """Convert a root histogram to the Histo class"""
    hst = Histo()

    if not hasattr(histo, 'ClassName'):
        print histo, "is no TObject. It could not be converted."
        exit(0)
    # Detect if it is a histogram or a graph
    if histo.ClassName() == 'TH1D' or histo.ClassName() == 'TH1F' or \
            histo.ClassName() == 'TProfile':
        # histo is a 1D histogram, read it
        if rebin > 1:
            histo.Rebin(rebin)
        hst.source = rootfile
        hst.name = histo.GetName()
        hst.classname = histo.ClassName()
        hst.title = histo.GetTitle()
        hst.xlabel = histo.GetXaxis().GetTitle()
        hst.ylabel = histo.GetYaxis().GetTitle()
        hst.RMS = histo.GetRMS()
        hst.RMSerr = histo.GetRMSError()
        for i in range(1, histo.GetSize() - 1):
            hst.x.append(histo.GetBinLowEdge(i))
            hst.xc.append(histo.GetBinCenter(i))
            hst.y.append(histo.GetBinContent(i))
            hst.xerr.append(histo.GetBinWidth(i) / 2.0)
            hst.yerr.append(histo.GetBinError(i))
        hst.x.append(histo.GetBinLowEdge(histo.GetSize() - 1))
        hst.xc.append(histo.GetBinLowEdge(histo.GetSize() - 1))
        hst.y.append(0.0)
        hst.xerr.append(0.0)
        hst.yerr.append(0.0)
        hst.mean = histo.GetMean()
        hst.meanerr = histo.GetMeanError()
    elif histo.ClassName() == 'TH2D' or histo.ClassName() == 'TH2F' or histo.ClassName() == 'TProfile2D':
        hst = Histo2D()
        histo.RebinX(rebin[0])
        histo.RebinY(rebin[1])
        hst.source = rootfile
        hst.name = histo.GetName()
        hst.title = histo.GetTitle()
        hst.xlabel = histo.GetXaxis().GetTitle()
        hst.ylabel = histo.GetYaxis().GetTitle()
        for x in range(1, histo.GetNbinsX() + 1):
            hst.x.append(histo.GetBinLowEdge(x))
            hst.xc.append(histo.GetBinCenter(x))
            hst.xerr.append(histo.GetBinWidth(x) / 2.0)
        a = np.zeros(shape=(histo.GetNbinsY(), histo.GetNbinsX()))
        hst.BinContents = np.ma.masked_equal(a, 0.0)
        for y in range(1, histo.GetNbinsY() + 1):
            for x in range(1, histo.GetNbinsX() + 1):
                if (histo.ClassName() != 'TProfile2D') or histo.GetBinEntries(histo.GetBin(x, y)) > 0:
                    hst.BinContents[y - 1, x - 1] = histo.GetBinContent(x, y)
            hst.y.append(histo.GetYaxis().GetBinLowEdge(y))
            hst.yc.append(histo.GetYaxis().GetBinCenter(y))
            hst.yerr.append(histo.GetYaxis().GetBinWidth(y) / 2.0)
        hst.xborderhigh = hst.xc[-1] + hst.xerr[-1]
        hst.xborderlow = hst.xc[0] - hst.xerr[0]
        hst.yborderhigh = hst.yc[-1] + hst.yerr[-1]
        hst.yborderlow = hst.yc[0] - hst.yerr[0]
        hst.mean = histo.GetMean()
        hst.meanerr = histo.GetMeanError()
    elif histo.ClassName() == 'TGraphErrors' or histo.ClassName() == 'TGraph':
        # histo is a graph, read it
        hst.source = rootfile
        hst.name = histo.GetName()
        hst.title = histo.GetTitle()
        hst.xlabel = histo.GetXaxis().GetTitle()
        hst.ylabel = histo.GetYaxis().GetTitle()
        a = ROOT.Double(0.0)
        b = ROOT.Double(0.0)
        for i in range(0, histo.GetN()):
            histo.GetPoint(i, a, b)
            x = float(a)
            y = float(b)
            hst.x.append(x)
            hst.xc.append(x)
            hst.y.append(y)
            hst.xerr.append(histo.GetErrorX(i))
            hst.yerr.append(histo.GetErrorY(i))
    else:
        # histo is of unknown type
        print "The object '" + str(histo) + "' is no histogram, no graph and no profile!",
        print "It could not be converted."
        exit(0)
    return hst


def rootdivision(rootobjects, normalize=False):
    #convert TProfiles into TH1Ds because ROOT cannot correctly divide TProfiles
    if (rootobjects[0].ClassName() != 'TH1D' and
           rootobjects[1].ClassName() != 'TH1D'):
        rootobjects[0] = ROOT.TH1D(rootobjects[0].ProjectionX())
        rootobjects[1] = ROOT.TH1D(rootobjects[1].ProjectionX())
    elif normalize:
        rootobjects[1].Scale(rootobjects[0].Integral() /
                             rootobjects[1].Integral())
    rootobjects[0].Divide(rootobjects[1])

    #account for error in empty bins:
    for n in range(rootobjects[0].GetNbinsX()):
        if ((rootobjects[0].GetBinError(n) < 1e-5
                                and rootobjects[0].GetBinContent(n) > 0.1)
                                or rootobjects[0].GetBinContent(n) > 1.15):
            rootobjects[0].SetBinError(n, 0.1)

    return(rootobjects[0])


class Histo:
    """Reduced Histogramm or Graph

    Self-defined histogram class
    constructor needed: (), (roothisto), (x,y,yerr)
    """
    def __init__(self):
        self.source = ""
        self.path = ""
        self.name = ""
        self.classname = ""
        self.title = ""
        self.xlabel = "x"
        self.ylabel = "y"
        self.x = []
        self.xc = []
        self.y = []
        self.xerr = []
        self.yerr = []  # TODO yel, yeh wenn unterschiedlich...
        self.mean = 0.0
        self.meanerr = 0.0
        self.RMS = 0.0
        self.RMSerr = 0.0

    def __len__(self):
        return len(self.y)

    def scale(self, factor):
        for i in range(len(self.y)):
            self.y[i] *= factor
            self.yerr[i] *= factor

    def ysum(self):
        return sum(self.y)

    def __sum__(self):
        return sum(self.y)

    def __min__(self):
        return min(self.y)

    def __max__(self):
        return max(self.y)

    def ymax(self):
        return max(self.y)

    def ymin(self):
        return min(self.y)

    def norm(self):
        return 1.0 / sum(self.y)

    def normalize(self, factor=1.0):
        self.scale(factor * self.norm())
        return self

    def append(self, x, xc, y, yerr=0, xerr=0):
        if xc == True:
            xc = x
        self.x.append(x)
        self.xc.append(xc)
        self.y.append(y)
        self.xerr.append(xerr)
        self.yerr.append(yerr)

    def dropbin(self, number):
        self.x.pop(number)
        self.xc.pop(number)
        self.y.pop(number)
        self.xerr.pop(number)
        self.yerr.pop(number)

    def __div__(self, other):
        #if 0 in other.y:
        #    print "Division by zero!"
        #    return None
        if len(self) != len(other):
            print "Histos of different lengths! The shorter is taken."
        res = Histo()
        res.x = [0.5 * (a + b) for a, b in zip(self.x, other.x)]
        res.xc = [0.5 * (a + b) for a, b in zip(self.xc, other.xc)]
        res.y = [a / b if b != 0 else 0. for a, b in zip(self.y, other.y)]
        res.xerr = [0.5 * (abs(da) + abs(db)) for da, db in zip(self.xerr, other.xerr)]
        res.yerr = [abs(da / b) + abs(db * a / b / b) if b != 0 else 0. for a, da, b, db in zip(self.y, self.yerr, other.y, other.yerr)]
        return res

    def read(self, filename):
        """Read the histogram from a text file

        """
        f = file(filename, 'r')
        self.__init__()
        while True:
            line = f.readline()
            if not line:
                break
            if '#' in line:
                if 'Histogram' in line:
                    self.name = getValue(line, ':')
                elif 'Path' in line:
                    self.path = getValue(line, ':')
                elif 'file' in line:
                    self.source = getValue(line, ':')
                elif 'lumi' in line:
                    self.lumi = getValue(line, ':')
                elif 'x label' in line:
                    self.xlabel = getValue(line, ':')
                elif 'y label' in line:
                    self.xlabel = getValue(line, ':')
                elif 'Title' in line:
                    self.title = getValue(line, ':')
            else:
                values = line.split()
                self.x.append(float(values[1]))
                if len(values) > 5:
                    self.xc.append(float(values[2]))
                    self.y.append(float(values[3]))
                    self.yerr.append(float(values[5]))
                elif len(values) > 2:
                    self.xc.append(float(values[1]))
                    self.y.append(float(values[2]))
                    self.yerr.append(0.0)

    def write(self, filename='.txt'):
        """Write the histogram to a text file"""
        if filename == '.txt':
            filename = 'out/' + self.name + '.txt'
        f = file(filename, 'w')
        f.write(str(self))
        f.close()

    def __str__(self):
        """Show the data of the histogram"""
        text = "#Histogram: " + self.name
        text += "\n#Path:      " + self.path
        text += "\n#From file: " + self.source
        text += time.strftime("\n#Date/time: %a, %d %b %Y %H:%M:%S", time.localtime())
        text += "\n#Norm:      " + str(self.norm())
        text += "\n#Sum:       " + str(self.ysum())
        text += "\n#Maximum:   " + str(self.ymax())
        text += "\n#x label:   " + self.xlabel
        text += "\n#y label:   " + self.ylabel
        text += "\n#title:     " + self.title
        text += "\n#energy:    7"  # in TeV
        text += "\n#  i     x        xmid      y               ynorm"
        text += "           yerr            ynormerr\n"
        if len(self.y) != len(self.x):
            print "This will fail because x and y have not the same length."
        if self.xc == []:
            self.xc = self.x
        for i in range(len(self.y)):
            text += '%4d %8.2f %8.2f %15.8f %15.8f %15.8f %15.8f\n' % (
                    i, self.x[i], self.xc[i], self.y[i], self.y[i] * self.norm(),
                    self.yerr[i], self.yerr[i] * self.norm())
        return text

    def dump(self, filename='.dat'):
        """Write the histogram to a data file"""
        if filename == '.dat':
            filename = 'out/dat/' + self.name + '.dat'
        try:
            f = file(filename, 'w')
            pickle.dump(self, f)
        except:
            print "File " + filename + " could not be written."
        finally:
            f.close()

    def load(self, filename):
        """Write the histogram to a data file"""
        f = file(filename)
        self = pickle.load(f)

    def draw(self, filename="preview.png"):
        import matplotlib.pyplot as plt
        fig = plt.figure()
        ax = fig.add_subplot(111)
        if len(self.y) == len(self.yerr):
            ax.errorbar(self.x, self.y, self.yerr)
        else:
            ax.plot(self.x, self.y)
        fig.savefig(filename)


def getValue(line, key):
    value = line[line.rfind(key) + len(key):line.rfind('\n')]
    value = str.strip(value)
    try:
        val = float(value)
        return val
    except:
        return value


class Histo2D(Histo):
    def __init__(self):
        Histo.__init__(self)
        self.yc = []
        self.BinContents = []
        self.xborderhigh = 0.0
        self.yborderhigh = 0.0
        self.xborderlow = 0.0
        self.yborderlow = 0.0

    def scale(self, factor):
        BinContentsNew = []
        for l in self.BinContents:
            BinContentsNew.append(map(lambda item: item * factor, l))
        self.BinContents = BinContentsNew

    def binsum(self):
        return sum([sum(a) for a in self.BinContents])

    def maxBin(self):
        a = max([max(l) for l in self.BinContents])
        return a


class Fitfunction:
    """For now this can only handle linear functions intended for extrapolation"""
    def __init__(self, x=None, y=None, yerr=None, chi2=None, ndf=None):
        if x is None:
            x = [0.0, 0.3]
        if y is None:
            y = [1.0, 0.99]
        if yerr is None:
            yerr = [0.0, 0.0]
        if chi2 is None:
            chi2 = -1
        if ndf is None:
            ndf = -1
        self.function = '(y1-y0)/(x1-x0)*x + y0'
        self.x = x
        self.y = y
        self.yerr = yerr
        self.chi2 = chi2
        self.ndf = ndf

    def __del__(self):
        pass

    def __str__(self):
        return "y = %s \n  Parameters: x = %s, y = %s, yerr = %s\n  Fit: chi2 / ndf = %1.5f / %d" % (
            (self.function).replace('(y1-y0)/(x1-x0)', str(float(self.y[1] - self.y[0]) / (self.x[1] - self.x[0]))).replace('y0', str(self.y[0])),
            str(self.x),
            str(self.y),
            str(self.yerr),
            self.chi2,
            self.ndf)

    def __len__(self):
        return len(self.y)

    def f(self, x):
        return float(self.y[1] - self.y[0]) / (self.x[1] - self.x[0]) * x + float(self.y[0])

    def ferr(self, x):
        result = float(self.yerr[1] - self.yerr[0]) / (self.x[1] - self.x[0]) * x + float(self.yerr[0])
        assert result >= 0
        return result

    def k(self):
        return float(self.y[0] / self.f(0.2))

    def kerr(self):
        return float(self.yerr[0]) / self.f(0.2) - self.y[0] / self.f(0.2) / self.f(0.2) * self.ferr(0.2)

    def plotx(self, left=0.0, right=0.34):
        assert left < right
        return [left + x / 100.0 for x in range((right - left) * 100)]

    def ploty(self, left=0.0, right=0.34):
        assert left < right
        return [self.f(x) for x in self.plotx(left, right)]

    def plotyh(self, left=0.0, right=0.34):
        assert left < right
        return [self.f(x) + self.ferr(x) for x in self.plotx(left, right)]

    def plotyl(self, left=0.0, right=0.34):
        assert left < right
        return [self.f(x) - self.ferr(x) for x in self.plotx(left, right)]


def dividegraphs(graph1, graph2):
    assert graph1.ClassName() == 'TGraphErrors'
    assert graph2.ClassName() == 'TGraphErrors'
    assert graph1.GetN() == graph2.GetN()
    result = ROOT.TGraphErrors(graph1.GetN())

    for i in range(graph1.GetN()):
        x1, y1, dx1, dy1 = getgraphpoint(graph1, i)
        x2, y2, dx2, dy2 = getgraphpoint(graph2, i)
        if y2 == 0 or y1 == 0:
            print "Division by zero!"
        else:
            result.SetPoint(i, 0.5 * (x1 + x2), y1 / y2)
            result.SetPointError(i, 0.5 * (abs(dx1) + abs(dx2)),
                abs(y1 / y2) * math.sqrt((dy1 / y1) ** 2 + (dy2 / y2) ** 2))
    return result


def getgraphpoint(graph, i):
        a = ROOT.Double(0.0)
        b = ROOT.Double(0.0)
        graph.GetPoint(i, a, b)
        return float(a), float(b), graph.GetErrorX(i), graph.GetErrorY(i)


def histo2root(plot):
    """Convert a Histo object to a root histogram (TH1D)"""
    title = plot.title + ";"  # + plot.xlabel() + ";" + plot.ylabel()
    print len(plot), title
    print min(plot.x), max(plot.x)
    th1d = ROOT.TH1D(plot.name, title, len(plot), min(plot.x), max(plot.x))

    if not hasattr(plot, 'dropbin'):
        print histo, "is no Histo. It could not be converted."
        exit(0)
    for i in range(0, len(plot)):
            th1d.SetBinContent(i + 1, plot.y[i])
            th1d.SetBinError(i + 1, plot.yerr[i])
            #th1d.SetBin(i, plot.x[i])
            #th1d.SetBinWidth(i, plot.xerr[i]/2)
            #th1d.SetBinCenter(i, plot.xc[i])
    return th1d


def writePlotToRootfile(plot, filename, plotname=None):
    if hasattr(plot, 'dropbin'):
        histo = histo2root(plot)
        print "File", filename
        print "plot", plotname
        f = ROOT.TFile("file.root", "RECREATE")
        histo.Write()
        f.Close()


def getgraph(x, y, f, opt, settings, changes=None, key='var', var=None, drop=True, root=True, median=False, absmean=False):
    """get a Histo easily composed from different folders

       get a Histo with quantity 'y' plotted over quantity 'x' from file 'f'
       and the settings in 'changes'. The x axis is the variation in 'var'.
    """

    settings = plotbase.applyChanges(settings, changes)
    try:
        f1 = f[0]
        f2 = f[1]
    except:
        f1 = f
        f2 = None

    settings['special_binning'] = True
    settings['rebin'] = 1

    settings['x'] = plotbase.getaxislabels_list(x)[:2]
    xvals = getplotfromtree('_'.join([x, x]), f, settings)
    settings['x'] = plotbase.getaxislabels_list(y)[:2]
    yvals = getplotfromtree('_'.join([y, x]), f, settings)

    graph = ROOT.TGraphErrors()

    for i, x, y, xerr, yerr in zip(range(len(yvals.y) - 1), xvals.y, yvals.y, xvals.yerr, yvals.yerr):
        graph.SetPoint(i, x, y)
        graph.SetPointError(i, xerr, yerr)

    return graph


def getgraphratio(x, y, f1, f2, opt, settings, changes={}, key='var', var=None, drop=True, median=False, absmean=False):
    if f2 is None:
        return getgraph(x, y, f1, opt, settings, changes, key, var, drop, True, median, absmean)
    graph1 = getgraph(x, y, f1, opt, settings, changes, key, var, drop, True, median, absmean)
    graph2 = getgraph(x, y, f2, opt, settings, changes, key, var, drop, True, median, absmean)
    return dividegraphs(graph1, graph2)


def histomedian(histo):

    import array
    nBins = histo.GetXaxis().GetNbins()
    x = []
    y = []
    for i in range(nBins):
        x += [ROOT.Double(histo.GetBinCenter(i))]
        y += [ROOT.Double(histo.GetBinContent(i))]
    assert len(x) == len(y)
    assert len(x) == nBins
    x = array.array('d', x)
    y = array.array('d', y)

    return ROOT.TMath.Median(nBins, x, y)


def histoabsmean(histo):
    absEta = histo.Clone()
    n = absEta.GetNbinsX()       # 200
    assert n % 2 == 0
    for i in range(1, n / 2 + 1):    # 1..100 : 200..101
        absEta.SetBinContent(n + 1 - i, histo.GetBinContent(i) + histo.GetBinContent(n - i + 1))
        absEta.SetBinContent(i, 0)
