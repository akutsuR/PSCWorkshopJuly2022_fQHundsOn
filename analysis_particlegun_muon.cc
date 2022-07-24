#include <iostream>
#include <map>

#include "TFile.h"
#include "TChain.h"
#include "TMath.h"
#include "TH1D.h"

#include "WCSimRootEvent.hh"

using std::cout;
using std::endl;
using std::map;

// Dimension of Hyper-K innter detector
const float kMaxIDR = 6480./2.;  // cm
const float kMaxIDZ = 6575.1/2.; // cm

const double kRad2Deg = 180./TMath::Pi(); // radian -> degree

// Store fiTQun some of 1Rmu variables temporally 
struct fiTQun1R_t
{
    Int_t fqnse; // number of sub-events

    // 1st: sub-event (usually "0" is supposed to be primary charged particle)
    // 2nd: 1-ring hypothesis (1 for electron, 2 for muon, etc)
    // 3rd: x,y,z components
    Float_t fq1rmom[10][7];   // initial momentum 
    Float_t fq1rt0[10][7];    // creation time
    Float_t fq1rpos[10][7][3];  // initial position
    Float_t fq1rdir[10][7][3];   // momentum direction
};

// Store MC truth information about primary muon and 
// subsqeunt Michel electron temporally
struct MCTMuon_t
{
    // primary muon 
    Double_t mom_mu = -9999.; // initial mometum
    Double_t pos_mu[3] = {-99999.}; // initial position
    Double_t dir_mu[3] = {-99999.}; // momentum direction

    // Michel electron
    Bool_t has_mie = false;  // true if the primary muon decayed
    Double_t mom_mie = -9999.; //  initial momentum
    Double_t pos_mie[3] = {-99999.}; // initital position
    Double_t dir_mie[3] = {-99999.}; // momentum direction
    Double_t time_mie = -9999.; // creation time
};

// Fill the histograms
void FillHists(map<TString, TH1D*> &mph1d, MCTMuon_t &mm, fiTQun1R_t &fq)
{
    // MC truth 
    mph1d["true_mom_muon"]->Fill(mm.mom_mu);

    double true_pos_r2 = mm.pos_mu[0]*mm.pos_mu[0] + mm.pos_mu[1]*mm.pos_mu[1];
    mph1d["true_posr2_muon"]->Fill(true_pos_r2);
    mph1d["true_posz_muon"]->Fill(mm.pos_mu[0]);

    if( mm.has_mie )
    {
        mph1d["true_time_mie"]->Fill(mm.time_mie);
    }

    // Reconstrcted 
    double mom_res = -9999.; // (reco. - true)/true momentum
    double pos_res = 0.; // | reco. - true positions |
    double dir_res = 0.; // angle between reco. and true directions
    mom_res = (fq.fq1rmom[0][2] - mm.mom_mu)/mm.mom_mu;
    for(int i=0; i<3; i++)
    {
        pos_res += (fq.fq1rpos[0][2][i] - mm.pos_mu[i])*(fq.fq1rpos[0][2][i] - mm.pos_mu[i]);
        dir_res += fq.fq1rdir[0][2][i] * mm.dir_mu[i];
    }
    pos_res = TMath::Sqrt( pos_res );
    dir_res = TMath::ACos( dir_res ) * kRad2Deg;
    mph1d["reso_mom_muon"]->Fill(mom_res);
    mph1d["reso_pos_muon"]->Fill(pos_res);
    mph1d["reso_dir_muon"]->Fill(dir_res);

    if( fq.fqnse==2 )
    {
        // It is assumed that this event has a muon and a Michel electron
        // So, the electron hypothesis is used for the second sub-event, whereas
        // the muon hypothesis is used for the first sub-event
        double tdiff = (fq.fq1rt0[1][1] - fq.fq1rt0[0][2])/1000.; // ns -> us
        mph1d["tdiff_muon_mie"]->Fill(tdiff);
    }
}

