//ROOT
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TString.h>
#include <TGraph.h>
//c++
#include <iostream>

/* POUR LES PROGRAMMES :
"source_x2000.0_y0.0_z0.0_mm_661.7keV_r_hole0.0mm_cuts_e2.0_p2.0_g0.1_mm_SegClusterM2.root"
"source_x2000.0_y0.0_z0.0_mm_661.7keV_r_hole15.0mm_cuts_e2.0_p2.0_g0.1_mm_SegClusterM2.root"
"source_x2000.0_y0.0_z0.0_mm_661.7keV_r_hole30.0mm_cuts_e2.0_p2.0_g0.1_mm_SegClusterM2.root"
*/

void tree_realistic(TString filename) {
    if (filename.IsNull()) {
        std::cerr << "Erreur : Aucun fichier fourni !" << std::endl;
        return;
    }
    TFile *file = TFile::Open(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier ROOT." << std::endl;
        return;
    }

    // Récupérer l'arbre Hits
    TTree *tree = (TTree*)file->Get("Hits");
    if (!tree) {
        std::cerr << "Erreur : Impossible de trouver l'arbre Hits." << std::endl;
        file->Close();
        return;
    }
    
    // Déclarer les variables pour stocker les valeurs des branches
    Double_t TotalEnergyDeposit, GlobalTime, Position_X, Position_Y, Position_Z;
    Bool_t is_gamma;
    Int_t EventID, ParentID;
    Char_t Segment, Slice;
    Char_t ParticleName[50];

    // Associer les variables aux branches
    tree->SetBranchAddress("EventID", &EventID);
    tree->SetBranchAddress("Segment", &Segment);
    tree->SetBranchAddress("TotalEnergyDeposit", &TotalEnergyDeposit);
    tree->SetBranchAddress("is_gamma",&is_gamma);
    tree->SetBranchAddress("GlobalTime", &GlobalTime);
    tree->SetBranchAddress("ParticleName", &ParticleName);
    tree->SetBranchAddress("Slice", &Slice);
    tree->SetBranchAddress("ParentID", &ParentID);
    tree->SetBranchAddress("Position_X", &Position_X);
    tree->SetBranchAddress("Position_Y", &Position_Y);
    tree->SetBranchAddress("Position_Z", &Position_Z);

    // Lire les premières entrées et afficher les valeurs
    Long64_t nEntries = tree->GetEntries();
    std::cout << "Nombre total d'entrées : " << nEntries << std::endl;
    
    for (Long64_t i = 0; i < 12; i++) {
        tree->GetEntry(i);
        Int_t Segment_int = (Int_t)Segment;
    	Int_t Slice_int = (Int_t)Slice;
        std::cout << "Entrée " << i << " : "
                  << " EventID = " << EventID
                  << " Segment = " << Segment_int
                  << " E_dep = " << TotalEnergyDeposit
                  << " is_gamma = " << is_gamma
                  << " Time = " << GlobalTime
                  << " Particle = " << ParticleName
                  << " Slice = " << Slice_int
                  << " ParentID = " << ParentID
                  << " X = " << Position_X
                  << " Y = " << Position_Y
                  << " Z = " << Position_Z
                  << std::endl;
    }

    
   
    TH1F* his1 = new TH1F("his1", "Deposit energy spectrum (15mm)", 100, 0, 700);
    his1->GetXaxis()->SetTitle("Deposit energy (MeV)");
    his1->GetYaxis()->SetTitle("Number of events");
    for (Long64_t i = 0; i < nEntries; i++) {
    	tree->GetEntry(i);
    	his1->Fill(TotalEnergyDeposit);
    }
    TCanvas* can1 = new TCanvas("can1", "can1", 800, 800);
    his1->Draw("colz");
    can1->SaveAs("energy_spectrum_15mm.root");
    
    
    
    TH2F* his2 = new TH2F("his2", "Position Y vs. position X (15mm)",
    			82, -40, 40, // X axis
			82, -40, 40); // Y axis
    his2->GetXaxis()->SetTitle("X");
    his2->GetYaxis()->SetTitle("Y");
    for (Long64_t i = 0; i < nEntries; i++) {
    	tree->GetEntry(i);
	his2->Fill(Position_X, Position_Y);
    }
    TCanvas* can2 = new TCanvas("can2", "can2", 800, 800);
    his2->Draw("colz");
    can2->SaveAs("y_vs_x_15mm.root");
    
    
    
    TH3F* his3 = new TH3F("his3", "pos Z vs. pos Y vs. pos X (15mm)",
    			82, -40, 40, // X axis
			82, -40, 40, // Y axis
			82, -40, 40); // Z axis
    his3->GetXaxis()->SetTitle("X");
    his3->GetYaxis()->SetTitle("Y");
    his3->GetZaxis()->SetTitle("Z");
    for (Long64_t i = 0; i < 10000; i++) {
    	tree->GetEntry(i);
	his3->Fill(Position_X, Position_Y, Position_Z);
    }
    TCanvas* can3 = new TCanvas("can3", "can3", 800, 800);
    his3->Draw("colz"); // ou lego
    can3->SaveAs("z_vs_y_vs_x_15mm.root");
    
    
    /*
    TGraph *graph = new TGraph();
    for (Long64_t i = 0; i < 1000; i++) {
    	tree->GetEntry(i);
    	graph->SetPoint(i, Position_X, Position_Z);
    }
    TCanvas *cancan = new TCanvas("cancan", "X vs Z", 800, 800);
    graph->SetTitle("Position X vs Position Z; X; Z");
    graph->SetMarkerStyle(20);  // Style de point
    graph->SetMarkerColor(kBlue);
    graph->Draw("AP");  // "A" pour les axes, "P" pour les points
    cancan->SaveAs("graph_X_vs_Z_15mm.root");
    */
    //file->Close();
}
