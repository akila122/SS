
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Emulator.cpp                            ****
//****             Info:    Emulator class Source                   ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#include "Emulator.h"


Emulator::Emulator(vector<string> files,map<string,uint16_t> preff) {
    
    Linker l(files,preff);
    
    mem = l.mem;
    flags = l.flags;
    
    for(int i=0; i<8; i++){
        mem[2*i] = l.IVT[i];
        mem[2*i+1] = l.IVT[i]>>8; 
    }
    
    regs[pc] = mem[0];
    regs[pc] |= mem[1]<<8;
    regs[sp] = STACK_START_ADR;
  
    for(int i = 0; i<8; i++){
        uint16_t adr = mem[2*i];
        adr |= mem[2*i+1]<<8;
      
    }


    
    clksNow = timerMap.at(0);

    t = thread([=]{keyboardThread();});

    t.detach();   
    
    run();
    
    
}

void Emulator::keyboardThread(){
    
    while(true){
        char tmp;
        cin >> noskipws >> tmp;
        keyboardReg = tmp;
        terminalIntr = true;
        while(terminalIntr);
    }
    
}

Emulator::~Emulator() {
}


void Emulator::run(){
    
    
    while(running){
        
        uint8_t b0 = nextByte();
        
        bool opSz = (b0>>2) & 0x1;
        InstrDescrType instrType = static_cast<InstrDescrType>(b0>>3);
        
        if(instrType > IRET)
            throw "Emulator Error: Unknown opcode encountered -> "+to_string((int)instrType);
        
        try{
            switch(instrType){
            case NOP:
                break;
            case HALT:
                _halt(opSz);
                break;
            case XCHG:
                _xchg(opSz);
                break;
            case INT:
                _int(opSz);
                break;
            case MOV:
                _mov(opSz);
                break;
            case ADD:
                _add(opSz);
                break;
            case SUB:
                _sub(opSz);
                break;
            case MUL:
                _mul(opSz);
                break;
            case DIV:
                _div(opSz);
                break;
            case CMP:
                _cmp(opSz);
                break;
            case NOT:
                _not(opSz);
                break;
            case AND:
                _and(opSz);
                break;
            case OR:
                _or(opSz);
                break;
            case XOR:
                _xor(opSz);
                break;
            case TEST:
                _test(opSz);
                break;
            case SHL:
                _shl(opSz);
                break;
            case SHR:
                _shr(opSz);
                break;
            case PUSH:
                _push(opSz);
                break;
            case POP:
                _pop(opSz);
                break;
            case JMP:
                _jmp(opSz);
                break;
            case JEQ:
                _jeq(opSz);
                break;
            case JNE:
                _jne(opSz);
                break;
            case JGT:
                _jgt(opSz);
                break;
            case CALL:
                _call(opSz);
                break;
            case RET:
                _ret(opSz);
                break;
            case IRET:
                _iret(opSz);
                break;
                
        }
            if(running && !getI()) checkInts();
        }
        catch(string s){
            cout<<s<<endl;
            push(regs[pc],true);
            push(regs[psw],true);
            regs[pc] = getWordAt(2*1);
        }
        catch(char const* s){
            cout<<s<<endl;
            push(regs[pc],true);
            push(regs[psw],true);
            regs[pc] = getWordAt(2*1);
        }
        
        
    }
    cout<<endl;
    
}

void Emulator::checkInts() {
    
    
    if (!getTr()){
        if(clksNow <= 0){
            push(regs[pc],true);
            push(regs[psw],true);
            setTr(true);
            clksNow += timerMap.at(mem[timer_cfg]%8); 
            regs[pc] = getWordAt(2*2);
        }
    }
    if(!getTl() && terminalIntr){
        
        mem[data_in] = keyboardReg;
        terminalIntr =false;
        push(regs[pc],true);
        push(regs[psw],true);
        regs[pc] = getWordAt(2*3);
        
    }
    
}

