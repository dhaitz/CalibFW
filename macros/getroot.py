# -*- coding: utf-8 -*-
""" interaction with ROOT

    file open, reading of histograms and converting to a Histo class
    as well as the resp_cuts file format specific functions.

    Here is an overview of the most interesting functions besides 'openfile':
    quantity -------------------------getplotfromnick------------------> Histo
                              name ----------getplot-------------------> Histo
    quantity -getobjectname-> name -getobject-> rootobject -root2histo-> Histo
    quantity ------getobjectfromnick----------> rootobject

    quantity:   Short name/nick name (expanded with default values) e.g. z_pt
    name:       name of root object in file, e.g. NoBinning_incut/z_pt_AK5PFJetsL1L2L3
    rootobject: root object (TH1D, TGraphErrors, etc.)
    Histo:      getroot Histo class object

    getobjectname expands a short name with default values to the object name in the root file
    getobject yields a root object
    getplot yields a getroot Histo

"""
import ROOT
import cPickle as pickle
import time
import copy



# converts a integer list of bins [1, 30, 70] to a
# string representation ["Pt1to30", "Pt30to70"]
def binstrings(bins):
    return ["Pt{0}to{1}".format(b) for b in zip(bins[:-1], bins[1:])]


def npvstrings(npv):
    return ["var_Npv_{0}to{1}".format(*n) for n in npv]


def etastrings(eta):
    etastr = map(lambda x: str(x).replace(".", "_"), eta)
    return ["var_eta_%sto%s" % h for h in zip(etastr[:-1], etastr[1:]) ]


def cutstrings(cuts):
    return ["var_CutSecondLeadingToZPt__%s" % str(c).replace(".", "_") for c in cuts]


def openfile(filename, verbose=False, exitonfail=True):
    """Open a root file"""
    f = ROOT.TFile(filename)
    if not f:
        print "Can't open file:", filename
        assert not exitonfail
    if verbose:
        print " * Inputfile:", filename
    return f


def getplot(name, rootfile, changes={}, rebin=1):
    rootobject = getobject(name, rootfile, changes)
    return root2histo(rootobject, rootfile.GetName(), rebin)


def getplotfromnick(nickname, rootfile, changes, rebin=1):
    objectname = getobjectname(nickname, changes)
    rootobject = getobject(objectname, rootfile, changes)
    return root2histo(rootobject, rootfile.GetName(), rebin)


def getobjectfromnick(nickname, rootfile, changes, rebin=1):
    objectname = getobjectname(nickname, changes)
    return getobject(objectname, rootfile, changes)


def getbins(rootfile, fallbackbins):
    """ guess the binning from the folders in a rootfile

    this function assumes that the binning is reflected in the naming
    scheme of the folders in the rootfile in the following way:
    Pt<low>to<high>_* where <low> and <high> are the bin borders.
    If this fails the value of fallbackbins is returned.
    """
    result = []
    try:
        for key in rootfile.GetListOfKeys():
            name = key.GetName()
            if name.find("Pt") == 0 and "to" in name:
                low, high = [int(x) for x in name[2:name.find("_")].split("to")]
                if low  not in result:
                    result.append(low)
                if high not in result:
                    result.append(high)
        assert result != [], "No bins found in " + rootfile.GetName()
    except AssertionError:
        print result
        print "Bins could not be determined from root file."
        print "Fall-back binning used:", fallbackbins
        result = fallbackbins
        assert result != []
    result.sort()
    return result


def getobject(name, rootfile, changes={}, exact=True):
    """get a root object by knowing the exact name

    'exact' could be used to enforce the loading of exactly this histogram and
    not the MC version without 'Res' but it is not at the moment (strict version)
    """
    oj = rootfile.Get(name)
    if not oj:
        print "Can't load object", name, "from root file", rootfile.GetName()
        exit(0)
    assert oj
    return oj


def getobjectname(quantity='z_mass', change={}):
    """Build the name of a histogram according to CalibFW conventions.

    Every histogram written by closure has a name like
    'NoBinning_incut/<quantity>_ak5PFJetsCHSL1L2L3'
    This string is returned for each quantity and any changes to the default
    can be applied via the change dictionary.

    Examples for quantities:
        z_pt, mu_plus_eta, cut_all_npv, L1_npv
    """
    # Set default values
    keys = ['bin', 'incut', 'var', 'quantity', 'algorithm', 'correction']
    selection = {'bin': 'NoBinning', 'incut': 'incut', 'var': '',
                 'quantity': '<quantity>', 'algorithm': 'AK5PFJets',
                 'correction': 'L1L2L3' }
    hst = ''
    # apply requested changes
    for k in change:
        if k in selection:
            selection[k] = change[k]
        else:
            print k, "is no valid key. Valid keys are: ", keys
            exit(1)
    # make a prototype name and correct it
    for k in keys:
        hst += selection[k] + '_'
        
    hst = hst[:-1].replace('Jets_', 'Jets').replace('__', '_').replace('_L1', 'L1')
    hst = hst.replace('_<quantity>', '/' + quantity)
    #print "return:", hst
    return hst


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
        histo.Rebin(rebin)
        hst.source = rootfile
        hst.name = histo.GetName()
        hst.title = histo.GetTitle()
        hst.xlabel = histo.GetXaxis().GetTitle()
        hst.ylabel = histo.GetYaxis().GetTitle()
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
    elif histo.ClassName() == 'TGraphErrors':
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


