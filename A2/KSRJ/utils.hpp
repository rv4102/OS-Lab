#ifndef __UTILITY_HPP
#define __UTILITY_HPP

#include <string>
#include <vector>

using namespace std;

string trim(string s);
vector<string> split(string str, char delim);
vector<char*> cstrArray(vector<string>& args);

#endif