# -*- coding: utf-8 -*-
""" interaction with ROOT

    file open, reading of histograms and converting to a Histo class
    as well as the resp_cuts file format specific functions.

    Here is an overview of the most interesting functions besides 'openfile':
             -------------------gethisto(f)------------------------->
    quantity -gethistoname-> name -getobject(f)-> histo -root2histo-> Histo
                                  --------gethisto(f)--------------->

"""
import ROOT
import cPickle as pickle
from time import localtime, strftime, clock


def openfile(filename, verbose=False):
    """Open a root file"""
    f = ROOT.TFile(filename)
    if f:
        if verbose: print " * Inputfile:", filename
    else:
        print "Can't open file:", filename
        assert False
    return f


def gethisto(name, rootfile, changes={}, isdata=False, rebin=1):
    """get a Histo by only knowing the name of the quantity

    if the full name is present in the root file this will be taken
    if not the name is made by the gethistoname algorithm.
    isdata is used to enforce the loading of exactly this histogram and
    not the MC version without 'Res'
    """
    if not rootfile.Get(name):
        name = gethistoname(name, changes)
    if rootfile.Get(name) or isdata:
        roothist = getobject(rootfile, name)
    else:
        roothist = getobject(rootfile, name.replace("Res",""))
    return root2histo(roothist, rootfile.GetName(), rebin)


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
            if name.find("Pt")==0 and name.find("to")>0:
                low  = int(name[2:name.find("to")])
                high = int(name[name.find("to")+2:name.find("_")])
                if low  not in result:
                    result.append(low)
                if high not in result:
                    result.append(high)
        assert result != [], "No bins found in "+rootfile.GetName()
    except AssertionError:
        print result
        print "Bins could not be determined from root file."
        print "Fall-back binning used:", fallbackBins
        result = fallbackbins
        assert result != []
    result.sort()
    return result


def getobject(rootfile, objectname):
    """Import a root object"""
    oj = rootfile.Get(objectname)
    if not oj:
        print "Can't load", objectname, "from", rootfile.GetName()
        assert False
    return oj


def gethistoname(quantity='zmass', change={}):
    """Build the name of a histogram according to CalibFW conventions.

    Every histogram written by resp_cuts has a name like
    'NoBinning_incut/<quantity>_ak5PFJetsL1L2L3CHS_hist'
    This string is returned for each quantity and any changes to the default
    can be applied via the change dictionary.
    Toplevel plots have names like
    'jetresp_ak5PFJetsL1L2L3CHS_graph'
    and they are retrieved with the '../' prefix, e.g. '../jetrepsonse'

    """
    # Set default values
    keys = ['bin', 'incut', 'var', 'quantity', 'algorithm', 'correction', 'plottype']
    selection = {'bin': 'NoBinning', 'incut': 'incut', 'var': '',
                 'quantity': '<quantity>', 'algorithm': 'ak5PFJets',
                 'correction': 'L1L2L3CHS', 'plottype': 'hist'}
    hst = ''
    # apply requested changes
    for k in change.keys():
        if k in selection.keys():
            selection[k] = change[k]
        else:
            print k, "is no valid key. Valid keys are: ", keys
            assert False
    # make a prototype name
    for k in keys:
        hst += selection[k] + '_'
    hst = hst[:-1].replace('Jets_', 'Jets').replace('__', '_')
    # Now, the default string for hst looks like:
    # NoBinning_incut_<quantity>_ak5PFJetsL1L2L3CHS_hist
    # ability to get level 2 pipeline plots via ../ prefix:
    if quantity.find('../') == 0:
        quantity = quantity[3:]
        hst = hst[hst.find('_<quantity>'):]
        hst = hst.replace('hist', 'graph')
    else:
        quantity = '/' + quantity
    hst = hst.replace('_<quantity>',quantity)
    return hst



def root2histo(histo, rootfile='', rebin=1):
    """Convert a root histogram to the Histo class"""
    hst = Histo()
    # Detect if it is a TH1D or a TGraphErrors
    if hasattr(histo,'GetNbinsX'): 
        # histo is a histogram, read it
        histo.Rebin(rebin)
        hst.source = rootfile
        hst.name = histo.GetName()
        hst.title = histo.GetTitle()
        hst.xlabel = histo.GetXaxis().GetTitle()
        hst.ylabel = histo.GetYaxis().GetTitle()
        for i in range(1,histo.GetSize()-1):
            hst.x.append(histo.GetBinLowEdge(i))
            hst.xc.append(histo.GetBinCenter(i))
            hst.y.append(histo.GetBinContent(i))
            hst.xerr.append(histo.GetBinWidth(i)/2.0)
            hst.yerr.append(histo.GetBinError(i))
        hst.x.append(histo.GetBinLowEdge(histo.GetSize()-1))
        hst.xc.append(histo.GetBinLowEdge(histo.GetSize()-1))
        hst.y.append(0.0)
        hst.xerr.append(0.0)
        hst.yerr.append(0.0)
        hst.norm = 1.0/histo.GetSum()
        hst.ysum = histo.GetSum()
        hst.ymax = histo.GetMaximum()
    elif hasattr(histo,'GetN'):
        # histo is a graph, read it
        hst.source = rootfile
        hst.name = histo.GetName()
        hst.title = histo.GetTitle()
        hst.xlabel = histo.GetXaxis().GetTitle()
        hst.ylabel = histo.GetYaxis().GetTitle()
        a = ROOT.Double(0.0)
        b = ROOT.Double(0.0)
        for i in range(0,histo.GetN()):
            histo.GetPoint(i,a,b)
            x=float(a)
            y=float(b)
            if hst.ymax < y: hst.ymax = y
            hst.ysum += y
            hst.x.append(x)
            hst.xc.append(x)
            hst.y.append(y)
            hst.xerr.append(histo.GetErrorX(i))
            hst.yerr.append(histo.GetErrorY(i))
        hst.norm = 1.0/hst.ysum
    else:
        # histo is of unknown type
        print "The object '" + histo + "' is no histogram and no graph! Could not convert."
    return hst
    

