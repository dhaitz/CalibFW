{
//gSystem.Load("libKappa.so")

TChain chain("Events");
chain.Add("../kappa44_MC11/*.root");
TH1F* npu = new TH1F("npu","npu",51,-0.5,50.5);
chain.Draw("KEventMetadata.numPUInteractionsTruth >> npu");

TFile * file = TFile::Open("pudist_Fall11.root","RECREATE");
npu->Write();
file->Write();
file->Close();
}
