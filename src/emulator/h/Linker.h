//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Linker.h                                ****
//****             Info:    Linker class Header                     ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#ifndef LINKER_H
#define LINKER_H

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
#include "ObjectFile.h"
#include "MemAllocator.h"

using namespace std;

class Linker {
    
public:
    
    Linker(vector<string>,map<string,uint16_t>);
    virtual ~Linker();
    
private:
    
    void resolveGlobals();
    void resolveSections();
    void resolveSymbols();
    void mapGlobals();
    void resolveTNS();
    void resolveRelocs();
    void generateMem();
    void resolveIVT();
    void writeText();
    
    struct SectionInfo {
        
        string name;
        size_t totalSz;
        uint16_t nextAdr;
        bool preff;
        
        SectionInfo(string nm,size_t s,uint16_t n,bool p) : name(nm),totalSz(s),nextAdr(n),preff(p){};
        
        
    };
    
    vector<ObjectFile> objFiles;
    uint8_t* mem,*flags;
    
    uint16_t _start;
    uint16_t IVT[8];
    
    map<string,SectionInfo> sections;
    map<string,uint16_t> globals;    
    
    friend class Emulator;
};

#endif /* LINKER_H */

