
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Emulator.h                              ****
//****             Info:    Emulator class Header                   ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#ifndef EMULATOR_H
#define EMULATOR_H

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
#include <thread>
#include "Linker.h"
#include "Utils.h"

class Emulator {

public:
    Emulator(vector<string>, map<string, uint16_t>);
    virtual ~Emulator();

private:
    uint8_t* mem,*flags;
    uint16_t regs[16];
    int64_t clksNow = 0;
    
    const uint64_t CPU_SPEED = 1000000*200;
    const int16_t REG_ACCESS = -1;
    const int16_t MEM_ACCESS = -5;
    
    bool terminalIntr = false;
    
    void keyboardThread();
    
    thread t;
    
    //FOR PSW
    
    const int sp = 6;
    const int pc = 7;
    const int psw = 15;
    const uint16_t data_out = 0xFF00;
    const uint16_t data_in = 0xFF02;
    const uint16_t timer_cfg = 0xFF10;
    bool running = true;
    
    uint8_t keyboardReg = 0;
    bool keyboardI = false;
    
    map<uint8_t,int64_t> timerMap = {
        {0,CPU_SPEED*0.5},{1,CPU_SPEED},{2,CPU_SPEED*1.5},{3,CPU_SPEED*2},
        {4,CPU_SPEED*5},{5,CPU_SPEED*10},{6,CPU_SPEED*30},{7,CPU_SPEED*60}
    };
    
    static int16_t signExtend(int16_t); 
    static int16_t pcHelper(uint8_t,bool);
    void checkInts();
    void push(uint16_t,bool);
    uint8_t popByte();
    uint16_t popWord();
    uint8_t nextByte();
    uint8_t getByteAt(uint16_t adr);
    uint16_t getWordAt(uint16_t adr);
    void setAt(uint16_t,uint16_t,bool);
    
    void setI(bool);
    void setTl(bool);
    void setTr(bool);
    void setN(bool);
    void setC(bool);
    void setO(bool);
    void setZ(bool);
    bool getI();
    bool getTl();
    bool getTr();
    bool getN();
    bool getC();
    bool getO();
    bool getZ();
    


    void run();

    void _halt(bool);

    void _xchg(bool);

    void _int(bool);

    void _mov(bool);
    
    void _add(bool);

    void _sub(bool);

    void _mul(bool);
    
    void _div(bool);

    void _cmp(bool);
    
    void _not(bool);

    void _and(bool);
    
    void _or(bool);

    void _xor(bool);

    void _test(bool);

    void _shl(bool);

    void _shr(bool);

    void _push(bool);

    void _pop(bool);

    void _jmp(bool);

    void _jeq(bool);

    void _jne(bool);

    void _jgt(bool);

    void _call(bool);

    void _ret(bool);

    void _iret(bool);



};

#endif