void Emulator::_halt(bool opSz) {
    
    cout<<endl<<"HALT instruction encountered. The program will finish";
    running = false;
}
void Emulator::_xchg(bool opSz) {
    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in XCHG";
            break;
            
        }
        case REGDIR:{
          
           
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
	    adr1 += signExtend(off1);
	    
            A = opSz ? getWordAt(adr1) : getByteAt(adr1);
            break;
        }
            
        case REGIND_OFF16:{
            
            
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);	
            adr1 += off1;
       
	    A = opSz ? getWordAt(adr1) : getByteAt(adr1);

	   
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : getByteAt(adr1);
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
            
            
            throw "Emulator Error: Imm with dst in XCHG";
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);


            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }
    
    R = A;
    A = B;
    B = R;
    
    
    switch(type1){
        
        case REGDIR:{
          
            clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= A<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= A & 0xFF;
                }
            }
            else regs[reg1] = A;
            break;
        }
        case REGIND_OFF8:{
            setAt(A,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(A,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(A,adr1,opSz);
            
            break;
        
        }
            
    }
    
    switch(type2){
        
        case REGDIR:{
          
            clksNow+=REG_ACCESS;
            if(!opSz){
                if(b2 & 1){
                    regs[reg2] &= 0xFF;
                    regs[reg2] |= R<<8;
                }
                else{
                 regs[reg2] &= 0xFF00;
                 regs[reg2] |= R & 0xFF;
                }
            }
            else regs[reg2] = R;
            break;
        }
        case REGIND_OFF8:{
            
            setAt(B,adr2,opSz);
            break;
        }
            
        case REGIND_OFF16:{
         
            setAt(B,adr2,opSz);
            
            break;
        }
        case MEM:{
          
            setAt(B,adr2,opSz);
            break;
        
        }
            
    }
    
    
}
void Emulator::_int(bool opSz)  {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            A = nextByte();
            if(opSz) A |= nextByte() << 8;
	    else A = signExtend(A);
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    A%=8;
    push(regs[pc],true);
    push(regs[psw],true);
    
    clksNow+=REG_ACCESS;
    regs[pc] = getWordAt(A*2);
    
    
    
    
}
void Emulator::_mov(bool opSz)  {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in MOV";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
	    adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = (b2>>1) & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
	 	
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }
	
    
    R = B;
    
    switch(type1){
        
        case REGDIR:{
          clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        }    
    }   
}
void Emulator::_add(bool opSz)  {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in ADD";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
		
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
	    adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
	    adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }

    if(!opSz) B =signExtend(B);  
    
    R = A + B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        setO((R>0 && A<0 && B<0) || (R<0 && A>0 && B>0));
        setC(getO());
        //All signed so its the same Error??? (Micko)
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        setO((Rb>0 && Ab<0 && Bb<0) || (Rb<0 && Ab>0 && Bb>0));
        setC(getO());
    }    
    
    
    switch(type1){
        
        case REGDIR:{
          clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        
        }
            
    }
    
}
void Emulator::_sub(bool opSz)  {

    
    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in SUB";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
	    adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }

    if(!opSz) B =signExtend(B); 
    
    R = A - B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        setO((R>0 && A<0 && B>0) || (R<0 && A>0 && B<0));
        setC(getO());
        //All signed so its the same Error??? (Micko)
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        setO((Rb>0 && Ab<0 && Bb>0) || (Rb<0 && Ab>0 && Bb<0));
        setC(getO());
    }   
    
    
    switch(type1){
        
        case REGDIR:{
          clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            break;
        }       
    }
}
void Emulator::_mul(bool opSz)  {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in MUL";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
	    adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }

    if(!opSz) B =signExtend(B); 
    
    R = A * B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
       
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        
    }  
    
    switch(type1){
        
        case REGDIR:{
          clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        
        }
            
    }
    
}
void Emulator::_div(bool opSz)  {

    
    
    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in DIV";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }

    
    if(B==0) throw "Emulator Error: Division by zero encountered";

    if(!opSz) B =signExtend(B);    

    R = A / B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        
        
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        
    }
    
    switch(type1){
        
        case REGDIR:{
          
            clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        }    
    }   
}
void Emulator::_cmp(bool opSz)  {
    
    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            A = nextByte();
            if(opSz) A |= nextByte()<<8;
	    else A = signExtend(A);
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }

    if(!opSz) B =signExtend(B); 
    
    R = A - B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        setO((R>0 && A<0 && B>0) || (R<0 && A>0 && B<0));
        setC(getO());
        //All signed so its the same Error??? (Micko)
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        setO((Rb>0 && Ab<0 && Bb>0) || (Rb<0 && Ab>0 && Bb<0));
        setC(getO());
    }   
}
void Emulator::_not(bool opSz)  {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in NOT";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
	    adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    R = ~A;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        
        
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        
    }
    
    switch(type1){
        
        case REGDIR:{
        clksNow+=REG_ACCESS;  
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        }    
    }   

}
void Emulator::_and(bool opSz)  {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in AND";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }

    if(!opSz) B =signExtend(B); 
    
    R = A & B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        
        
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        
    }
    
    switch(type1){
        
        case REGDIR:{
        
            clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        }    
    }   
     
}
void Emulator::_or(bool opSz)   {
    
    
    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in OR";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }

    if(!opSz) B =signExtend(B); 
    
    R = A | B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        
        
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        
    }
    
    switch(type1){
        
        case REGDIR:{
          clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        }    
    }   
    
    
}
void Emulator::_xor(bool opSz)  {

    
    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in XOR";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
	    adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }

    if(!opSz) B =signExtend(B); 
    
    R = A ^ B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        
        
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        
    }
    
    switch(type1){
        clksNow+=REG_ACCESS;
        case REGDIR:{
          
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        }    
    }  
    
}
void Emulator::_test(bool opSz) {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            A = nextByte();
            if(opSz) A |= nextByte()<<8;
	    else A = signExtend(A);
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
	    adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }
    
    if(!opSz) B =signExtend(B); 
    
    R = A & B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);

        //All signed so its the same Error??? (Micko)
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
    }   
    
}
void Emulator::_shl(bool opSz)  {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in SHL";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) :signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : signExtend(getByteAt(adr1));
            break;
        
        }
    }

    
     R = A << B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        setC(B!=0 ? A<<(B-1)&1 : 0);
        //All signed so its the same Error??? (Micko)
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        setC(Bb!=0 ? Ab<<(Bb-1)&1 : 0);
    }
    
    
    switch(type1){
        
        case REGDIR:{
          clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        }    
    }   
    
    
    
}
void Emulator::_shr(bool opSz)  {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            throw "Emulator Error: Imm with dst in SHR";
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
		A = signExtend(A);
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
	    if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            if(reg1==pc) adr1 += pcHelper(mem[regs[pc]],opSz);
            adr1 += off1;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : signExtend(getByteAt(adr1));
            break;
        
        }
            
    }
    
    b2 = nextByte();
    OpDescrType type2 = static_cast<OpDescrType>(b2>>5);
    
    switch(type2){
        
        case IMM: {
       
            B = nextByte();
            if(opSz) B |= nextByte()<<8;
            break;

            
        }
        case REGDIR:{
          
            reg2 = b2>>1 & 0xF;
            B = regs[reg2];
            if(!opSz){
                if(b2 & 1) B >>= 8;
                else B &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off2 = nextByte();
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += signExtend(off2);
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        }
            
        case REGIND_OFF16:{
         
            off2 = nextByte();
            off2 |= nextByte() << 8;
            reg2 = b2>>1 & 0xF;
            adr2 = regs[reg2];
            adr2 += off2;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            
            break;
        }
        case MEM:{
            
            adr2 = nextByte();
            adr2 |= nextByte() << 8;
            B = opSz ? getWordAt(adr2) : getByteAt(adr2);
            break;
        
        }
    }

    
     R = A >> B;
    
    if(opSz){
        
        setN(R<0);
        setZ(R==0);
        setC(B!=0 ? A>>(B-1)&1 : 0);
        //All signed so its the same Error??? (Micko)
    }else{
        int8_t Ab = A;
        int8_t Bb = B;
        int8_t Rb = Ab + Bb;
        setN(Rb<0);
        setZ(Rb==0);
        setC(Bb!=0 ? Ab>>(Bb-1)&1 : 0);
    }
    
    
    switch(type1){
        
        case REGDIR:{
          clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
            setAt(R,adr1,opSz);
            
            break;
        }    
    }   
    
    

}
void Emulator::_push(bool opSz) {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    
    switch(type1){
        
        case IMM: {
            
            A = nextByte();
            if(opSz) A |= nextByte()<<8;
            break;
            
        }
        case REGDIR:{
          
            reg1 = b1>>1 & 0xF;
            A = regs[reg1];
            if(!opSz){
                if(b1 & 1) A >>= 8;
                else A &=  0xFF;
            }
            break;
        }
        case REGIND_OFF8:{
            
            off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            adr1 += signExtend(off1);
            A = opSz ? getWordAt(adr1) : getByteAt(adr1);
            break;
        }
            
        case REGIND_OFF16:{
         
            off1 = nextByte();
            off1 |= nextByte() << 8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            adr1 += off1;
            A = opSz ? getWordAt(adr1) : getByteAt(adr1);
            
            break;
        }
        case MEM:{
            
            adr1 = nextByte();
            adr1 |= nextByte() << 8;
            A = opSz ? getWordAt(adr1) : getByteAt(adr1);
            break;
        
        }
            
    }
    
    push(A,opSz);

}
void Emulator::_pop(bool opSz)  {

    int16_t A;
    int16_t B;
    int16_t R;
    
    uint8_t reg1;
    uint16_t adr1;
    uint8_t reg2;
    uint16_t adr2;
    int16_t off1;
    int16_t off2;
    
    uint8_t b1;
    uint8_t b2;
    
    b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
   
    R = opSz ? popWord() : popByte();
    
   switch(type1){
       
       case IMM:{
           throw "Emulator Error: Imm with dst in POP";
           break;
       }
       
        case REGDIR:{
          
	    reg1 = b1>>1 & 0xF;
	    clksNow+=REG_ACCESS;
            if(!opSz){
                if(b1 & 1){
                    regs[reg1] &= 0xFF;
                    regs[reg1] |= R<<8;
                }
                else{
                 regs[reg1] &= 0xFF00;
                 regs[reg1] |= R & 0xFF;
                }
            }
            else regs[reg1] = R;
            break;
        }
        case REGIND_OFF8:{
  
	    off1 = nextByte();
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            adr1 += signExtend(off1);

            setAt(R,adr1,opSz);
            break;
        }
            
        case REGIND_OFF16:{
      
	    off1 = nextByte();
	    off1 |= nextByte()<<8;
            reg1 = b1>>1 & 0xF;
            adr1 = regs[reg1];
            adr1 += off1;
            setAt(R,adr1,opSz);
            
            break;
        }
        case MEM:{
           
	    adr1 = nextByte();
   	    adr1 |= nextByte()<<8;
            setAt(R,adr1,opSz);
            
            break;
        }    
    }   
    
    
}
void Emulator::_jmp(bool opSz)  {

    uint8_t b1 = nextByte();
    clksNow+=REG_ACCESS;
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    switch(type1){
   
        case MEM:{
            
            uint16_t adr = nextByte();
            adr |= nextByte() << 8;
            regs[pc] = adr;
            break;
        }
        case REGIND_OFF16:{
         
            int16_t off = nextByte();
            off |= nextByte() << 8;
            uint8_t reg = b1>>1 & 0xF;
            uint16_t adr = regs[reg];
            adr += off;
            regs[pc] = adr;
            
            break;
        }
        default:
            throw "Emulator Error: Invalid JMP instruction";
            break;       
    }
    
}
void Emulator::_jeq(bool opSz)  {

    uint8_t b1 = nextByte();
    clksNow+=REG_ACCESS;
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    switch(type1){
   
        case MEM:{
            
            uint16_t adr = nextByte();
            adr |= nextByte() << 8;
            if (getZ()) regs[pc] = adr;
            break;
        }
        case REGIND_OFF16:{
         
            int16_t off = nextByte();
            off |= nextByte() << 8;
            uint8_t reg = b1>>1 & 0xF;
            uint16_t adr = regs[reg];
            adr += off;
            if (getZ()) regs[pc] = adr;
            
            break;
        }
        default:
            throw "Emulator Error: Invalid JMP instruction";
            break;       
    }
    
}
void Emulator::_jne(bool opSz)  {

     uint8_t b1 = nextByte();
     clksNow+=REG_ACCESS;
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    switch(type1){
   
        case MEM:{
            
            uint16_t adr = nextByte();
            adr |= nextByte() << 8;
            if (!getZ()) regs[pc] = adr;
            break;
        }
        case REGIND_OFF16:{
         
            int16_t off = nextByte();
            off |= nextByte() << 8;
            uint8_t reg = b1>>1 & 0xF;
            uint16_t adr = regs[reg];
            adr += off;
            if (!getZ()) regs[pc] = adr;
            
            break;
        }
        default:
            throw "Emulator Error: Invalid JMP instruction";
            break;       
    }
}
void Emulator::_jgt(bool opSz)  {

    clksNow+=REG_ACCESS;
     uint8_t b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    switch(type1){
   
        case MEM:{
            
            uint16_t adr = nextByte();
            adr |= nextByte() << 8;
            if ((getN() ^ getO()) | getZ()) regs[pc] = adr;
            break;
        }
        case REGIND_OFF16:{
         
            int16_t off = nextByte();
            off |= nextByte() << 8;
            uint8_t reg = b1>>1 & 0xF;
            uint16_t adr = regs[reg];
            adr += off;
            if ((getN() ^ getO()) | getZ()) regs[pc] = adr;
            
            break;
        }
        default:
            throw "Emulator Error: Invalid JMP instruction";
            break;       
    }
    
}
void Emulator::_call(bool opSz) {

    clksNow+=REG_ACCESS;
    uint8_t b1 = nextByte();
    OpDescrType type1 = static_cast<OpDescrType>(b1>>5);
    switch(type1){
   
        case MEM:{
            
            uint16_t adr = nextByte();
            adr |= nextByte() << 8;
            push(regs[pc],true);
            regs[pc] = adr;
            break;
        }
        case REGIND_OFF16:{

         

            int16_t off = nextByte();
            off |= nextByte() << 8;
            uint8_t reg = b1>>1 & 0xF;
            uint16_t adr = regs[reg];
            adr += off;
	    push(regs[pc],true);
            regs[pc] = adr;

	    
            
            break;
        }
        default:
            throw "Emulator Error: Invalid CALL instruction";
            break;       
    }
    
}
void Emulator::_ret(bool opSz)  {

    clksNow+=REG_ACCESS;
    regs[pc] = popWord();
    
}
void Emulator::_iret(bool opSz) {
    
    clksNow+=2*REG_ACCESS;
    regs[psw] = popWord();
    regs[pc] = popWord();

    
}

