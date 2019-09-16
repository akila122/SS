//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    ObjectFile.h                            ****
//****             Info:    ObjectFile class Header                 ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#ifndef OBJECTFILE_H
#define OBJECTFILE_H

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
#include "Symbol.h"
#include "Instruction.h"
#include "Directive.h"
#include "Expression.h"
#include "Section.h"
#include "ExprRelocation.h"

using namespace std;

class ObjectFile {
public:
    ObjectFile(string inputFile);
    virtual ~ObjectFile();
private:
    
    void writeText();
    
    string inputFile;
    map<string,Symbol> symTable;
    map<string,Section*> sections;
    map<string,Expression> TNS;
    vector<ExprRelocation> exprRelocations;
    friend class Linker;
    

};

#endif

