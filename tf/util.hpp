#ifndef utilH
#define utilH
#include <string>
#include <sstream>
#include <vector>

const int SIZE_LIMIT = 2000;
const char msg_delim = '|';

template<typename Out> void split(const std::string &s, char delim, Out result);

std::vector<std::string> split(const std::string &s, char delim);
#endif
