#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <termios.h>
#include <glob.h>
#include <signal.h>

using namespace std;

int main(){
    cout << "Current : " << getpid() << endl;
    int x, y;
    cin >> x >> y;
    cout << x << " " << y << endl;
    // int t=0;
    // while(1){
    //     cout << t << endl;
    //     t++;
    // }
    // pid_t p = fork();
    // if(p == 0){
    //     cout << "Child : " << getpid() << endl;
    // }
    // else{
    //     cout << "Parent : " << getpid() << endl;
    // }
    // cout << "Hi " << getpid() << endl;
}