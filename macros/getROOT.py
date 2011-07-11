# -*- coding: utf-8 -*-

import ROOT
import cPickle as pickle
from time import localtime, strftime


def SafeGet( RootDict, ObjectName ):
    """Import a root object"""
    oj = RootDict.Get( ObjectName )
    if not oj:
        print "Can't load", ObjectName,"from", RootDict.GetName()
        assert False

    return oj

def IsObjectExistent ( RootDict, ObjectName ):
    oj = RootDict.Get( ObjectName )
    if oj:
        return True
    else:
        return False
        
def OpenFile(filename, message=False):
    """Open a root file"""
    f = ROOT.TFile(filename)
    if f:
        if message: print " -> Inputfile:", filename
    else:
        print "Can't open file:", filename
        assert False
    return f

def ConvertToArray(histo, lumi=0.0, rootfile='', rebin=1):
    """Convert a root histogram to a numpy histogram
    
    """
    hst = npHisto()
    # Detect if it is a TH1D or a TGraphErrors
    if hasattr(histo,'GetNbinsX'): 
        # histo is a histogram, read it
        histo.Rebin(rebin)
        hst.source = ""#rootfile
        hst.path = ""#path in rootfile
        hst.name = histo.GetName()
        hst.title = histo.GetTitle()
        hst.xlabel = histo.GetXaxis().GetTitle()
        hst.ylabel = histo.GetYaxis().GetTitle()
        hst.lumi = lumi
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
        hst.path = ""#path in rootfile
        hst.name = histo.GetName()
        hst.title = histo.GetTitle()
        hst.xlabel = histo.GetXaxis().GetTitle()
        hst.ylabel = histo.GetYaxis().GetTitle()
        hst.lumi = lumi
        a = ROOT.Double(0.0)
        b = ROOT.Double(0.0)
        for i in range(1,histo.GetN()):
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
    #print hst
    return hst
    

def SafeConvert(RootDict, ObjectName, lumi=0.0, formatslist=[], rebin=1):
    """Combined import and conversion to npHisto
    
    """
    root_histo = SafeGet(RootDict, ObjectName)
    histo = ConvertToArray(root_histo,lumi,'',rebin)
    if 'txt' in formatslist:
        histo.write()
    if 'dat' in formatslist:
        histo.dump()
    return histo

class npHisto:
    """Reduced Histogramm or Graph
    
    Self-defined Numpy histogram
    """
    def __init__(self):
        self.source = ""
        self.path = ""
        self.name = ""
        self.title = ""
        self.xlabel = "x"
        self.ylabel = "y"
        self.lumi = 0.0
        self.norm = 1.0
        self.ysum = 0.0
        self.ymax = 0.0
        self.x = []
        self.xc = []
        self.y = []
        self.xerr = []
        self.yerr = [] #TODO yel, yeh wenn unterschiedlich...

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
        text += "\n#lumi:      " + str(self.lumi) # in pb^{-1}
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
#        print self


def getValue(line, key):
    value = line[line.rfind(key)+len(key):line.rfind('\n')]
    value = str.strip(value)
    try:
        val = float(value)
        return val
    except:
        return value
#    if type(value) == type('s'):
#        return str(value)
#    elif type(value) == type(1.0):
#        return float(value)
#    else:
#        return value


class fitfunction:
    """
    
    """
    def __init__(self):
        self.function = 'a*x+b'
        self.parameter = [0.5,-1.0]
        
    def __init__(self, rootfunction):
        pass
    
    def __del__(self):
        pass
    
    def __str__(self):
        pass
    
    def __len__(self):
        pass


