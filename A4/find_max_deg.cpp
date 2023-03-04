#include <bits/stdc++.h>
using namespace std;
int indeg[40000];
int main(){
    string path = "musae_git_edges.csv";
    ifstream infile(path);
    if(!infile.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }
    int x = 0, y = 0;
    char dummy[10], c; // c consumes the comma
    infile >> dummy;
    while(infile >> x >> c >> y) {
        indeg[x]++;
        indeg[y]++;
    }
    infile.close();
    int max_deg = 0;
    for(int i=0; i<40000; i++){
        max_deg = max(max_deg, indeg[i]);
    }
    cout<<max_deg<<endl;
}