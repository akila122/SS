//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    MemAllocator.h                          ****
//****             Info:    MemAllocator class Header               ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#ifndef MEMALLOCATOR_H
#define MEMALLOCATOR_H

#include <stdint.h>
#include <regex>
#include <string>
#include <map>
#include <iostream>
#include <exception>
#include <climits>
#include <list>

using namespace std;

class MemAllocator {

public:


    MemAllocator(uint16_t startAdr, size_t size) {
        mem.push_back(Node(startAdr, size));
        maxAdr = startAdr + size;
    };
    virtual ~MemAllocator();

    struct Node {
        uint16_t startAdr;
        size_t size;

        Node(uint16_t _startAdr, size_t _size) : startAdr(_startAdr), size(_size) {
        }
    };
private:

    friend int main(int, char**);
    
    void scan();
    list<Node> mem;

    uint16_t maxAdr;
    uint16_t allocFree(size_t sz, uint8_t align);
    uint16_t allocAt(uint16_t adr, size_t sz, uint8_t align);
    static size_t alignAt(uint16_t adr, uint8_t align);

       
    
    friend class Linker;
    

};

#endif /* MEMALLOCATOR_H */

