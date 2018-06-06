#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "TGraph.h"

#include "CTerminal.h" // split_str

// Typical fresco grace output header.
/*# 181 angles,  1 tensor ranks for 0=projectile
@subtitle   "p+Ni78 Coulomb and Nuclear;"												   
@legend ON
@legend x1 0.2
@legend y1 0.8
@g0 type LOGY
@yaxis ticklabel format power
@xaxis label "Scattering angle (degrees)"
@yaxis label "Cross section (mb/sr)"
@legend string   0 "Partition=  1 Excit=  1 near/far= 1"
#legend string   0 "Lab energy =	6.9000"
@s0 linestyle   1
#  Theta	   sigma	   iT11		T20		 T21		 T22		 Kyy for projectile*/

const double pi = 3.1415926536;

bool readGrace(const std::string &filename, TGraph *graph, double &integral, const bool &debug=false){
	std::ifstream file(filename.c_str());
	if(!file.good()) return false;

	std::vector<double> thetaCM;
	std::vector<double> xsection;

	if(debug){
		std::cout << " debug: Cross-section data follows...\n";
		std::cout << "thetaCM\txsection\n";
	    }

	bool isdone = false;
	std::string line;
	while(!isdone){
		std::getline(file, line);
		if(file.eof()) break;

		// Check for blank line.
		if(line.empty()) continue;

		// Check for comments and grace commands.	
		if(line.at(0) == '@' || line.at(0) == '#') continue;

		// Split string into columns.
		std::vector<std::string> cols;
		if(split_str(line, cols, ' ') == 0) continue;

		int Ncol = 0;
		for(std::vector<std::string>::iterator iter = cols.begin(); iter != cols.end(); iter++){
			if(iter->empty()) continue;
			if(*iter == "END"){
				isdone = true;
				break;
			}

			if(Ncol == 0){
				thetaCM.push_back(strtod(iter->c_str(), NULL));
				Ncol++;
			}
			else if(Ncol == 1){
				xsection.push_back(strtod(iter->c_str(), NULL));
				break;
			}
		}

		if(debug) std::cout << thetaCM.back() << "\t" << xsection.back() << std::endl;
	}

	file.close();

	// Failed to find any cross-section data.
	if(thetaCM.empty()) return false;

	// Check the length of the graph against the number of points read from the file.
	if(thetaCM.size() != (unsigned int)graph->GetN())
		graph->Set(thetaCM.size());

	// Set the first point of the distribution.
	graph->SetPoint(0, thetaCM.at(0), xsection.at(0));

	// Set the remaining points and calculate the integral (mb).
	integral = 0;
	double solidAngle;
	for(size_t i = 1; i < thetaCM.size(); i++){
	    solidAngle = 2*pi*(std::cos(thetaCM.at(i-1)*pi/180)-std::cos(thetaCM.at(i)*pi/180));
	    integral += 0.5*solidAngle*(xsection.at(i)+xsection.at(i-1));
	    graph->SetPoint(i, thetaCM.at(i), xsection.at(i));
	}

	return true;
}
