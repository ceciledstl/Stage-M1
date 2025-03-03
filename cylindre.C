#include <TCanvas.h>
#include <TH3F.h>
#include <TPolyLine3D.h>
#include <TGraph2D.h>
#include <TEllipse.h>
#include <TPolyLine.h>
#include <TPolyMarker3D.h>
#include <iostream>
#include <cmath>

struct Point {
    double_t x, y, z;
};

bool InCylinder(double_t x, double_t y, double_t z, double_t holeRadius, double_t cylinderRadius, double_t holeZMax, double_t holeZMin,  double_t cylinderHeight) {
    bool insideCylinder = (x*x+y*y)<=(cylinderRadius*cylinderRadius) && z>=-cylinderHeight && z<=cylinderHeight; // point au sein du cylindre Germanium et compris dans toute sa hauteur 
    bool outsideHole = (z>holeZMax) || (z<holeZMin) || ((x*x+y*y)>(holeRadius*holeRadius)); // point en dehors du rayon du cylindre vide dans toute sa hauteur
    return insideCylinder && outsideHole;
}

bool InHole(double_t x, double_t y, double_t z, double_t holeRadius, double_t holeZMin, double_t holeZMax) {
    bool insideHole = (x*x+y*y)<(holeRadius*holeRadius) && z>holeZMin && z<holeZMax;
    return insideHole;
}

double_t HolePath(const Point& p1, const Point& p2, double_t holeRadius, double_t holeZMin, double_t holeZMax, double_t cylinderRadius, double_t cylinderHeight) {
    if (!InCylinder(p1.x, p1.y, p1.z, holeRadius, cylinderRadius, holeZMax, holeZMin, cylinderHeight) ||
        !InCylinder(p2.x, p2.y, p2.z, holeRadius, cylinderRadius, holeZMax, holeZMin, cylinderHeight)) {
        std::cout<<"Un ou les deux points ne sont pas dans la zone Germanium du cylindre."<<std::endl;
        return 0.0;
    }
    
    double_t vx=p2.x-p1.x;
    double_t vy=p2.y-p1.y;
    double_t vz=p2.z-p1.z;
    double_t Ltot=std::sqrt(vx*vx+vy*vy+vz*vz);
    std::cout<<"Longueur totale du segment au sein du detecteur : "<<Ltot<<" mm"<<std::endl;

    double_t tMin=1.0, tMax=0.0; // Paramètres
    for (double_t t=0.0; t<=1.0; t+=0.001) { 
        double_t x=p1.x+t*vx;
        double_t y=p1.y+t*vy;
        double_t z=p1.z+t*vz;
        
        if (InHole(x, y, z, holeRadius, holeZMin, holeZMax)) {
            tMin = std::min(tMin, t); // tMin est la valeur pour laquelle le segment entre dans le trou
            tMax = std::max(tMax, t); // tMax est la valeur pour laquelle le segment quitte le trou
        }
    }
    if (tMax>tMin) { // Si tMax>tMin, le segment passe dans le trou et on peut calculer la distance traversant le trou
        double_t lenght = std::sqrt((tMax-tMin)*(tMax-tMin)*(vx*vx+vy*vy+vz*vz)); // tMax-tMin est la fraction du segment dans le trou
        double_t Lger=Ltot-lenght;
        std::cout<<"Longueur du segment traversant le germanium : "<<Lger<<" mm"<<std::endl;
        return lenght;
    } else {
        return 0.0; // Si le segment ne traverse pas le trou, la longueur est donc de 0mm
    }
}

void drawLine3D(Point p1, Point p2) {
    const int32_t nPoints = 100;
    double_t x[nPoints], y[nPoints], z[nPoints];
    for (int32_t i=0; i<nPoints; i++) {
        double_t t = (double_t)i/(nPoints-1);
        x[i]=p1.x+t*(p2.x-p1.x);
        y[i]=p1.y+t*(p2.y-p1.y);
        z[i]=p1.z+t*(p2.z-p1.z);
    }
    TPolyLine3D* line = new TPolyLine3D(nPoints, x, y, z);
    line->SetLineColor(kRed);
    line->SetLineWidth(2);
    line->Draw("same");
}

void drawCylinder(double_t radius, double_t zMin, double_t zMax, int32_t nSlices=100, int32_t color = kBlue) {
    const int32_t nPoints = nSlices+1;
    double_t x[nPoints], y[nPoints], z[nPoints];

    for (int32_t i=0; i<nPoints; i++) {
        double_t theta = 2*M_PI*i/(nPoints-1);
        x[i] = radius*cos(theta);
        y[i] = radius*sin(theta);
    }

    // Bas du cylindre
    for (int32_t i=0; i<nPoints; i++) z[i] = zMin;
    TPolyLine3D* bottomCircle = new TPolyLine3D(nPoints, x, y, z);
    bottomCircle->SetLineColor(color);
    bottomCircle->Draw("same");

    // Haut du cylindre
    for (int32_t i=0; i<nPoints; i++) z[i] = zMax;
    TPolyLine3D* topCircle = new TPolyLine3D(nPoints, x, y, z);
    topCircle->SetLineColor(color);
    topCircle->Draw("same");

    // Lignes verticales
    for (int32_t i=0; i<nPoints; i+=nPoints/15) {
        double_t vx[] = {x[i], x[i]};
        double_t vy[] = {y[i], y[i]};
        double_t vz[] = {zMin, zMax};
        TPolyLine3D* edge = new TPolyLine3D(2, vx, vy, vz);
        edge->SetLineColor(color);
        edge->Draw("same");
    }
}

void cylindre() {
    TH3F* his = new TH3F("his", "Segment au sein du detecteur", 
                           82, -40, 40,  // X
                           82, -40, 40,  // Y
                           82, -45, 45);   // Z
    his->GetXaxis()->SetTitle("X (mm)");
    his->GetYaxis()->SetTitle("Y (mm)");
    his->GetZaxis()->SetTitle("Z (mm)");

    // Points de départ et d'arrivée
    Point p1 = {-30.0, 0.0, 20.0};
    Point p2 = {30.0, 0.0, 20.0};
   
    // Tracer le grand cylindre (rayon 40mm, hauteur 90mm) et le petit à l'intérieur (rayon 15mm, hauteur 90mm)
    TCanvas* can = new TCanvas("can", "Segment et cylindre", 800, 800);
    his->Draw();
    drawCylinder(40.0, -45.0, 45.0, 100, kGreen);
    drawCylinder(15.0, -45.0, 32.0, 100, kBlue);
    drawLine3D(p1, p2); // Segment

    // Calcul de la longueur du segment traversant le trou
    double_t holeRadius = 15.0, holeZMin = -45.0, holeZMax = 32.0, cylinderRadius = 40.0, cylinderHeight = 45.0;
    double_t Lhole = HolePath(p1, p2, holeRadius, holeZMin, holeZMax, cylinderRadius, cylinderHeight);
    std::cout<<"Longueur du segment traversant le trou : "<<Lhole<<" mm"<<std::endl;
	
    can->SaveAs("segment_cylindre_3D.root");
}
