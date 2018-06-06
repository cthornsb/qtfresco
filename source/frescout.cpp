#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "frescout.hpp"

#include "TFile.h"
#include "TGraph.h"

const double pi = 3.1415926536;

bool frescout(const std::string &filename, TGraph *graph, double &integral, const bool &debug/*=false*/){
	const std::string searchString1 = "CROSS SECTIONS FOR OUTGOING";
	const std::string searchString2 = "NEUTRON  &";

	std::ifstream file(filename.c_str());
	if(!file.good()) return false;

	std::vector<double> thetaCM;
	std::vector<double> xsection;

	bool scanning = false;

	// Scan the fresco output file and look for cross-section data.
	std::string line;
	while(true){
		std::getline(file, line);
		if(file.eof()) break;

		if(!scanning){
			if(line.find(searchString1) != std::string::npos && line.find(searchString2) != std::string::npos){
				if(debug){
					std::cout << " debug: \"" << line << "\"" << std::endl;
					std::cout << " debug: Cross-section data follows...\n";
					std::cout << "thetaCM\txsection\n";
				}
				scanning = true;
				continue;
			}
		}
		else{
			if(line.find("Integrated") != std::string::npos){
				scanning = false;
				break;
			}
			// Extract the x-section.
			size_t index1 = line.find("X-S =");
			if(index1 == std::string::npos) continue;
			index1 = line.find_first_not_of(' ', index1+6);
			size_t index2 = line.find(" mb/sr");
			std::string substring2 = line.substr(index1, index2-index1);

			// Extract the angle.
			index1 = line.find_first_not_of(' ', 0);
			index2 = line.find(" deg.:");
			std::string substring1 = line.substr(index1, index2-index1);

			thetaCM.push_back(strtod(substring1.c_str(), NULL));
			xsection.push_back(strtod(substring2.c_str(), NULL));

			// Output the info.
			if(debug) std::cout << substring1 << "\t" << substring2 << std::endl;
		}
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
