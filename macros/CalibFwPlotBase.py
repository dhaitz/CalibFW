from ROOT import TCanvas, TH1D, TF1, TStyle, kTRUE, TFile, TTree, TParticle, TGraphErrors

# colors
from ROOT import kWhite, kRed, kBlue

import math

def safeGet( RootDict, ObjectName ):
    oj = RootDict.Get( ObjectName )
    if not oj:
        print "Cant load " + ObjectName + " from "  + RootDict.GetName()
        assert False

    return oj

def GetDefaultStyle():
  theStyle = TStyle("cholder","cholder")

  # For the canvas:
  theStyle.SetCanvasBorderMode(0)
  theStyle.SetCanvasColor(kWhite)
  theStyle.SetCanvasDefH(600)
  theStyle.SetCanvasDefW(600)
  theStyle.SetCanvasDefX(0)
  theStyle.SetCanvasDefY(0)

  # For the Pad:
  theStyle.SetPadBorderMode(0)  
  theStyle.SetPadColor(kWhite)

  #theStyle.SetPadGridX(m_grid_x)
  #theStyle.SetPadGridY(m_grid_y)
  theStyle.SetGridColor(0)
  theStyle.SetGridStyle(3)
  theStyle.SetGridWidth(1)
  #theStyle.SetPadTopMargin(0.05)
  #theStyle.SetPadBottomMargin(0.13)
  #theStyle.SetPadLeftMargin(0.14)
  #theStyle.SetPadRightMargin(0.08)
  
  # For the frame:
  theStyle.SetFrameBorderMode(0)
  theStyle.SetFrameBorderSize(1)
  theStyle.SetFrameFillColor(0)
  theStyle.SetFrameFillStyle(0)
  theStyle.SetFrameLineColor(1)
  theStyle.SetFrameLineStyle(1)
  theStyle.SetFrameLineWidth(1)

  #For the histo:
  theStyle.SetHistLineColor(1)
  theStyle.SetHistLineStyle(0)
  theStyle.SetHistLineWidth(2)
  theStyle.SetEndErrorSize(2)
  theStyle.SetErrorX(0.)
  theStyle.SetMarkerStyle(20)

  #For the fit/function:
  theStyle.SetOptFit(0)
  theStyle.SetFitFormat("5.4g")
  theStyle.SetFuncColor(1)
  theStyle.SetFuncStyle(1)
  theStyle.SetFuncWidth(1)

  # For the date:
  theStyle.SetOptDate(0);

  # For the statistics box:
  theStyle.SetOptFile(0)
  theStyle.SetOptStat(0)
  theStyle.SetStatColor(kWhite)
  theStyle.SetStatFont(42)
  theStyle.SetStatFontSize(0.025)
  theStyle.SetStatTextColor(1)
  theStyle.SetStatFormat("6.4g")
  theStyle.SetStatBorderSize(1)
  theStyle.SetStatH(0.1)
  theStyle.SetStatW(0.15)

  # Margins:
  #theStyle.SetPadTopMargin(0.05)
  #theStyle.SetPadBottomMargin(0.23)
  #theStyle.SetPadLeftMargin(0.17)
  #theStyle.SetPadRightMargin(0.05)

  # For the Global title:

  theStyle.SetOptTitle(0)
  theStyle.SetTitleFont(42)
  theStyle.SetTitleColor(1)
  theStyle.SetTitleTextColor(1)
  theStyle.SetTitleFillColor(10)
  theStyle.SetTitleFontSize(0.05)

  # For the axis titles:
  theStyle.SetTitleColor(1, "XYZ")
  theStyle.SetTitleFont(42, "XYZ")
  theStyle.SetTitleSize(0.05, "XYZ")
  theStyle.SetTitleXOffset(0.9)
  theStyle.SetTitleYOffset(1.1)

  # For the axis labels:
  theStyle.SetLabelColor(1, "XYZ")
  theStyle.SetLabelFont(42, "XYZ")
  theStyle.SetLabelOffset(0.007, "XYZ")
  theStyle.SetLabelSize(0.045, "XYZ")

  # For the axis:
  theStyle.SetAxisColor(1, "XYZ")
  theStyle.SetStripDecimals(kTRUE)
  theStyle.SetTickLength(0.03, "XYZ")
  theStyle.SetNdivisions(510, "XYZ")
  theStyle.SetPadTickX(1)
  theStyle.SetPadTickY(1)

  # Change for log plots:
  theStyle.SetOptLogx(0)
  theStyle.SetOptLogy(0)
  theStyle.SetOptLogz(0)

  # Postscript options:
  theStyle.SetPaperSize(15.,15.)
  return theStyle


class GraphDataProviderBase:
  def GetPointCount():
    return 23
  
  def GetPoints(self):
    return map ( lambda x: (x, math.sin(x * 0.3), x * 0.2, x * 0.1),range(23))


class PlotBase:
  def Execute( self, dataProvider ):
    return


class GraphPlotBase ( PlotBase ):
  pass

class RootGraphPlot ( GraphPlotBase ):
  
  def Execute( self, dataProviderFunc ):
    c1 = TCanvas('c1', 'Example with Formula', 200, 10, 700, 500 )
    points = dataProviderFunc.GetPoints()
    gr = TGraphErrors( len (points) )
    
    i = 0 
    for ( x, y, ex, ey ) in points:
      gr.SetPoint( i, x, y )
      gr.SetPointError( i, ex, ey )
      i = i + 1
       
    gr.Draw("pa")
    c1.Print("out.png")
    return
    
