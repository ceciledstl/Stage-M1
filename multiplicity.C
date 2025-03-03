//ROOT
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TString.h>
#include <TGraph.h>
//c++
#include <iostream>

void multiplicity(TString filename) {
    // Ouverture du fichier et récupération de l'arbre
    if (filename.IsNull()) {
        std::cerr << "Erreur : Aucun fichier fourni !" << std::endl;
        return;
    }
    TFile *file = TFile::Open(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier ROOT." << std::endl;
        return;
    }
    TTree *tree = (TTree*)file->Get("Hits");
    if (!tree) {
        std::cerr << "Erreur : Impossible de trouver l'arbre Hits." << std::endl;
        file->Close();
        return;
    }
    
    // Création des variables
    Int_t EventID;
    Double_t TotalEnergyDeposit;
    tree->SetBranchAddress("EventID", &EventID);
    tree->SetBranchAddress("TotalEnergyDeposit", &TotalEnergyDeposit);
    Int_t lastEventID = -1; // impossible value
    Int_t multiplicity = 1;
    Double_t TotalEnergy = 0.0;
    
    // Nombre d'entrées totales
    Long64_t nEntries = tree->GetEntries();
    std::cout << "Nombre total d'entrées : " << nEntries << std::endl;
    
    // Création des histogrammes
    TH1F* his = new TH1F("his", "Multiplicity distribution", 20, 0, 20);
    his->GetXaxis()->SetTitle("Multiplicity");
    his->GetYaxis()->SetTitle("Number of events");
    TH1F* spectrum1 = new TH1F("spectrum1", "Energy Spectrum for Multiplicity 1 (15mm)", 100, 0, 700);
    spectrum1->GetXaxis()->SetTitle("Deposit energy (MeV)");
    spectrum1->GetYaxis()->SetTitle("Number of events");
    TH1F* spectrum2 = new TH1F("spectrum2", "Energy Spectrum for Multiplicity 2 (15mm)", 100, 0, 700);
    spectrum2->GetXaxis()->SetTitle("Deposit energy (MeV)");
    spectrum2->GetYaxis()->SetTitle("Number of events");
    TH1F* spectrum3 = new TH1F("spectrum3", "Energy Spectrum for Multiplicity 3 (15mm)", 100, 0, 700);
    spectrum3->GetXaxis()->SetTitle("Deposit energy (MeV)");
    spectrum3->GetYaxis()->SetTitle("Number of events");
    TH1F* spectrum4 = new TH1F("spectrum4", "Energy Spectrum for Multiplicity 4 (15mm)", 100, 0, 700);
    spectrum4->GetXaxis()->SetTitle("Deposit energy (MeV)");
    spectrum4->GetYaxis()->SetTitle("Number of events");
    
    // Remplissage des histogrammes
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        if (EventID == lastEventID) {
            multiplicity++;
            TotalEnergy += TotalEnergyDeposit;
        } 
        else {
            if (lastEventID != -1) {
            	if (multiplicity == 1) {
        			spectrum1->Fill(TotalEnergy);
    	    	}
    	    	if (multiplicity == 2) {
        			spectrum2->Fill(TotalEnergy);
    	    	}
    	    	if (multiplicity == 3) {
        			spectrum3->Fill(TotalEnergy);
    	    	}
    	    	if (multiplicity == 4) {
        			spectrum4->Fill(TotalEnergy);
    	    	}
                his->Fill(multiplicity); 
            }
            multiplicity = 1;
            TotalEnergy = TotalEnergyDeposit;
            lastEventID = EventID;
        }
    }
    
    // Affichage des histogrammes
    TCanvas* can = new TCanvas("can", "Spectra for several multiplicities", 800, 800);
    can->Divide(2, 2);
    can->cd(1);
    spectrum1->Draw();
    can->cd(2);
    spectrum2->Draw();
    can->cd(3);
    spectrum3->Draw();
    can->cd(4);
    spectrum4->Draw();
    can->SaveAs("multiplicity_spectra_15mm.root");
    
    // Calcul d'intégrale des spectres (totales, et des pics)
    Double_t T1 = spectrum1->Integral();
    Double_t T2 = spectrum2->Integral();
    Double_t T3 = spectrum3->Integral();
    Double_t T4 = spectrum4->Integral();
    std::cout << " T1 = " << T1
    		  << " T2 = " << T2
    		  << " T3 = " << T3
    		  << " T4 = " << T4
                  << std::endl;
    Int_t BinMin = spectrum1->FindBin(658);
    Int_t BinMax = spectrum1->FindBin(665);
    Double_t P1 = spectrum1->Integral(BinMin,BinMax);
    Double_t P2 = spectrum2->Integral(BinMin,BinMax);
    Double_t P3 = spectrum3->Integral(BinMin,BinMax);
    Double_t P4 = spectrum4->Integral(BinMin,BinMax);
    std::cout << " P1 = " << P1
    		  << " P2 = " << P2
    		  << " P3 = " << P3
    		  << " P4 = " << P4
                  << std::endl;
    std::cout << " P1/T1 = " << P1/T1
    		  << " P2/T2 = " << P2/T2
    		  << " P3/T3 = " << P3/T3
    		  << " P4/T4 = " << P4/T4
                  << std::endl;
    
    // Mise en graphique
    TGraph *graph1 = new TGraph();
    graph1->SetTitle("P/T (blue) and T (red) vs. multiplicity (15mm)");
    graph1->SetMarkerColor(kRed);
    graph1->SetLineColor(kRed);
    graph1->SetMarkerSize(1);
    graph1->SetMarkerStyle(20);
    graph1->SetPoint(0, 1, P1/T1);
    graph1->SetPoint(1, 2, P2/T2);
    graph1->SetPoint(2, 3, P3/T3);
    graph1->SetPoint(3, 4, P4/T4);
    
    // Mise en graphique
    TGraph *graph2 = new TGraph();
    graph2->SetMarkerColor(kBlue);
    graph2->SetLineColor(kBlue);
    graph2->SetPoint(0, 1, T1/T1);
    graph2->SetPoint(1, 2, T2/T1);
    graph2->SetPoint(2, 3, T3/T1);
    graph2->SetPoint(3, 4, T4/T1);
    
    // Affichage des graphiques
    TCanvas* cancan = new TCanvas("cancan", "Multiplicity and ratio", 800, 800);
    /*
    cancan->Divide(1, 2);
    cancan->cd(1);
    his->Draw();
    cancan->cd(2);
    */
    graph1->GetXaxis()->SetLimits(1,5);
	graph1->GetYaxis()->SetRangeUser(0,1);
    graph1->Draw("APL");
    graph2->Draw("PLsame");
    cancan->SaveAs("P_over_T_graph_15mm.root");

    //file->Close();
}
