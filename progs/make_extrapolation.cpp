#include <iostream>

#include "common_functions.h"
#include "MinimalParser.h"

#include "TCanvas.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TF1.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TObjString.h"

#include "TMatrixTSym.h"


using namespace common_functions;

//------------------------------------------------------------------------------

enum plot_styles_enum {
     response_data,
     response_mc,
     balance_ratio,
     mpf_ratio};


plot_style plot_styles[4] = {plot_style(22,1.5,2,kBlue),
plot_style(23,1.5,2,kRed),
plot_style(21,1.5,2,kBlue),
plot_style(8,1.5,2,kRed)};


// enum plot_styles_enum {
//      data,
//      pythia_h,
//      herwig_h,
//      balance_g,
//      mpf_g};
//
//
// plot_style plot_styles[5] = {plot_style(8,1.5,2,1),
//                              plot_style(0,0,2,kRed), // No points for them
//                              plot_style(0,0,2,kBlue),// No points for them
//                              plot_style(21,1.5,2,kBlue),
//                              plot_style(8,1.5,2,kRed)};

//------------------------------------------------------------------------------
void add_topline(TCanvas& c){

  c.cd();

//   // add a bit of space
//   c.SetLeftMargin(c.GetLeftMargin()+0.02);
//   c.SetTopMargin(c.GetTopMargin()+0.05);
//   c.SetBottomMargin(c.GetBottomMargin()+0.02);
//
//   TLatex lumi_latex(.18, .93 , "#scale[.7]{CMS}");
//   lumi_latex.SetNDC();
//   lumi_latex.DrawClone();
//
//   TLatex cem_latex(.65, .93 , "#bf{#scale[.7]{36 pb^{-1}      #sqrt{s}= 7 TeV}}");
//   cem_latex.SetNDC();
//   cem_latex.DrawClone();

  double intLumi=36;
  bool wide = false;
  TLatex *latex = new TLatex();
  latex->SetNDC();
  latex->SetTextSize(0.045);

  latex->SetTextAlign(31); // align right
  latex->DrawLatex(wide ? 0.98 : 0.95, 0.96, "#sqrt{s} = 7 TeV");
  if (intLumi > 0.) {
    latex->SetTextAlign(11); // align left
    //latex->DrawLatex(0.15,0.96,Form("CMS preliminary, %.2g nb^{-1}",intLumi));
    latex->DrawLatex(wide ? 0.06 : 0.15, 0.96,
		     Form("CMS preliminary, %.2g pb^{-1}",intLumi));
  }
  else if (intLumi==0) { // simulation
    latex->SetTextAlign(11); // align left
    latex->DrawLatex(wide ? 0.06 : 0.15, 0.96, "CMS simulation (Fall10 QCD)");
  }
  else {
    latex->SetTextAlign(11); // align left
    latex->DrawLatex(0.15,0.96,"CMS preliminary 2010");
  }

  }
//------------------------------------------------------------------------------