void Emulator::push(uint16_t val,bool sz){
    
    clksNow += sz ? 2*MEM_ACCESS : MEM_ACCESS;
    mem[regs[sp]++] = val;
    if(regs[sp] >= STACK_START_ADR+STACK_SZ)
        throw "Emulator Error: Stack Overflow";
    if(sz) mem[regs[sp]++] = val >> 8;
    if(regs[sp] >= STACK_START_ADR+STACK_SZ)
        throw "Emulator Error: Stack Overflow";

    
    
}

uint8_t Emulator::popByte(){
    
    clksNow += MEM_ACCESS;
    
    uint8_t ret = mem[--regs[sp]];
    if(regs[sp] < STACK_START_ADR)
        throw "Emulator Error: Stack Underflow";
    
    
    return ret;
    
}

uint16_t Emulator::popWord(){
    
    clksNow += 2*MEM_ACCESS;
    
    uint16_t ret = popByte();
    ret<<=8;
    ret |= popByte();
    
    return ret;
    
    
}

uint8_t Emulator::nextByte(){
 
    if(!(flags[regs[pc]] & 1))
	throw "Emulator Error: Segmentation fault X.";
    clksNow += MEM_ACCESS;
    return mem[regs[pc]++];
}

uint8_t Emulator::getByteAt(uint16_t adr){
    
    if(!(flags[adr] & 4))
	throw "Emulator Error: Segmentation fault R.";
    clksNow += MEM_ACCESS;
    return mem[adr];
    
}

