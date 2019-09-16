
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Symbol.h                                ****
//****             Info:    Symbol class Header                     ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#ifndef SYMBOL_H
#define SYMBOL_H

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

class Symbol {
public:

    Symbol(string _name, string _section, bool _defined, bool _scope, uint16_t _offset, ssize_t _size, bool _isEqu = false) :
    name(_name), section(_section), defined(_defined), scope(_scope), val(_offset), size(_size), isEqu(_isEqu), align(0) {
    };
    virtual ~Symbol();

    Symbol(string input);

private:

    string name;
    string section;
    bool defined;
    uint16_t val;
    bool scope;
    size_t size;
    bool isEqu;
    uint8_t align;

    friend class Assembler;
    friend class ObjectFile;
    friend class Linker;


};

#endif