class extrapolator {

public:
extrapolator(TString pt_bin,
             double lower_pt_bin_edge,
             TString prefix,
             TString ifilename_data,TString ifilename_mc,
             TString comment_balance,
             TString comment_mpf,
             TString cut_name,vdouble cut_values,
             vString balance_histo_names_data,
             vString mpf_histo_names_data,
             vString balance_histo_names_mc,
             vString mpf_histo_names_mc){

  // Adapt the names to the file structure
  for (unsigned int iname=0;iname< balance_histo_names_data.size(); iname++){
    balance_histo_names_data[iname].ReplaceAll("@PTBIN@",pt_bin);
    mpf_histo_names_data[iname].ReplaceAll("@PTBIN@",pt_bin);
    balance_histo_names_mc[iname].ReplaceAll("@PTBIN@",pt_bin);
    mpf_histo_names_mc[iname].ReplaceAll("@PTBIN@",pt_bin);
    }

  m_lower_pt_bin_edge = lower_pt_bin_edge;

  m_balance_histos_data = getObjects<TH1D>(balance_histo_names_data, ifilename_data);
  m_mpf_histos_data = getObjects<TH1D>(mpf_histo_names_data, ifilename_data);
  m_balance_histos_mc = getObjects<TH1D>(balance_histo_names_mc, ifilename_mc);
  m_mpf_histos_mc = getObjects<TH1D>(mpf_histo_names_mc, ifilename_mc);

  m_cut_values = cut_values;
  m_cut_name = cut_name;

  m_comment_balance = comment_balance;
  m_comment_mpf = comment_mpf;
  m_prefix=prefix;
  m_pt_bin = pt_bin;

  m_extr_balance=m_extr_balancee=0;
  m_extr_mpf=m_extr_mpfe=0;
  m_extr_mc_balance=m_extr_mc_balancee=0;
  m_extr_mc_mpf=m_extr_mc_mpfe=0;

}

void extrapolate(){
  // Build extrapolations

  // DATA BALANCE
  GraphContent extr_balance_data = m_build_extr_graph("Data",m_cut_values,m_balance_histos_data);
  // MC BALANCE
  GraphContent extr_balance_mc = m_build_extr_graph("MC",m_cut_values,m_balance_histos_mc);
  // DATA MPF
  GraphContent extr_mpf_data = m_build_extr_graph("Data",m_cut_values,m_mpf_histos_data);
  // MC MPF
  GraphContent extr_mpf_mc = m_build_extr_graph("MC",m_cut_values,m_mpf_histos_mc);

  //Build ratios
  GraphContent extr_balance_ratio ("Balance",extr_balance_data.divide(extr_balance_mc));
  GraphContent extr_mpf_ratio ("MPF",extr_mpf_data.divide(extr_mpf_mc));

  // Do plots
  do_plot(m_pt_bin+"_response_balance_extrapolation_data_mc",
          "p_{T} balance",
          m_cut_name,
          "Z+jet Balance",
          plot_styles[response_data].setStyle<TGraphErrors>(extr_balance_data.getGraph()),
          plot_styles[response_mc].setStyle<TGraphErrors>(extr_balance_mc.getGraph()),
          "[0]+[1]*(x-[2])");

  do_plot(m_pt_bin+"_response_mpf_extrapolation_data_mc",
          "MPF",
          m_cut_name,
          "Z+jet MPF",
          plot_styles[response_data].setStyle<TGraphErrors>(extr_mpf_data.getGraph()),
          plot_styles[response_mc].setStyle<TGraphErrors>(extr_mpf_mc.getGraph()),
          "[0]+[1]*(x-[2])");

  do_plot(m_pt_bin+"_data_over_mc",
          "Method",
          m_cut_name,
          "Data / MC",
          plot_styles[balance_ratio].setStyle<TGraphErrors>(extr_balance_ratio.getGraph()),
          plot_styles[mpf_ratio].setStyle<TGraphErrors>(extr_mpf_ratio.getGraph()),
          "[0]+[1]*(x-[2])");



}


void get_extrapolation_values_balance(double& val, double& val_err){
  val=m_extr_balance;
  val_err=m_extr_balancee;
  }

void get_extrapolation_values_balance_mc(double& val, double& val_err){
  val=m_extr_mc_balance;
  val_err=m_extr_mc_balancee;
  }

void get_extrapolation_values_mpf(double& val, double& val_err){
  val=m_extr_mpf;
  val_err=m_extr_mpfe;
  }

void get_extrapolation_values_mpf_mc(double& val, double& val_err){
  val=m_extr_mc_mpf;
  val_err=m_extr_mc_mpfe;
  }


private:
vector<TH1D*> m_balance_histos_data;
vector<TH1D*> m_mpf_histos_data;
vector<TH1D*> m_balance_histos_mc;
vector<TH1D*> m_mpf_histos_mc;
vdouble m_cut_values;

TString m_cut_name;
TString m_comment_balance;
TString m_comment_mpf;
TString m_pt_bin;
TString m_prefix;

double m_lower_pt_bin_edge;
double m_extr_balance,m_extr_balancee;
double m_extr_mpf,m_extr_mpfe;
double m_extr_mc_balance,m_extr_mc_balancee;
double m_extr_mc_mpf,m_extr_mc_mpfe;

//------------------------------------------------------------------------------
template <class T>
GraphContent m_build_extr_graph(const char* name,vector<double> cut_values,vector<T*> balance_histos, bool decorrelate=true){

  const unsigned int size = balance_histos.size();
  GraphContent extrapolated_graph;
  extrapolated_graph.setName(name);
  double center=0;
  double center_err=0;

  double point_x=0;
  for (unsigned int ipoint=0;ipoint<size;++ipoint){
    point_x = m_cut_values[ipoint];
    if (point_x*m_lower_pt_bin_edge < 5 and m_lower_pt_bin_edge>0.001 and not (m_cut_name.Contains("Phi")or m_cut_name.Contains("phi")))
      break;

    center=balance_histos[ipoint]->GetMean();
    center_err=balance_histos[ipoint]->GetMeanError();

    // Small study about the precision of RMS
    double sqrtNminus1=TMath::Sqrt(balance_histos[ipoint]->GetEntries()-1);
    double center_err_err = balance_histos[ipoint]->GetRMSError()/sqrtNminus1;

    if (balance_histos[ipoint]->GetEntries()>20)
      extrapolated_graph.addPoint(m_cut_values[ipoint],0,
                                  center,center_err,center_err_err);
    }// end loop on points
  if (decorrelate)
    extrapolated_graph.decorrelate();
  return extrapolated_graph;
  };

//------------------------------------------------------------------------------

void do_plot(TString name,
             TString leg_header,
             TString x_axis_title,
             TString y_axis_title,
             TGraphErrors* g1,
             TGraphErrors* g2,
             TString formula){

  TString g2name(g2->GetName());
  bool do_mpf = true;

  // Get at first x max and x min, y max and y min
  double x1first,y1first,x2first,y2first, x1last,y1last,x2last,y2last;
  double y1first_err, y2first_err;

  g1->GetPoint(0,x1first,y1first);
  g1->GetPoint(g1->GetN()-1,x1last,y1last);
  g2->GetPoint(0,x2first,y2first);
  g2->GetPoint(g2->GetN()-2,x2last,y2last);


  y1first_err = g1->GetErrorY(0);
  y2first_err = g2->GetErrorY(0);

  double x_max = max( max(x1first,x1last), max(x2first,x2last) );
  double y_min = min( min(y1first,y1last), min(y2first,y2last) );
  double y_max = max( max(y1first,y1last), max(y2first,y2last) );

  // Get the TF1s for the extrapolations
  TF1 func1("interpolation1", formula, 0, x_max*1.2);
  func1.SetLineWidth(2);
  func1.SetLineColor(g1->GetLineColor());

  TF1 func2("interpolation2", formula, 0, x_max*1.2);
  func2.SetLineWidth(2);
  func2.SetLineColor(g2->GetLineColor());

  for (int ipar=0;ipar<func1.GetNpar();ipar++){
    func1.SetParameter(ipar,1);
    func2.SetParameter(ipar,1);
    }

  //"[0]+[1]*(x-[2])"
  func1.FixParameter(0, y1first );
  func2.FixParameter(0, y2first );

  func1.FixParameter(2, x1first );
  func2.FixParameter(2, x2first );

  std::cout << std::endl << name.Data() <<  " balance fit" << std::endl;
  TFitResultPtr fitres1p = g1->Fit(&func1,"S");
  std::cout << std::endl << name.Data() <<   " mpf fit" << std::endl;
  TFitResultPtr fitres2p = g2->Fit(&func2,"S");

  // Dummy for x axis
  TH1F dummy_h("dummy","dummy",2,0,x_max*1.3);
  dummy_h.GetXaxis()->SetTitle(x_axis_title);
  dummy_h.GetYaxis()->SetTitle(y_axis_title);

  // Make the bands
  // so ParGradient will not treat this as a constant, but a parameter
  func1.ReleaseParameter(0 );
  func2.ReleaseParameter(0 );

  TMatrixDSym cov1((*fitres1p).GetCovarianceMatrix());
  cov1[0][0] = y1first_err *  y1first_err;
  cov1.Print();
  TGraph** func_1_bands_ud = make_bands_up_down(func1, cov1 );
  TMatrixDSym cov2((*fitres2p).GetCovarianceMatrix());
  cov2[0][0] = y2first_err * y2first_err;
  cov2.Print();
  TGraph** func_2_bands_ud = make_bands_up_down(func2, cov2 );

  TCanvas c("ciccio","pippo",600,600);
  dummy_h.Draw();
  g1->Draw("PESAME");
  if (do_mpf) g2->Draw("PESAME");
  func1.Draw("Same");
  if (do_mpf) func2.Draw("Same");
//   func1_band->Draw("SameE3");
  func_1_bands_ud[0]->Draw("SameC");
  func_1_bands_ud[1]->Draw("SameC");

//   if (do_mpf) func2_band->Draw("SameE3");
  if (do_mpf) {
    func_2_bands_ud[0]->Draw("SameC");
    func_2_bands_ud[1]->Draw("SameC");
    }

  // add a bit of space
  add_topline(c);
//   c.SetLeftMargin(c.GetLeftMargin()+0.02);
//   c.SetTopMargin(c.GetTopMargin()+0.05);
//   c.SetBottomMargin(c.GetBottomMargin()+0.02);
//
//   // TLatex'es
//   TLatex lumi_latex(.18, .93 , "#scale[.6]{#int L = 36 pb^{-1}}");
//   lumi_latex.SetNDC();
//
//   TLatex cem_latex(.78, .93 , "#scale[.6]{#sqrt{s}= 7 TeV}");
//   cem_latex.SetNDC();


  TString comment(m_comment_balance);
  if (leg_header.Contains("MPF")){
//     cout << "  /*-->*/ " << leg_header.Data() << " contains MPF.\n";
    comment=m_comment_mpf;
    }

  TLatex comment_latex(.22, .8 ,comment);
  comment_latex.SetNDC();

  TString ptbin_str(m_pt_bin);
  ptbin_str.ReplaceAll("Pt","#scale[.6]{");
  ptbin_str.ReplaceAll("to"," < p_{T}^{Z} < ");
  ptbin_str+="}";
  TLatex ptbin_latex(.22, .25 ,ptbin_str);
  ptbin_latex.SetNDC();

//   lumi_latex.Draw("Same");
//   cem_latex.Draw("Same");
  comment_latex.Draw("Same");
  ptbin_latex.Draw("Same");

  // Make Legend
  TLegend legend(.65,.7,.88,.85);
  legend.SetFillColor(kWhite);
  legend.SetHeader(leg_header);
  legend.AddEntry(g1,g1->GetName());
  if (do_mpf) legend.AddEntry(g2,g2->GetName());
  legend.Draw("Same");


  if (name.Contains("_data_over_mc"))
    dummy_h.GetYaxis()->SetRangeUser(.9,1.1);
  else
    dummy_h.GetYaxis()->SetRangeUser(y_min*0.7,y_max*1.3);

  c.Print(m_prefix+"_"+name+".png");
  c.Print(m_prefix+"_"+name+".pdf");
  c.Print(m_prefix+"_"+name+".eps");

  // Fill the extrapolation values
  double fit_plus_err_at_zero=0;
  double dummy;

  if (name.Contains("mpf_extrapolation")){
    m_extr_mpf = func1.Eval(0);
    func_1_bands_ud[1]->GetPoint(0,dummy,fit_plus_err_at_zero);
    m_extr_mpfe = fit_plus_err_at_zero-m_extr_mpf;

    m_extr_mc_mpf = func2.Eval(0);
    func_2_bands_ud[1]->GetPoint(0,dummy,fit_plus_err_at_zero);
    m_extr_mc_mpfe = fit_plus_err_at_zero-m_extr_mc_mpf;
    }
  else if (name.Contains("balance_extrapolation")){
    m_extr_balance = func1.Eval(0);
    func_1_bands_ud[1]->GetPoint(0,dummy,fit_plus_err_at_zero);
    m_extr_balancee = fit_plus_err_at_zero-m_extr_balance;


    m_extr_mc_balance = func2.Eval(0);
    func_2_bands_ud[1]->GetPoint(0,dummy,fit_plus_err_at_zero);
    m_extr_mc_balancee = fit_plus_err_at_zero-m_extr_mc_balance;
    }

  };

//------------------------------------------------------------------------------

};