uint16_t Emulator::getWordAt(uint16_t adr){
    
    if(!(flags[adr] & 4) || !(flags[adr+1] & 4) )
	throw "Emulator Error: Segmentation fault R.";
    clksNow += 2*MEM_ACCESS;
    uint16_t ret = mem[adr];
    ret |= mem[adr+1] << 8;
    return ret;
    
}

void Emulator::setAt(uint16_t val,uint16_t adr,bool sz){
    
    clksNow+= sz ? 2*MEM_ACCESS : MEM_ACCESS;
    mem[adr] = val;
    if(!(flags[adr] & 2))
	throw "Emulator Error: Segmentation fault W.";
    if(adr == data_out) cout<<(char)mem[adr];
    if (adr == timer_cfg) clksNow = timerMap.at(val%8);
    if(sz){
        mem[adr+1] = val>>8;
	if(!(flags[adr+1] & 2))
		throw "Emulator Error: Segmentation fault W.";
        if(adr + 1 == data_out) cout<<(char)mem[adr+1];
        if(adr + 1 == timer_cfg) clksNow = timerMap.at((val>>8)%8);
    }
};

bool Emulator::getI(){
    return regs[psw]>>15;
}
bool Emulator::getTl(){
    return regs[psw]>>14 & 1;
}
bool Emulator::getTr(){
    return regs[psw]>>13 & 1;
}

