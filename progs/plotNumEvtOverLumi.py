from ROOT import TCanvas, TH1D, TF1, TStyle, kTRUE, TFile, TTree, TParticle, TGraphErrors

# colors
from ROOT import kWhite, kRed, kBlue

import CalibFwPlotBase

CalibFwPlotBase.GetDefaultStyle().cd()

pl = CalibFwPlotBase.RootGraphPlot()
print pl
pl.Execute( CalibFwPlotBase.GraphDataProviderBase()) 


