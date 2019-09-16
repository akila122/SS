
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    ExprRelocation.cpp                      ****
//****             Info:    ExprRelocation class Source             ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#include "ExprRelocation.h"

ExprRelocation::~ExprRelocation() {
    
}
ExprRelocation::ExprRelocation(string inputLine){
    
 
    regex split("^(.*)\\t\\t\\t(.*)\\t\\t\\t(.*)\\t\\t\\t(.*)\\t\\t\\t(.*)\\t\\t\\t(.*)$");
    smatch m;
    regex_search(inputLine,m,split);
    section = m[1];
    offset = stoi(m[2]);
    sz = m[4] == "WORD";
    relType = m[5]=="REL_ABS" ? REL_ABS : REL_PC;
    size_t pos = 0;
    string s = m[6];
    string token;
    string delimiter = ",";
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        exprs.push_back(Expression(token));
        s.erase(0, pos + delimiter.length());
    }
    exprs.push_back(Expression(s));
}