class Histo:
    """Reduced Histogramm or Graph
    
    Self-defined histogram class
    constructor needed: (), (roothisto), (x,y,yerr)
    """
#    def __init__(self,xcv=[],yv=[],yerrv=[],xv=[],xerrv=[]):
#        if len(xcv)!=len(yv) and len(xv)!=len(yv):
#            print "x and y values are not of equal number:", xc, y
#        print xcv, yv, yerrv, xv, xerrv
#        self.source = ""
#        self.path = ""
#        self.name = ""
#        self.title = ""
#        self.xlabel = "x"
#        self.ylabel = "y"
#        self.x = xv
#        self.xc = xcv
#        self.y = yv
#        self.xerr = xerrv
#        self.yerr = yerrv #TODO yel, yeh wenn unterschiedlich...
#        self.ysum = sum(yv)
#        if self.ysum:
#            self.norm = 1.0/self.ysum
#        else:
#            self.norm = 1.0
#        if self.y==[]:
#            self.ymax=0.0
#        else:
#            self.ymax = max(yv)

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
        self.yerr =  [] #TODO yel, yeh wenn unterschiedlich...
        self.ysum = 0.0
        self.norm = 1.0
        self.ymax=0.0

    def __del__(self):
        pass
    
    def __len__(self):
        return len(self.y)
        
    def scale(self,factor):
        for i in range(len(self.y)):
            self.y[i] *= factor
            self.yerr[i] *= factor
        self.ysum *= factor
        self.norm /= factor
        self.ymax *= factor

    def normalize(self, factor=1.0):
        self.scale(factor*self.norm)
        return self

    def dropbin(self,number):
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
            if len(line) == 0: break
            if line.find('#') >=0:
                if line.find('Histogram')>0: self.name = getValue(line,':')
                elif line.find('Path')>0: self.path = getValue(line,':')
                elif line.find('file')>0: self.source = getValue(line,':')
                elif line.find('lumi')>0: self.lumi = getValue(line,':')
                elif line.find('Norm')>0: self.norm = getValue(line,':')
                elif line.find('Sum')>0: self.ysum = getValue(line,':')
                elif line.find('Max')>0: self.ymax = getValue(line,':')
                elif line.find('x label')>0: self.xlabel = getValue(line,':')
                elif line.find('y label')>0: self.xlabel = getValue(line,':')
                elif line.find('Title')>0: self.title = getValue(line,':')
            else:
                values = line.split()
                self.x.append(float(values[1]))
                if len(values)>5:
                    self.xc.append(float(values[2]))
                    self.y.append(float(values[3]))
                    self.yerr.append(float(values[5]))
                elif len(values)>2:
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
        text =    "#Histogram: " + self.name
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
            text += '%4d %8.2f %8.2f %15.8f %15.8f %15.8f %15.8f\n' \
                % (i, self.x[i], self.xc[i], self.y[i], self.y[i]*self.norm, \
                self.yerr[i], self.yerr[i]*self.norm)
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
    value = line[line.rfind(key)+len(key):line.rfind('\n')]
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
        self.parameter = [0.5,-1.0]
        
    def __del__(self):
        pass
    
    def __str__(self):
        pass
    
    def __len__(self):
        pass


# deprecated
def IsObjectExistent ( RootDict, ObjectName ):
    return bool(RootDict.Get(ObjectName))

# not necessary, there is openfile
def openFiles(datafiles, mcfiles, verbose=False):
    print "DEPRECATED"
#    if datafiles ... openFiles(op) auch machen
    if verbose: print "%1.2f | Open files" % clock()
    fdata = [openfile(f, verbose) for f in datafiles]
    fmc   = [openfile(f, verbose) for f in mcfiles]
    # be prepared for multiple data and multiple mc files. For the moment: return the first
    return fdata[0], fmc[0]

def SafeGet(RootDict, ObjectName):
    print "SafeGet is deprecated, use getobject instead"
    return getobject(RootDict, ObjectName)

def GetHistoname(quantity='zmass', change={}):
    print "'GetHistoname' is deprecated, use 'gethistoname' instead."
    gethistoname(quantity, change)

def ConvertToArray(histo, rootfile='', rebin=1):
    print "'ConverToArray' is deprecated, use 'root2histo' instead."
    return root2histo(histo, rootfile, rebin)

def SafeConvert(RootDict, ObjectName, formatslist=[], rebin=1):
    """Combined import and conversion to Histo"""
    print "Deprecated SafeConvert, please use gethisto?"
    root_histo = SafeGet(RootDict, ObjectName)
    histo = ConvertToArray(root_histo,'',rebin)
    if 'txt' in formatslist:
        histo.write()
    if 'dat' in formatslist:
        histo.dump()
    return histo
