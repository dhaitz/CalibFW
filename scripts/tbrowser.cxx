void tbrowser(){
    gSystem->Load("libKappa.so");
    gSystem->Load("libFWCoreFWLite");
    AutoLibraryLoader::enable();
    static TBrowser t;
}