//------------------------------------------------------------------------------

void set_extr_point(int ipoint,
                    extrapolator& the_extrapolator,
                    TGraphErrors* response_graph,
                    TGraphErrors* response_graph_extr){
  double v,ev;
  double x,dummy;
  double ex;

  TString name(response_graph->GetName());

//   cout << " ---> Studying " << name.Data() << endl;

  if (name.Contains("jetresp")){
//     cout << "   ---> We Have Balance\n";
    if (name.Contains("mc")){
//       cout << "   ---> We Have MC\n";
      the_extrapolator.get_extrapolation_values_balance_mc(v,ev);
      }
    else{
//       cout << "   ---> We Have Data\n";
      the_extrapolator.get_extrapolation_values_balance(v,ev);
      }
    }
  else{
//     cout << "   ---> We Have MPF\n";
    if (name.Contains("mc")){
//       cout << "   ---> We Have MC\n";
      the_extrapolator.get_extrapolation_values_mpf_mc(v,ev);
      }
    else{
//       cout << "   ---> We Have Data\n";
      the_extrapolator.get_extrapolation_values_mpf(v,ev);
      }
    }

  response_graph->GetPoint(ipoint,x,dummy);
  ex=response_graph->GetErrorX(ipoint);
  response_graph_extr->SetPoint(ipoint,x,v);
  response_graph_extr->SetPointError(ipoint,ex,ev);
}

