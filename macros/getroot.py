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
    return ["Pt{0}to{1}".format(*b) for b in zip(bins[:-1], bins[1:])]


def npvstrings(npv):
    return ["var_Npv_{0}to{1}".format(*n) for n in npv]


def etastrings(eta):
    etastr = map(lambda x: str(x).replace(".", "_"), eta)
    return ["var_JetEta_%sto%s" % h for h in zip(etastr[:-1], etastr[1:]) ]


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
    for k in change:
        if k not in selection:
            print k, "is no valid key. Valid keys are: ", keys
            exit(1)
    # apply requested changes
    selection.update(change)
    # make a prototype name and correct it
    for k in keys:
        hst += selection[k] + '_'
    hst = hst[:-1].replace('Jets_', 'Jets').replace('__', '_').replace('_L1', 'L1')
    hst = hst.replace('_<quantity>', '/' + quantity)
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

    def __div__(self, other):
        if 0 in other.y:
            print "Division by zero!"
            return None
        if len(self) != len(other):
            print "Histos of different lengths! The shorter is taken."
        res = Histo()
        res.x = [0.5*(a+b) for a, b in zip(self.x, other.x)]
        res.xc = [0.5*(a+b) for a, b in zip(self.xc, other.xc)]
        res.y = [a / b if b != 0 else 0. for a,b in zip(self.y, other.y)]
        res.xerr = [0.5*(abs(da)+abs(db)) for da, db in zip(self.xerr, other.xerr)]
        res.yerr = [abs(da/b)+abs(db*a/b/b) if b != 0 else 0. for a, da, b, db in zip(self.y, self.yerr, other.y, other.yerr)]


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
        result = float(self.yerr[1]-self.yerr[0])/(self.x[1]-self.x[0])*x + float(self.yerr[0])
        assert result >= 0
        return result

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
    fitf = ROOT.TF1("fit1", "1*[0]", 1.0, 1000.0)
    fitres = rootgraph.Fit(fitf,"SQN")
    return (fitf.GetParameter(0), fitf.GetParError(0), fitres.Chi2(), fitres.Ndf())


def dividegraphs(graph1, graph2):
    assert graph1.ClassName() == 'TGraphErrors'
    assert graph2.ClassName() == 'TGraphErrors'
    assert graph1.GetN() == graph2.GetN()
    result = ROOT.TGraphErrors(graph1.GetN())

    for i in range(graph1.GetN()):
        x1, y1, dx1, dy1 = getgraphpoint(graph1, i)
        x2, y2, dx2, dy2 = getgraphpoint(graph2, i)
        if dy2 == 0:
            print "Division by zero!"
        else:
            result.SetPoint(i, 0.5 * (x1 + x2), y1 / y2)
            result.SetPointError(i, 0.5 * (abs(dx1) + abs(dx2)),
                                 abs(dy1 / y2) + abs(dy2 * y1 / y2 / y2))
    return result

def getgraphpoint(graph, i):
        a = ROOT.Double(0.0)
        b = ROOT.Double(0.0)
        graph.GetPoint(i, a, b)
        return float(a), float(b), graph.GetErrorX(i), graph.GetErrorY(i)


def getgraph(x, y, f, opt, changes={}, key='var', var=None, drop=True, root=True):
    """get a Histo easily composed from different folders

       x ['z_pt', 'npv', 'alpha', 'jet1_eta', 'var'(list), 'custom'(custom)]
       y any quantity (ratio?)
       var
       #result = root, py, both (idee: x könnte bin grenzen beinhalten, xc mean)
       get a Histo with quantity 'y' plotted over quantity 'x' from file 'f'
       and the settings in 'changes'. The x axis is the variation in 'var'.
         var = ["_var_SecondJetCut_0_3", ...] (stringlist)
         var = {'var': ["_var_SecondJetCut_0_3", ...]} (dictionary)
       Optionally a straight line can be fitted to it.
       If var is Pt-bins, the first datapoint can be dropped (default).
       x,y,var(x)
    """
    print "Get a", y, "over", x, "plot from file:", f.GetName()
    try:
        f1 = f[0]
        f2 = f[1]
    except:
        f1 = f
        f2 = None
    # Determine the value to be varied/to be looped over:
    if x == 'z_pt':
        # x = mean(z_pt), var = Pt0to30
        key = 'bin'
        var = binstrings(opt.bins)
        if drop:
            var.pop(0)
    elif x == 'npv':
        # x = mitte(npvbin) from opt, var = var_Npv_0to1
        var = npvstrings(opt.npv)
        print opt.npv, var
        x = [0.5 * (a + min(b, 35)) for a, b in opt.npv]
        xerr = [0.5 * (b - a) for a, b in opt.npv]
    elif x == 'alpha':
        # x = alpha from opt, var mit
        var = cutstrings(opt.cut)
        x = opt.cut
        xerr = [0 for a in x]
    elif x == 'jet1_eta':
        var = etastrings(opt.eta)
    elif type(var) == list:
        # x is a list of variations in the var key
        print "Plot", x, "over this list:", var
    elif type(var) == dict:
        # x is a dictionary of variations
        print "Plot", x, "over this dictionary:", var
        key = var.keys()[0]
        var = var.values()[0]
    else:
        print "The value to be varied is unknown! x =", x, "var =", var
        exit(0)
    assert type(var) == list
    assert len(var) > 1
    assert type(var[0]) == str

    # loop over variation (x-axis)
    ch = copy.deepcopy(changes)
    if 'var' in ch:
        var = [v + "_" + ch['var'] for v in var]
    graph = ROOT.TGraphErrors()
    for i in range(len(var)):
        # read the values from folders
        ch[key] = var[i]
        # get x (histogram mean or from list)
        if type(x) == str:
            ojx = getobjectfromnick(x, f1, ch)
            xi = ojx.GetMean()
            xerri = ojx.GetMeanError()
        else:
            xi = x[i]
            xerri = xerr[i]
        # get y (single histogram mean or ratio)
        ojy = getobjectfromnick(y, f1, ch)

        # store them in a root graph
        graph.SetPoint(i, xi, ojy.GetMean())
        graph.SetPointError(i, xerri, ojy.GetMeanError())
    if not root:
        return root2histo(graph)
    return graph


def getgraphratio(x, y, f1, f2, opt, changes={}, key='var', var=None, drop=True):
    if f2 is None:
        return getgraph(x, y, f1, opt, changes, key, var, drop)
    graph1 = getgraph(x, y, f1, opt, changes, key, var, drop)
    graph2 = getgraph(x, y, f2, opt, changes, key, var, drop)
    return dividegraphs(graph1, graph2)


# for compatibility
def gethisto(name, rootfile, changes={}, rebin=1):
    print "Please use getplot!"
    return getplot(name, rootfile, changes, rebin)