class Histo:
    """Reduced Histogramm or Graph

    Self-defined histogram class
    constructor needed: (), (roothisto), (x,y,yerr)
    """
    def __init__(self):
        self.source = ""
        self.path = ""
        self.name = ""
        self.title = ""
        self.xlabel = "x"
        self.ylabel = "y"
        self.x = []
        self.xc = []
        self.y = []
        self.xerr = []
        self.yerr = []  # TODO yel, yeh wenn unterschiedlich...

    def __del__(self):
        pass

    def __len__(self):
        return len(self.y)

    def scale(self, factor):
        for i in range(len(self.y)):
            self.y[i] *= factor
            self.yerr[i] *= factor

    def ysum(self):
        return sum(self.y)

    def ymax(self):
        return max(self.y)

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
            filename = 'out/dat/' + self.name + '.txt'
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
        for i in range(len(self.y)):
            text += '%4d %8.2f %8.2f %15.8f %15.8f %15.8f %15.8f\n' % (
                    i, self.x[i], self.xc[i], self.y[i], self.y[i] * self.norm,
                    self.yerr[i], self.yerr[i] * self.norm)
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
        return "y = %s \n  Parameters: x = %s, y = %s, yerr = %s\n  Fit: chi2 / ndf = %1.5f / %d" %(
            (self.function).replace('(y1-y0)/(x1-x0)', str(float(self.y[1]-self.y[0])/(self.x[1]-self.x[0]))).replace('y0', str(self.y[0])),
            str(self.x),
            str(self.y),
            str(self.yerr),
            self.chi2,
            self.ndf)

    def __len__(self):
        return len(self.y)

    def f(self, x):
        return float(self.y[1]-self.y[0])/(self.x[1]-self.x[0])*x + float(self.y[0])

    def ferr(self, x):
        return float(self.yerr[1]-self.yerr[0])/(self.x[1]-self.x[0])*x + float(self.yerr[0])

    def k(self):
        return float(self.y[0]/self.f(0.2))

    def kerr(self):
        return float(self.yerr[0]) / self.f(0.2) - self.y[0] / self.f(0.2) / self.f(0.2) * self.ferr(0.2)

    def plotx(self, left=0.0, right=0.34):
        assert left < right
        return [left + x / 100.0 for x in range((right-left)*100)]

    def ploty(self, left=0.0, right=0.34):
        assert left < right
        return [self.f(x) for x in self.plotx(left, right)]

    def plotyh(self, left=0.0, right=0.34):
        assert left < right
        return [self.f(x) + self.ferr(x) for x in self.plotx(left, right)]

    def plotyl(self, left=0.0, right=0.34):
        assert left < right
        return [self.f(x) - self.ferr(x) for x in self.plotx(left, right)]


def fitline(rootgraph):
    fitf = TF1("fit1", "1*[0]", 1.0, 1000.0)
    rootgraph.Fit(fitf,"QN")
    return (fitf.GetParameter(0), fitf.GetParError(0))

# for compatibility
def gethisto(name, rootfile, changes={}, rebin=1):
    return getplot(name, rootfile, changes, rebin)

# deprecated
def getobject_direct(name, rootfile):
    oj = rootfile.Get(name)
    return oj

# deprecated
#def IsObjectExistent(RootDict, ObjectName):
#    return bool(RootDict.Get(ObjectName))


# not necessary, there is openfile
#def openFiles(datafiles, mcfiles, verbose=False):
#    print "DEPRECATED"
    # if datafiles ... openFiles(op) auch machen
#    if verbose:
#        print "%1.2f | Open files" % clock()
#    fdata = [openfile(f, verbose) for f in datafiles]
#    fmc = [openfile(f, verbose) for f in mcfiles]
#    # be prepared for multiple data and multiple mc files.
#    # For the moment: return the first
#    return fdata[0], fmc[0]


#def SafeGet(RootDict, ObjectName):
#    print "SafeGet is deprecated, use getobject instead"
#    return getobject(RootDict, ObjectName)


#def GetHistoname(quantity='zmass', change={}):
#    print "'GetHistoname' is deprecated, use 'gethistoname' instead."
#    gethistoname(quantity, change)


#def ConvertToArray(histo, rootfile='', rebin=1):
#    print "'ConverToArray' is deprecated, use 'root2histo' instead."
#    return root2histo(histo, rootfile, rebin)


#def SafeConvert(RootDict, ObjectName, formatslist=[], rebin=1):
#    """Combined import and conversion to Histo"""
#    print "Deprecated SafeConvert, please use gethisto?"
#    root_histo = SafeGet(RootDict, ObjectName)
#    histo = ConvertToArray(root_histo, '', rebin)
#    if 'txt' in formatslist:
#        histo.write()
#    if 'dat' in formatslist:
#        histo.dump()
#    return histo