//------------------------------------------------------------------------------

void  do_response_plot(TString name, TGraphErrors* graph,TGraphErrors* extr_graph, TString leg_header,TString comment){

  graph->RemovePoint(0);
  extr_graph->RemovePoint(0);

  // Get at first x max and x min, y max and y min
  double x1first,y1first,x2first,y2first, x1last,y1last,x2last,y2last;
  graph->GetPoint(0,x1first,y1first);
  graph->GetPoint(graph->GetN()-1,x1last,y1last);
  extr_graph->GetPoint(0,x2first,y2first);
  extr_graph->GetPoint(extr_graph->GetN()-1,x2last,y2last);

  double x_max = max( max(x1first,x1last), max(x2first,x2last) );
  double y_min = min( min(y1first,y1last), min(y2first,y2last) );
  double y_max = max( max(y1first,y1last), max(y2first,y2last) );

  // Dummy for x axis
  TH1F dummy_h("dummy","dummy",2,0,x_max*1.3);
  dummy_h.GetXaxis()->SetTitle("p_{T}^{Z}");
  if (name.Contains("balance"))
    dummy_h.GetYaxis()->SetTitle("Z+jet Balance");
  else
    dummy_h.GetYaxis()->SetTitle("Z+jet MPF");

  TCanvas c("ciccio","pippo",600,600);
  dummy_h.Draw();

  dummy_h.GetYaxis()->SetRangeUser(y_min*0.7,y_max*1.3);

  graph->Draw("PESAME");
  extr_graph->Draw("PESAME");

  add_topline(c);

  // add a bit of space
//   c.SetLeftMargin(c.GetLeftMargin()+0.02);
//   c.SetTopMargin(c.GetTopMargin()+0.05);
//   c.SetBottomMargin(c.GetBottomMargin()+0.02);
//
//   // TLatex'es
//   TLatex lumi_latex(.18, .93 , "#scale[.6]{#int L = 36 pb^{-1}}");
//   lumi_latex.SetNDC();
//
//   TLatex cem_latex(.78, .93 , "#scale[.6]{#sqrt{s}= 7 TeV}");
//   cem_latex.SetNDC();
//
//
//   lumi_latex.Draw("Same");
//   cem_latex.Draw("Same");
  TLatex comment_latex(.22, .8 ,comment);
  comment_latex.SetNDC();

  comment_latex.Draw("Same");

  // Make Legend
  TLegend legend(.65,.7,.88,.85);
  legend.SetFillColor(kWhite);
  legend.SetBorderSize(0);
  legend.SetHeader(leg_header);
  legend.AddEntry(graph,"Standard");
  legend.AddEntry(extr_graph,"Extrapolated");
  legend.Draw();


  c.Print(name+".png");
  c.Print(name+".pdf");
  c.Print(name+".eps");



  }
