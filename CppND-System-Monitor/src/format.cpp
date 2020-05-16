#include <string>
#include <stdio.h>
#include <iostream>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) { 
    string hh = std::to_string(seconds/3600);
    int ss = seconds%3600;
    string mm = std::to_string(ss/60);
    ss = ss%60;
    string time = hh+":"+mm+":"+ std::to_string(ss);
    return time; 
    }