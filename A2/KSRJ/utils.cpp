#include "utils.h"

#include <cstring>
#include <sstream>

using namespace std;

// Removes whitespaces from beginning and end of a string
string trim(string s){
    int i = 0;
    while(s[i] == ' '){
        i++;
    }
    if(i == s.length()){
        return "";
    }
    s = s.substr(i);
    while(s.back() == ' '){
        s.pop_back();
    }
    return s;
}

vector<string> split(string s, char delim){
    vector<string> v;
    int last = 0;
    for(int i=0; i<s.length(); i++){
        if(s[i] == delim){
            string temp = trim(s.substr(last, i-last));
            if(!temp.empty()){
                v.push_back(temp);
            }
            last = i+1;
        }
    }
    string temp = trim(s.substr(last));
    if(!temp.empty()){
        v.push_back(temp);
    }
    return v;
}

// Converts a vector of strings to a vector of char*
vector<char*> cstrArray(vector<string>& args) {
    vector<char*> args_(args.size() + 1);
    for (int i = 0; i < (int)args.size(); i++) {
        args_[i] = (char*)malloc((args[i].length() + 1) * sizeof(char));
        strcpy(args_[i], args[i].c_str());
    }
    args_[args.size()] = (char*)malloc(sizeof(char));
    args_[args.size()] = nullptr;
    return args_;
}
