#include <iostream>
#include <sstream>
#include <fstream>

#include "simpleChisquare.hpp"

#include "graphPlotter.hpp"
#include "frescout.hpp"
#include "readGrace.hpp"

#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TKey.h"

#include "TSystem.h"
#include "TApplication.h"

#define FRESCO_SYSTEM_CALL "fresco"

void setGraphStyle(TGraph *g, const Color_t &color){
	g->SetLineWidth(2);
	g->SetLineColor(color);

	g->SetTitle("");
	g->GetXaxis()->SetTitle("#theta_{CM} (deg)");
	g->GetYaxis()->SetTitle("d#sigma/d#Omega (mb/sr)");
}

graphPlotter::graphPlotter() : TApplication("rootapp", 0, 0) { // Enable root graphics.
	can = new TCanvas("can", "qtfresco");
	can->cd()->SetFrameLineColor(kWhite);

	currGraph = new TGraph(181);
	prevGraph = new TGraph(181);

	setGraphStyle(currGraph, kBlue+2);
	setGraphStyle(prevGraph, kRed);
	
	dataGraph = NULL;
	externalFile = NULL;

	debug = false;

	draw();
}

graphPlotter::~graphPlotter(){ 
	can->Close();
	delete can;

	delete currGraph;
	delete prevGraph;

	if(externalFile){
		externalFile->Close();
		delete externalFile;
	}

	// Call this last because it will terminate the program.
	this->Terminate();
}

void graphPlotter::reset(){
	for(int i = 0; i < 181; i++){
		currGraph->SetPoint(i, 0, 0);
		prevGraph->SetPoint(i, 0, 0);
	}

	can->Clear();
	can->Update();
}

void graphPlotter::draw(const bool &prev_/*=false*/){
	can->Clear();

	currGraph->Draw("AL");
	if(prev_) 
		prevGraph->Draw("LSAME");
	
	can->Update();
}

void graphPlotter::drawData(const std::string &opt/*="PSAME"*/){
	if(dataGraph){
		dataGraph->Draw(opt.c_str());
		can->Update();
	}
}

void graphPlotter::idleTask(){
	gSystem->ProcessEvents();
}

bool graphPlotter::runFresco(const std::string &inputFilename){
	const std::string frescoSystemCall(FRESCO_SYSTEM_CALL);

	// fresco < ${INPUT_FILENAME} > fresco.out
	std::string call = frescoSystemCall + " < " + inputFilename + " > qtfresco.tmp.out";

	if(debug) std::cout << " debug: `" << call << "`\n";

	// Do the system call for fresco.
	return (system(call.c_str()) == 0);
}

bool graphPlotter::runFrescout(const std::string &searchString, double &integral){
	// Copy the current graph to the previous one.
	double x, y;
	for(int i = 0; i < 181; i++){
		currGraph->GetPoint(i, x, y);
		prevGraph->SetPoint(i, x, y);
	}

	if(debug) std::cout << " debug: Reading from fresco stdout file \"qtfresco.tmp.out\".\n";

	// Call frescout to parse the output from fresco.
	bool retval = frescout("qtfresco.tmp.out", currGraph, integral, searchString, debug);

	return retval;
}

bool graphPlotter::runReadGrace(const std::string &filename, double &integral){
	// Copy the current graph to the previous one.
	double x, y;
	for(int i = 0; i < 181; i++){
		currGraph->GetPoint(i, x, y);
		prevGraph->SetPoint(i, x, y);
	}

	if(debug) std::cout << " debug: Reading from fresco output file \"" << filename << "\".\n";

	// Call readGrace to parse the output file from fresco.
	bool retval = readGrace(filename, currGraph, integral, debug);

	return retval;
}

