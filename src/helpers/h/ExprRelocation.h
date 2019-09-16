
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    ExprRelocation.h                        ****
//****             Info:    ExprRelocation class Header             ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#ifndef EXPRRELOCATION_H
#define EXPRRELOCATION_H

#include <string>
#include <regex>
#include <stack>
#include <vector>
#include <map>
#include <stdint.h>
#include <iostream>
#include "Expression.h"
#include "Utils.h"

class ExprRelocation {
    
public:
    
    ExprRelocation(string _section,uint16_t _offset,vector<Expression> _exprs,bool _sz,RelType _relType) :
   section(_section),offset(_offset),exprs(_exprs),sz(_sz),relType(_relType){};
    
    ExprRelocation(string _section,uint16_t _offset,Expression e,bool _sz,RelType _relType,string sy = "") :
    section(_section),offset(_offset),sz(_sz),relType(_relType),symbol(sy){ exprs.push_back(e);}
    
    
    ExprRelocation(string);
    
    virtual ~ExprRelocation();
    
private:

    string section;
    string symbol;
    uint16_t offset;
    vector<Expression> exprs;
    bool sz;
    RelType relType;
    
    friend class Assembler;
    friend class ObjectFile;
    friend class Linker;
    
};

#endif 