// Initialize TH1D histograms
void InitializeHists1D(map<TString, TH1D*> &mph1d)
{
    mph1d.clear();

    int xbins = -9999;
    double xmin = 9999.;
    double xmax = -9999.;
    TString name = "";
    TString title = "";

    // True muon momentum [MeV/c]
    xbins = 1000;
    xmin = 0.;
    xmax = 1000.;
    name = "true_mom_muon";
    title = "primary muon";
    mph1d[name] = new TH1D(name, title, xbins, xmin, xmax);

    // True muon R^2 position [cm]
    xbins = 100;
    xmin = 0.;
    xmax = kMaxIDR*kMaxIDR;
    name = "true_posr2_muon";
    mph1d[name] = new TH1D(name, title, xbins, xmin, xmax);

    // True muon Z position [cm]
    xbins = 100;
    xmin = -kMaxIDZ;
    xmax = kMaxIDZ;
    name = "true_posz_muon";
    mph1d[name] = new TH1D(name, title, xbins, xmin, xmax);

    // True muon decay time (or production time of Michel electron) [us]
    xbins = 100;
    xmin = 0.;
    xmax = 5.;
    name = "true_time_mie";
    title = "Michel electron";
    mph1d[name] = new TH1D(name, title, xbins, xmin, xmax);

    // Muon momentum resolution
    xbins = 100;
    xmin = -0.3;
    xmax = 0.3;
    name = "reso_mom_muon";
    title = "primary muon";
    mph1d[name] = new TH1D(name, title, xbins, xmin, xmax);

    // Muon poistion resolution
    xbins = 100;
    xmin = 0.;
    xmax = 100.; 
    name = "reso_pos_muon";
    mph1d[name] = new TH1D(name, title, xbins, xmin, xmax);

    // Muon direction resolution
    xbins = 100;
    xmin = 0.;
    xmax = 30.; 
    name = "reso_dir_muon";
    mph1d[name] = new TH1D(name, title, xbins, xmin, xmax);

    // Time difference between muon and Michel electron [us]
    xbins = 100;
    xmin = 0.;
    xmax = 5.; 
    name = "tdiff_muon_mie";
    mph1d[name] = new TH1D(name, title, xbins, xmin, xmax);

}

 
void GetMCTruthMuonInfo(WCSimRootTrigger *aEvent, MCTMuon_t &mm)
{
// This function extracts MC truth informaion about primary muon track 
// and subsequent Michel electron if it is found

    const int n_tracks = aEvent->GetNtrack();
    // For a historical reason, we need to start from i=2
    for(int i=2; i<n_tracks; i++)
    {
        TObject *element = (aEvent->GetTracks())->At(i);
        if(!element){ continue; }
        WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
        if( wcsimroottrack->GetParentId()>1 ){ continue; }

        // Currently, both electrons produced by muon's ionization and Michel-electon are saved, 
        // but there is no variable that can distinguish between the two kinds of electrons.
        // Thus, the below uses time to find Michel-electon
        if( wcsimroottrack->GetParentId()==1 &&
            wcsimroottrack->GetIpnu()==11 &&
            wcsimroottrack->GetTime()<20 /* ns */)
        {
            continue; 
        }

        if( wcsimroottrack->GetParentId()==0 &&
            std::abs(wcsimroottrack->GetIpnu())==13 )
        {
            // Primary muon
            for(int j=0; j<3; j++)
            {
                mm.pos_mu[j] = wcsimroottrack->GetStart(j);
                mm.dir_mu[j] = wcsimroottrack->GetDir(j);
            }
            mm.mom_mu = wcsimroottrack->GetP();
        }
        else if( wcsimroottrack->GetParentId()==1 && 
                 std::abs(wcsimroottrack->GetIpnu())==11 )
        {
            // Michel-electron 
            mm.has_mie = true; 
            for(int j=0; j<3; j++)
            {
                mm.pos_mie[j] = wcsimroottrack->GetStart(j);
                mm.dir_mie[j] = wcsimroottrack->GetDir(j);
            }
            mm.mom_mie = wcsimroottrack->GetP();
            mm.time_mie = wcsimroottrack->GetTime()/1000.; // ns->us
        }
    }
}

int main(int argc, char **argv)
{
    TString wcsim_files = argv[1];
    TString fitqun_files = argv[2];
    TString out_file = argv[3];
   
    // MC truth information will be retrieved from WCSim output - wcsimT
    TChain *tWCSim = new TChain("wcsimT");
    WCSimRootEvent* fSpEvent = 0;
    tWCSim->SetBranchAddress("wcsimrootevent", &fSpEvent);
    tWCSim->Add(wcsim_files);

    // Reconstructed information will be retrieved from fiTQun output - fiTQun
    fiTQun1R_t fq1r;
    TChain *tfiTQun = new TChain("fiTQun");
    tfiTQun->SetBranchAddress("fqnse", &fq1r.fqnse);
    tfiTQun->SetBranchAddress("fq1rmom", fq1r.fq1rmom);
    tfiTQun->SetBranchAddress("fq1rt0", fq1r.fq1rt0);
    tfiTQun->SetBranchAddress("fq1rpos", fq1r.fq1rpos);
    tfiTQun->SetBranchAddress("fq1rdir", fq1r.fq1rdir);
    tfiTQun->Add(fitqun_files);
    
    // TH1D histograms that will be the outputs of this program
    map<TString, TH1D*> mph1d;
    InitializeHists1D(mph1d);

    //cout<<" # entries in WCSim files: " << tWCSim->GetEntries() <<endl;
    //cout<<" # entries in fiTQun files: " << tfiTQun->GetEntries() <<endl;
    const int n_entries = tWCSim->GetEntries();
    cout<<" Start processing " << n_entries <<" events....... " <<endl;
    for(int ientry=0; ientry<n_entries; ientry++)
    {
        tWCSim->GetEntry(ientry);
        tfiTQun->GetEntry(ientry);

        if( ientry%1000==0 )
        {
            cout<<" " << ientry <<" th event is being processed... " <<endl;
        }

        // Get MC truth information 
        WCSimRootTrigger *aEvent = fSpEvent->GetTrigger(0);
        MCTMuon_t mctmuon;
        GetMCTruthMuonInfo(aEvent, mctmuon);
        for(int itrig=1; itrig<fSpEvent->GetNumberOfEvents(); itrig++)
        {
            aEvent = fSpEvent->GetTrigger(itrig);
            GetMCTruthMuonInfo(aEvent, mctmuon);
        }

        FillHists(mph1d, mctmuon, fq1r);
    }


    TFile *fout = new TFile(out_file, "recreate");
    for(map<TString, TH1D*>::iterator itr=mph1d.begin(); itr!=mph1d.end(); itr++)
    {
        itr->second->Write();
    }
    fout->Close();

    cout<<" ==== FINSHED === " <<endl;
}
