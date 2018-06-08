#ifndef GRAPH_PLOTTER_HPP
#define GRAPH_PLOTTER_HPP

#include <vector>
#include <string>

class TCanvas;
class TGraph;
class TGraphErrors;
class TFile;

class TObject;

class graphPlotter {
  public:
	graphPlotter();
	
	~graphPlotter();

	bool toggleDebug(){ return (debug = !debug); }

	void reset();

	void draw(const bool &prev_=false);

	void drawData(const std::string &opt="PSAME");
	
	void idleTask();

	bool runFresco(const std::string &inputFilename);

	bool runFrescout(const std::string &searchString, double &integral);

	bool runReadGrace(const std::string &filename, double &integral);

	int write(const std::string &filename, const int &format, const std::string &name="graph", const bool &overwrite=true);

	bool loadExternalDataFile(const std::string &filename);

	bool readExternalDataFile(std::vector<std::string> &objNames);

	bool setExternalDataGraph(const std::string &name);

	bool setExternalDataGraph(TGraphErrors *ptr=NULL){ return ((dataGraph=ptr) != NULL); }

  private:
	TCanvas *can;
	
	TGraph *currGraph;
	TGraph *prevGraph;
	TGraphErrors *dataGraph;

	TFile *externalFile;

	bool debug;
	
	int appendObjectToFile(TFile *f, TObject *obj, const std::string &name);
};

#endif