//------------------------------------------------------------------------------

TLegend* buildLegend(TString header,
                     TObject* obj1, TString obj1_name,
                     TObject* obj2, TString obj2_name,
                     TObject* obj3, TString obj3_name,
                     TString position){


  TLegend* l = 0;
  if (position.Contains("ur"))
    l = new TLegend(.65,.7,.85,.85);
  if (position.Contains("ul"))
    l = new TLegend(.2,.7,.35,.85);
  if (position.Contains("br"))
    l = new TLegend(.7,.2,.85,.35);
  if (position.Contains("bl"))
    l = new TLegend(.2,.2,.35,.35);

  l->SetFillColor(kWhite);
  l->SetBorderSize(0);

  l->SetHeader(header);
  l->AddEntry(obj1,obj1_name);
  l->AddEntry(obj2,obj2_name);
  if (obj3) l->AddEntry(obj3,obj3_name);


  return l;

  };

//------------------------------------------------------------------------------

void make_jme1010(TString name,
                  TString x_axis_name,
                  TString y_axis_name,
                  TString comment,
                  TString extr_cut,
                  TGraphErrors* data_bal,
                  TGraphErrors* data_mpf,
                  TGraphErrors* mc_bal,
                  TGraphErrors* mc_mpf){

  // Title offset
  data_bal->GetYaxis()->SetTitleOffset(1.5);
  data_bal->GetYaxis()->SetRangeUser(.9,1.1);

  // Divide the Graphs
  GraphContent balance_ratio_gc(*data_bal);
  GraphContent balance_divisor_gc(*mc_bal);
  balance_ratio_gc.divide_inplace(balance_divisor_gc);
  TGraphErrors* balance_ratiog = balance_ratio_gc.getGraph();

  GraphContent mpf_ratio_gc(*data_mpf);
  GraphContent mpf_divisor_gc(*mc_mpf);
  mpf_ratio_gc.divide_inplace(mpf_divisor_gc);
  TGraphErrors* mpf_ratiog = mpf_ratio_gc.getGraph();

  // Set The Styles
  balance_ratiog = plot_styles[balance_ratio].setStyle<TGraphErrors>(balance_ratiog);
  mpf_ratiog = plot_styles[mpf_ratio].setStyle<TGraphErrors>(mpf_ratiog);

  // The Legend
  TLegend* leg = buildLegend("Z+jet",
                             balance_ratiog,"p_{T} Balance",
                             mpf_ratiog,"MPF",
                             0,"",
                             "ur");


  // Build a mega TGraphErrors with all points and errors!
  balance_ratio_gc.addGraphContent(mpf_ratio_gc);
  TGraphErrors megag (*balance_ratio_gc.getGraph());

  megag.Print();

  // Plug In and write on file
  TFile jme1010Plot(name+".root","RECREATE");
  megag.Write("combined_balance_mpf");
  balance_ratiog->Write("balnce");
  mpf_ratiog->Write("mpf");
  jme1010Plot.Close();


  TGraphErrors megag_dummy(megag);
  megag_dummy.SetPoint(0,-100,0);
  megag_dummy.SetPoint(1,250,2);

  // Make the first drawn beautiful
  megag_dummy.GetXaxis()->SetTitle(x_axis_name);
  megag_dummy.GetYaxis()->SetTitle(y_axis_name);
  megag_dummy.GetXaxis()->SetMoreLogLabels();

  // Perform the fit!
  TF1 fitf("FittingFunction", "[0]+[1]*log(x)", 20, 200);
  fitf.SetLineWidth(2);
  TFitResultPtr fitresp = megag.Fit(&fitf,"LSR");
  fitresp = megag.Fit(&fitf,"LSR");

  // The band(s)
  TMatrixDSym cov((*fitresp).GetCovarianceMatrix());
  TGraphErrors* yellowband = make_band(fitf,cov);
  yellowband->SetFillColor(kYellow);
  yellowband->SetFillStyle(1);

  TGraph** updownband = make_bands_up_down(fitf,cov);

  // Latex for the comment
  TLatex commentlatex(.2,.8,comment);
  commentlatex.SetNDC();

  // Latex for the extrapol
  TLatex extrcutlatex(.6,.2,extr_cut);
  extrcutlatex.SetNDC();

  // Draw Everything

  TCanvas c("ciccio","pippo",600,600);
  c.cd();

  c.SetLogx();

  megag_dummy.Draw("APE");
  megag.Draw("SamePE");  // To be sure of the procedure
  yellowband->Draw("E4");
  fitf.Draw("CSame");
  updownband[0]->Draw("SameC");
  updownband[1]->Draw("SameC");
  balance_ratiog->Draw("PE1Same");
  mpf_ratiog->Draw("PE1Same");
  add_topline(c);
  megag_dummy.GetYaxis()->SetRangeUser(.81,1.19);
  megag_dummy.GetXaxis()->SetRangeUser(20,250);
  leg->Draw();

  commentlatex.Draw();
  extrcutlatex.Draw();

  c.Print(name+".png");
  c.Print(name+".pdf");
  c.Print(name+".eps");

  delete leg;


  }

