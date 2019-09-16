
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Symbol.cpp                              ****
//****             Info:    Symbol class Source                     ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#include "Symbol.h"


Symbol::~Symbol() {
}

Symbol::Symbol(string input){
    
    regex split("^(.*)\\t\\t\\t(.*)\\t\\t\\t(.*)\\t\\t\\t(.*)\\t\\t\\t(.*)\\t\\t\\t(.*)\\t\\t\\t(.*)$");
    smatch m;
    regex_match(input,m,split);
    section = m[1];
    name = m[2];
    defined = m[3] == "1";
    scope = m[4] == "GLOBAL";
    val = stoi(m[5]);
    size = stoi(m[6]);
    align = stoi(m[7]);
    isEqu = section == "ABS";
    
    
    
}
