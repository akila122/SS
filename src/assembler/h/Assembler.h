//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Assembler.h                             ****
//****             Info:    Assembler class Header                  ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================


#ifndef ASSEMBLER_H
#define ASSEMBLER_H

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
#include <iomanip>
#include "Symbol.h"
#include "Instruction.h"
#include "Directive.h"
#include "Expression.h"
#include "Section.h"
#include "ExprRelocation.h"


using namespace std;

class Assembler {
    
public:
    
    Assembler(string,string);
    virtual ~Assembler();
    
private:
    
    string inputFile;
    string outputFile;
    
    vector<string> tokens;
    
    map<string,Symbol> symTable;
    map<string,Section*> sections;
    map<string,Expression> TNS;
    vector<ExprRelocation> exprRelocations;
    
    static Section* currSection;
    
    void firstRun();
    void checkSyms();
    void secondRun();
    void writeText();
    
    void tokenize();

    void newSymbol(string,bool);
    void newInstruction(string,bool);
    
    friend int main(int,char**);
    
    void _global(Directive&,bool);
    void _extern(Directive&,bool);
    void _equ(Directive&,bool);
    void _custom(Directive&, bool);
    void _byte(Directive&, bool);
    void _word(Directive&, bool);
    void _align(Directive&, bool);
    void _skip(Directive&, bool);
    
};

#endif

