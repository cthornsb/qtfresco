#ifndef GRAPH_PLOTTER_HPP
#define GRAPH_PLOTTER_HPP

#include <string>

class TCanvas;
class TGraph;
class TFile;

class TObject;

class graphPlotter {
  public:
	graphPlotter();
	
	~graphPlotter();

	bool toggleDebug(){ return (debug = !debug); }

	void reset();

	void redraw(const bool &prev_=false);
	
	void idleTask();

	bool runFresco(const std::string &inputFilename);

	bool runFrescout(const std::string &searchString, double &integral);

	bool runReadGrace(const std::string &filename, double &integral);

	int write(const std::string &filename, const int &format, const std::string &name="graph", const bool &overwrite=true);

  private:
	TCanvas *can;
	
	TGraph *currGraph;
	TGraph *prevGraph;

	bool debug;
	
	int appendObjectToFile(TFile *f, TObject *obj, const std::string &name);
};

#endif
