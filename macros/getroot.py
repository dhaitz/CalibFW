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
from time import localtime, strftime, clock


def createchanges(opt, change={}):
    change["correction"] = opt.correction
    change["algorithm"] = opt.algorithm
    return change


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
        hst.norm = 1.0 / histo.GetSum()
        hst.ysum = histo.GetSum()
        hst.ymax = histo.GetMaximum()
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
            if hst.ymax < y:
                hst.ymax = y
            hst.ysum += y
            hst.x.append(x)
            hst.xc.append(x)
            hst.y.append(y)
            hst.xerr.append(histo.GetErrorX(i))
            hst.yerr.append(histo.GetErrorY(i))
        if hst.ysum != 0:
            hst.norm = 1.0 / hst.ysum
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
        self.ysum = 0.0
        self.norm = 1.0
        self.ymax = 0.0

    def __del__(self):
        pass

    def __len__(self):
        return len(self.y)

    def scale(self, factor):
        for i in range(len(self.y)):
            self.y[i] *= factor
            self.yerr[i] *= factor
        self.ysum *= factor
        self.norm /= factor
        self.ymax *= factor

    def yysum(self):
        return sum(self.y)

    def yymax(self):
        return max(self.y)

    def normalize(self, factor=1.0):
        self.scale(factor * self.norm)
        return self

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
                elif 'Norm' in line:
                    self.norm = getValue(line, ':')
                elif 'Sum' in line:
                    self.ysum = getValue(line, ':')
                elif 'Max' in line:
                    self.ymax = getValue(line, ':')
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
        text += strftime("\n#Date/time: %a, %d %b %Y %H:%M:%S", localtime())
        text += "\n#Norm:      " + str(self.norm)
        text += "\n#Sum:       " + str(self.ysum)
        text += "\n#Maximum:   " + str(self.ymax)
        text += "\n#x label:   " + self.xlabel
        text += "\n#y label:   " + self.ylabel
        text += "\n#title:     " + self.title
        text += "\n#energy:    7"  # in TeV
        text += "\n#  i     x        xmid      y               ynorm"
        text += "           yerr            ynormerr\n"
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


def getValue(line, key):
    value = line[line.rfind(key) + len(key):line.rfind('\n')]
    value = str.strip(value)
    try:
        val = float(value)
        return val
    except:
        return value


class fitfunction:
    """

    """
    def __init__(self, rootfunction=""):
        self.function = 'a*x+b'
        self.parameter = [0.5, -1.0]

    def __del__(self):
        pass

    def __str__(self):
        pass

    def __len__(self):
        pass


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
