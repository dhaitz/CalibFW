

void plot_dr()
{
    gSystem->Load("libGenVector.so");

    using namespace ROOT::Math::VectorUtil;
    using namespace ROOT::Math;
    TChain * t = new TChain( "ak5PFJets_events" );
    t->Add("test_nov4.root");

    TParticle* mu_p = new TParticle();
    TParticle* mu_m = new TParticle();
    TParticle* jet = new TParticle();

    t->SetBranchAddress("mu_plus", &mu_p);
    t->SetBranchAddress("mu_minus", &mu_m);
    t->SetBranchAddress("jet1", &jet);

    TH1D * hist = new TH1D ( "bla", "bla", 100, 0.0, 4.0 );

    double dd;
    XYZVector vec1;
    XYZVector vec2;

    for (int i=0; i < t->GetEntries();++i)
    {
        t->GetEntry( i );
        
        vec1 = XYZVector( mu_m->Px(), mu_m->Py(), mu_m->Pz());
        vec2 = XYZVector( jet->Px(), jet->Py(), jet->Pz());

        dd = DeltaR( vec1, vec2);
        std::cout << dd << std::endl;

        hist->Fill( dd );
    }

    hist->Draw();

//    ROOT::Math::VectorUtil::DeltaR(
}
