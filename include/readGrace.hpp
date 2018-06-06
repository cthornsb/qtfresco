#ifndef READGRACE_HPP
#define READGRACE_HPP

#include <string>

class TGraph;

bool readGrace(const std::string &filename, TGraph *graph, double &integral, const bool &debug=false);

#endif // READGRACE_HPP

