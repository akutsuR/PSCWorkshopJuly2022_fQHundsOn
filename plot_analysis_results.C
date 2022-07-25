// This ROOT macro plots basic distributions about fiTQun 1Rmu results, using output of "analysis_particlegun_muon"
void plot_analysis_results()
{
    TString file_name = "out_analysis_particlegun_muon.root";
    TFile *f = TFile::Open(file_name);
    TH1D *hmu_pos_res = (TH1D*)f->Get("reso_pos_muon");
    TH1D *hmu_dir_res = (TH1D*)f->Get("reso_dir_muon");
    TH1D *hmu_mom_res = (TH1D*)f->Get("reso_mom_muon");
    TH1D *htdiff_true = (TH1D*)f->Get("true_time_mie");
    TH1D *htdiff_reco = (TH1D*)f->Get("tdiff_muon_mie");
    hmu_pos_res->SetDirectory(0);
    hmu_dir_res->SetDirectory(0);
    hmu_mom_res->SetDirectory(0);
    htdiff_true->SetDirectory(0);
    htdiff_reco->SetDirectory(0);
    f->Close();

    hmu_pos_res->SetLineWidth(2);
    hmu_dir_res->SetLineWidth(2);
    hmu_mom_res->SetLineWidth(2);
    htdiff_true->SetLineWidth(2);
    htdiff_reco->SetLineWidth(2);
    hmu_pos_res->SetLineColor(kAzure-1);
    hmu_dir_res->SetLineColor(kAzure-1);
    hmu_mom_res->SetLineColor(kAzure-1);
    htdiff_true->SetLineColor(kAzure-1);
    htdiff_reco->SetLineColor(kOrange+9);

    const Int_t nRow = 2;
    const Int_t nCol = 2;
    TCanvas *c =new TCanvas("c", "", 696*nRow, 474*nCol);
    c->Divide(nRow, nCol, 0.0001, 0.0001);
    Int_t n = 0;
    for(Int_t i=0; i<nRow; i++)
    {
        for(Int_t j=0; j<nCol; j++)
        {
            TPad *pd = (TPad*)c->cd(n+1);
            pd->SetTopMargin( 0.15 );
            pd->SetLeftMargin( 0.15 );
            pd->SetRightMargin( 0.15 );
            pd->SetTicks(1);
            TString y_title = "Counts";
            TString x_title = "";
            TString fig_title = "";
            Double_t xmin = 99999.;
            Double_t xmax = -99999.;
            Double_t ymin = 0.;
            Double_t ymax = -99999.;

            if( n==0 ) // position resolution
            {
                fig_title = "1R#mu position resolution";
                x_title = "|#bf{r}^{reco} - #bf{r}^{true}| (cm)";
                xmin = hmu_pos_res->GetXaxis()->GetXmin();
                xmax = hmu_pos_res->GetXaxis()->GetXmax();
                ymax = hmu_pos_res->GetMaximum();
            }
            else if( n==1 ) // direction resolution
            {
                fig_title = "1R#mu direction resolution";
                x_title = "cos^{-1}(#bf{d}^{reco}#bullet#bf{d}^{true}) (degree)";
                xmin = hmu_dir_res->GetXaxis()->GetXmin();
                xmax = hmu_dir_res->GetXaxis()->GetXmax();
                ymax = hmu_dir_res->GetMaximum();
            }
            else if( n==2 ) // momentum resolution
            {
                fig_title = "1R#mu momentum resolution";
                x_title = "(P^{reco} - P^{true})/P^{true}";
                xmin = hmu_mom_res->GetXaxis()->GetXmin();
                xmax = hmu_mom_res->GetXaxis()->GetXmax();
                ymax = hmu_mom_res->GetMaximum();
            }
            else if( n==3 ) // time difference between muon and Michel electron 
            {
                fig_title = "Muon decay time";
                x_title = "Time difference (#mus)";
                xmin = htdiff_true->GetXaxis()->GetXmin();
                xmax = htdiff_true->GetXaxis()->GetXmax();
                ymax = htdiff_true->GetMaximum();
            }
            else
            {
                std::cout<<" n: " << n <<" is not supported yet -> EXIT " <<endl;
                exit(-1);
            }

            TH1F *frame = (TH1F*)pd->DrawFrame(xmin, ymin, xmax, ymax*1.1, "");
            frame->GetXaxis()->CenterTitle();
            frame->GetYaxis()->CenterTitle();
            frame->GetXaxis()->SetTitleOffset(0.9);
            frame->GetYaxis()->SetTitleOffset(1.0);
            frame->GetYaxis()->SetTitleSize(0.055);
            frame->GetXaxis()->SetTitleSize(0.055);
            frame->GetYaxis()->SetLabelSize(0.045);
            frame->GetXaxis()->SetTitle( x_title );
            frame->GetYaxis()->SetTitle( y_title );
            frame->GetXaxis()->SetNdivisions(505);
            frame->GetYaxis()->SetNdivisions(505);

            if( n==0 ) 
            {
                hmu_pos_res->Draw("hist same");
            }
            else if( n==1 )
            {
                hmu_dir_res->Draw("hist same");
            }
            else if( n==2 )
            {
                hmu_mom_res->Draw("hist same");
            }
            else if( n==3 ) // time difference between muon and Michel electron 
            {
                htdiff_true->Draw("hist same");
                htdiff_reco->Draw("hist same");
            
                TLegend *leg = new TLegend(0.45, 0.675, 0.8, 0.805);
                leg->SetFillColor(0);
                leg->SetBorderSize(0);
                leg->SetTextSize(0.065);
                leg->AddEntry(htdiff_true, "MC truth", "l");
                leg->AddEntry(htdiff_reco, "Reconstructed", "l");
                leg->Draw();
            }

            pd->RedrawAxis();
            pd->Modified();

            TLatex tx;
            tx.SetNDC(1);
            tx.SetTextSize(0.07);
            tx.DrawLatex(0.175, 0.875, fig_title);

            n++;
        }
    }
    c->Print("./analysis_results.png");
}
