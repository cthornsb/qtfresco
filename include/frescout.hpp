#ifndef FRESCOUT_HPP
#define FRESCOUT_HPP

#include <string>

class TGraph;

bool frescout(const std::string &filename, TGraph *graph, double &integral, const std::string &searchString2, const bool &debug=false);

#endif // FRESCOUT_HPP