bool graphPlotter::runChisquare(double &A, double &chi2){
	chisquare chi;
	
	// Set user options for chi-square minimization.
	//chi.setFitRange(0, 180);
	chi.setParLimits(0, 1);
	//chi.setAddConstTerm(true);

	// Set the graphs.
	chi.setTheoreticalGraph((TGraphErrors*)currGraph);
	chi.setExperimentalGraph(dataGraph);

	// Run the minimization.
	if(chi.compute()){
		A = chi.getA();
		chi2 = chi.getChi2();

		// Copy the current graph to the previous one and scale the current one.
		double x, y;
		for(int i = 0; i < 181; i++){
			currGraph->GetPoint(i, x, y);
			prevGraph->SetPoint(i, x, y);
			currGraph->SetPoint(i, x, y*A);
		}

		return true;
	}

	return false;
}

int graphPlotter::write(const std::string &filename, const int &format, const std::string &name/*="graph"*/, const bool &overwrite/*=true*/){
	int retval = 0;
	if(format == 0){ // Write TGraph directly to file.
		TFile *fout = new TFile(filename.c_str(), (overwrite ? "RECREATE" : "UPDATE"));
		if(!fout || !fout->IsOpen()) return -9999;
		if(overwrite){ // Overwrite the file.
			fout->cd();
			currGraph->Write(name.c_str());
		}
		else{ // Append to the file.
			retval = appendObjectToFile(fout, currGraph, name);
		}
		fout->Close();
		delete fout;
	}
	else if(format == 1){ // Write entire TCanvas to file.
		TFile *fout = new TFile(filename.c_str(), (overwrite ? "RECREATE" : "UPDATE"));
		if(!fout || !fout->IsOpen()) return -9999;
		if(overwrite){ // Overwrite the file.
			fout->cd();
			can->Write(name.c_str());
		}
		else{ // Append to the file.
			retval = appendObjectToFile(fout, can, name);
		}
		fout->Close();
		delete fout;
	}
	else if(format == 2){ // Write the graph entries to an ascii file.
		std::ofstream fout(filename.c_str());
		if(!fout.good()) return -9999;
		fout << "angle\txsection\n";
		double x, y;
		for(int i = 0; i < currGraph->GetN(); i++){
			currGraph->GetPoint(i, x, y);
			fout << x << "\t" << y << std::endl;
		}
		fout.close();
	}
	else if(format == 3){ // Print the TCanvas to a file.
		can->Print(filename.c_str());
	}
	else return -9999;
	
	return retval;
}

bool graphPlotter::loadExternalDataFile(const std::string &filename){
	if(externalFile){ // Load the external data file if it is open.
		externalFile->Close();
		delete externalFile;
	}
	externalFile = new TFile(filename.c_str());
	if(!externalFile->IsOpen()) return false;
	return true;
}

bool graphPlotter::readExternalDataFile(std::vector<std::string> &objNames){
	if(!externalFile || !externalFile->IsOpen()) return false;

        TIter nextKey(externalFile->GetListOfKeys());
        TKey *key;
        while((key=(TKey*)nextKey())){
                std::string name = key->GetName();
                std::string className = key->GetClassName();
                if(debug) std::cout << " debug: name=\"" << name << "\", class=\"" << className << "\"\n";
                if(className == "TGraph" || className == "TGraphErrors")
                        objNames.push_back(name);
        }

	return true;
}

bool graphPlotter::setExternalDataGraph(const std::string &name){
	if(!externalFile || !externalFile->IsOpen()) return false;
	dataGraph = (TGraphErrors*)externalFile->Get(name.c_str());
	return (dataGraph != NULL);
}

int graphPlotter::appendObjectToFile(TFile *f, TObject *obj, const std::string &name){
	std::string actualname = name;
	int graphCounter = 0;
	while(true){
		TObject *check = f->Get(actualname.c_str());
		if(check == 0x0) break;
		std::stringstream stream;
		stream << ++graphCounter;
		actualname = name;
		if(graphCounter < 10)
			actualname += "00";
		else if(graphCounter < 100)
			actualname += "0";
		actualname += stream.str();
	}
	f->cd();
	obj->Write(actualname.c_str());
	return graphCounter;
}
