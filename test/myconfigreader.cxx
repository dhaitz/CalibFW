{

gSystem->Load("libCalibFW.so");

MinimalParser p("myconfig.conf");
p.setVerbose(true);

double mydouble;
p.read("mysection","mydouble",mydouble);
cout << "mydouble = " << mydouble << endl;

TString mystr;

p.read("mysection","mystring",mystr);
cout << "mystring = #" << mystr.Data() << "#"<< endl;

std::vector<TString> mystrv;
p.read("mysection","mystringv",mystrv);
for (int i=0;i<mystrv.size();++i)
    cout << " - mystrv["<<i<<"] = " << mystrv[i].Data() << endl;

std::vector<double> mydoublev;
p.read("mysection","mydoublev",mydoublev);
for (int i=0;i<mydoublev.size();++i)
    cout << " - mydoublev["<<i<<"] = " << mydoublev[i] << endl;

//Try to read from imported file

double mydouble2;
p.read("mysection2","mydouble2",mydouble2);
cout << "mydouble2 = " << mydouble2 << endl;


}