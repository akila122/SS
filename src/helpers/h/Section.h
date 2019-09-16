
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Section.h                               ****
//****             Info:    Section class Header                    ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#ifndef SECTION_H
#define SECTION_H

#include <stdint.h>
#include <regex>
#include <string>
#include <map>
#include <iostream>
#include <exception>
#include <climits>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

class Section {
    
public:

    Section(string _name,uint8_t _flags) : name(_name),flags(_flags),lc(0),maxAlign(0){};
    virtual ~Section();

private:
    
    string name;
    uint16_t lc;
    uint8_t flags;
    vector<uint8_t> data;
    int8_t maxAlign;
    
    friend class Assembler;
    friend class Directive;
    friend class ObjectFile;
    friend class Linker;
    
    

};

#endif /* SECTION_H */