//------------------------------------------------------------------------------

int main(int argc, char** argv){

setTDRStyle_mod();
gStyle->SetOptStat(0);
gStyle->SetOptFit(0);

if (argc<3){
    cout << "Usage: " << argv[0] << " cfgfile.py section_name\n";
    return -1;
    }

TString cfg_name(argv[1]);

for (int iarg=2;iarg<argc;iarg++){

  TString title(argv[iarg]);

  //----------------

  MinimalParser p(cfg_name);
  p.setVerbose(false);

  TString filename_data (p.getString(title+".filename_data"));
  TString filename_mc (p.getString(title+".filename_mc"));

  TString comment_balance (p.getString(title+".comment_balance"));
  TString comment_mpf (p.getString(title+".comment_mpf"));

  vdouble cut_vals (p.getvDouble(title+".cut_vals"));
  TString cut_name (p.getString(title+".cut_name"));

  vString  balance_histo_names_data(p.getvString(title+".balance_histo_names_data"));
  vString mpf_histo_names_data(p.getvString(title+".mpf_histo_names_data"));

  vString balance_histo_names_mc(p.getvString(title+".balance_histo_names_mc"));
  vString mpf_histo_names_mc(p.getvString(title+".mpf_histo_names_mc"));

  vString pt_bins(p.getvString(title+".pt_bins"));


  TString mpf_response_graph_name_data(p.getString(title+".mpf_response_graph_name_data"));
  TString balance_response_graph_name_data(p.getString(title+".balance_response_graph_name_data"));
  TString mpf_response_graph_name_mc(p.getString(title+".mpf_response_graph_name_mc"));
  TString balance_response_graph_name_mc(p.getString(title+".balance_response_graph_name_mc"));

  //----------------

  TGraphErrors* balance_response_graph=getObject<TGraphErrors>(balance_response_graph_name_data,filename_data);
  TGraphErrors* mpf_response_graph=getObject<TGraphErrors>(mpf_response_graph_name_data,filename_data);

  TGraphErrors* balance_response_graph_mc=getObject<TGraphErrors>(balance_response_graph_name_mc,filename_mc);
  TGraphErrors* mpf_response_graph_mc=getObject<TGraphErrors>(mpf_response_graph_name_mc,filename_mc);

  const int npoints=balance_response_graph_mc->GetN();

  TGraphErrors* balance_response_graph_extr= new TGraphErrors (npoints);
  balance_response_graph_extr->SetName("balance_response_graph_extr");
  balance_response_graph_extr->SetTitle("balance_response_graph_extr;p_{T}^{Z};Z+jet Balance");

  TGraphErrors* mpf_response_graph_extr= new TGraphErrors (npoints);
  mpf_response_graph_extr->SetName("mpf_response_graph_extr");
  mpf_response_graph_extr->SetTitle("mpf_response_graph_extr;p_{T}^{Z};Z+jet MPF");

  TGraphErrors* balance_response_graph_extr_mc= new TGraphErrors (npoints);
  balance_response_graph_extr_mc->SetName("balance_response_graph_extr_mc");
  balance_response_graph_extr_mc->SetTitle("balance_response_graph_extr_mc;p_{T}^{Z};Z+jet Balance");

  TGraphErrors* mpf_response_graph_extr_mc= new TGraphErrors (npoints);
  mpf_response_graph_extr_mc->SetName("mpf_response_graph_extr_mc");
  mpf_response_graph_extr_mc->SetTitle("mpf_response_graph_extr_mc;p_{T}^{Z};Z+jet MPF");

  int bincounter=0;
  vString::iterator ibin;
  for (ibin=pt_bins.begin();ibin!=pt_bins.end();ibin++){

    // Get bin low edge from name
    TString dummy(*ibin);
    dummy.ReplaceAll("Pt","");
    dummy = ((TObjString*)(dummy.Tokenize("to")->At(0)))->GetString();

    cout << " ******** " << dummy.Data() << " " << dummy.Atof() << endl;

    extrapolator the_extrapolator(*ibin,
                                  dummy.Atof(),
                                  title,
                                  filename_data,filename_mc,
                                  comment_balance,
                                  comment_mpf,
                                  cut_name, cut_vals,
                                  balance_histo_names_data,mpf_histo_names_data,
                                  balance_histo_names_mc,mpf_histo_names_mc);
    the_extrapolator.extrapolate();

    if (bincounter<npoints){

      cout << "*** Processing bin " << ibin->Data() << " for the extrapolation plot.\n";

      // Balance Data
      set_extr_point(bincounter,
                     the_extrapolator,
                     balance_response_graph,
                     balance_response_graph_extr);
      // MPF Data
      set_extr_point(bincounter,
                     the_extrapolator,
                     mpf_response_graph,
                     mpf_response_graph_extr);

      // Balance MC
      set_extr_point(bincounter,
                     the_extrapolator,
                     balance_response_graph_mc,
                     balance_response_graph_extr_mc);
      // MPF MC
      set_extr_point(bincounter,
                     the_extrapolator,
                     mpf_response_graph_mc,
                     mpf_response_graph_extr_mc);
      }
    else
      cout << "*** Skipping bin " << ibin->Data() << " for the extrapolation plot.\n";
    bincounter++;
    } // end loop on pt bins

  setTDRStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  do_response_plot(title+"_extr_resp_balance_data",
                   plot_styles[2].setStyle<TGraphErrors>(balance_response_graph),
                   plot_styles[3].setStyle<TGraphErrors>(balance_response_graph_extr),
                   "P_{T} Balance",comment_balance);

  do_response_plot(title+"_p_mpf_data",
                   plot_styles[2].setStyle<TGraphErrors>(mpf_response_graph),
                   plot_styles[3].setStyle<TGraphErrors>(mpf_response_graph_extr),
                   "MPF",comment_mpf);

  do_response_plot(title+"_p_balance_mc",
                   plot_styles[2].setStyle<TGraphErrors>(balance_response_graph_mc),
                   plot_styles[3].setStyle<TGraphErrors>(balance_response_graph_extr_mc),
                   "P_{T} Balance",comment_balance);

  do_response_plot(title+"_p_mpf_mc",
                   plot_styles[2].setStyle<TGraphErrors>(mpf_response_graph_mc),
                   plot_styles[3].setStyle<TGraphErrors>(mpf_response_graph_extr_mc),
                   "MPF",comment_mpf);


  TString extr_comment("#scale[.6]{");
  extr_comment+=cut_name+" #rightarrow 0}";
  make_jme1010(title+"_balance_data_over_pythia_mc_extrapolation",
               "p_{T}^{Z}",
               "Data/Pythia Z2",
               comment_balance,
               extr_comment,
               balance_response_graph_extr,
               mpf_response_graph_extr,
               balance_response_graph_extr_mc,
               mpf_response_graph_extr_mc);

  make_jme1010(title+"_balance_data_over_pythia_mc",
               "p_{T}^{Z}",
               "Data/Pythia Z2",
               comment_balance,
               "",
               balance_response_graph,
               mpf_response_graph,
               balance_response_graph_mc,
               mpf_response_graph_mc);


  TFile ofile(title+"_ofile.root","RECREATE");
  balance_response_graph_extr->Write();
  mpf_response_graph_extr->Write();
  balance_response_graph_extr_mc->Write();
  mpf_response_graph_extr_mc->Write();
  ofile.Close();

  }// end of config section

}// end of program













