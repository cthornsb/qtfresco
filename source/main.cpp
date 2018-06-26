#include <iostream>
#include <vector>

#include "qtfresco.hpp"
#include <QApplication>

#include "graphPlotter.hpp"

#include "optionHandler.hpp"

std::vector<optionExt> baseOpts; /// Base level command line options for the scan.

void help(char *name_){
	std::cout << " usage: " << name_ << " [options]\n";
	std::cout << "  Available options:\n";
	for(std::vector<optionExt>::iterator iter = baseOpts.begin(); iter != baseOpts.end(); iter++){
		if(!iter->name) continue;
		iter->print(40, "   ");
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow win;

	// Handle option input.
	std::vector<option> longOpts; /// Vector of all command line options.
	std::string optstr;
	
	baseOpts.push_back(optionExt("help", no_argument, NULL, 'h', "", "Display this dialogue."));
	baseOpts.push_back(optionExt("input", required_argument, NULL, 'i', "<filename>", "Specifies a fresco input file to process."));
	baseOpts.push_back(optionExt("output", required_argument, NULL, 'o', "<filename>", "Specifies the name of the output file."));
	baseOpts.push_back(optionExt("name", required_argument, NULL, 'n', "<name>", "Specify the output prefix for the TGraph or TCanvas."));
	baseOpts.push_back(optionExt("data", required_argument, NULL, 'd', "<filename>", "Specify the root file containing data to display."));

	optstr = "hi:o:n:d:";

	// Build the vector of all command line options.
	for(std::vector<optionExt>::iterator iter = baseOpts.begin(); iter != baseOpts.end(); iter++){
		longOpts.push_back(iter->getOption()); 
	}
	
	// Append all zeros onto the option list. Required for getopt_long.
	struct option zero_opt { 0, 0, 0, 0 };
	longOpts.push_back(zero_opt);
	
	int idx = 0;
	int retval = 0;
	
	std::string dataFilename;
	while ( (retval = getopt_long(argc, argv, optstr.c_str(), longOpts.data(), &idx)) != -1) {
		if(retval == 0x0 || retval == 0x3F){ // Unknown option, '?'
			return 1;
		}
		else{ // Single character options.
			switch(retval) {
				case 'h' :
					help(argv[0]);
					return 0;
				case 'i' :
					win.setInputFilename(optarg);
					break;
				case 'o' :
					win.setOutputFilename(optarg);
					break;
				case 'n' :
					win.setOutputPrefix(optarg);
					break;
				case 'd' : 
					dataFilename = optarg;
					break;
				default:
					break;
			}
		}
	}//while

	// Finalize option setting.
	win.finalize();

	// Show the GUI window.
	win.show();

	// MainWindow will delete the instance of graphPlotter.
	win.setPointer(new graphPlotter());

	if(!dataFilename.empty()) // The program will crash if we do this before setting the graphPlotter pointer.
		win.setDataFilename(dataFilename);

	return app.exec();
}