bool Emulator::getN(){
    return regs[psw]>>3 & 1;
}
bool Emulator::getC(){
    return regs[psw]>>2 & 1;
}
bool Emulator::getO(){
    return regs[psw]>>1 & 1;
}
bool Emulator::getZ(){
    return regs[psw] & 1;
}

void Emulator::setI(bool b){
    regs[psw] ^= (-(unsigned int)b^regs[psw]) & (1UL<< 15); 
}

void Emulator::setTl(bool b){
    regs[psw] ^= (-(unsigned int)b^regs[psw]) & (1UL<< 14);
}

void Emulator::setTr(bool b){
    regs[psw] ^= (-(unsigned int)b^regs[psw]) & (1UL<< 13);
}
void Emulator::setN(bool b){
    regs[psw] ^= (-(unsigned int)b^regs[psw]) & (1UL<< 3);
}
void Emulator::setC(bool b){
    regs[psw] ^= (-(unsigned int)b^regs[psw]) & (1UL<< 2);
}
void Emulator::setO(bool b){
    regs[psw] ^= (-(unsigned int)b^regs[psw]) & (1UL<< 1);
}
void Emulator::setZ(bool b){
    regs[psw] ^= (-(unsigned int)b^regs[psw]) & (1UL<< 0);
}

int16_t Emulator::signExtend(int16_t b){

	if(b>>7) return b|0xFF00;
	else return b;

}

int16_t Emulator::pcHelper(uint8_t b,bool opSz){


	OpDescrType type = static_cast<OpDescrType>(b>>5);

	switch (type) {
        case IMM:
            return opSz ? 3 : 2;
        case REGDIR:
            return 2;
        case REGIND_OFF8:
            return 2;
        case REGIND_OFF16:
            return 3;
        case MEM:
            return 3;
        default :
            throw "FATAL ERROR";
    }

}

