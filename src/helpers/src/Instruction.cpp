
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Instruction.cpp                         ****
//****             Info:    Instruction class Source                ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#include "Instruction.h"

Instruction::Instruction(string toParse) : instrSz(0),instrDescr(0),op1Descr(0),op1B1(0),
                                           op1B2(0),op2Descr(0),op2B1(0),op2B2(0),
                                           instrType(HALT),op1Type(REGDIR),op2Type(REGDIR),inputLine(toParse),
                                           op1Val(""),op2Val(""),
                                           op1Sym(""),op2Sym(""),opSz(true),
                                           op1(""),op2(""),
                                           op1SynType(IMM_VAL),op2SynType(IMM_VAL),
                                           op1Num(""),op2Num(""),
                                           op1HL(""),op2HL(""),
                                           op1IsImmHex(false),op2IsImmHex(false),
                                           op1RelType(NO_REL),op2RelType(NO_REL)
{  
    
    
    if(IDTLambdaMap.empty()) initMaps();
    regex e("^(halt|xchg|int|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|push|pop|jmp|jeq|jne|jgt|call|ret|iret)(b|w)? *.*$");
    
    smatch m;
    regex_search(inputLine,m,e);
    instrSz = 0x1;
    instrType = StrIDTMap.at(m[1]);
    
    if (instrType == NOP){
        byteVector.push_back(0);
        return;
    }
    IDTLambdaMap.at(instrType)(this);
    
    test();
    encode();
    
    
}

Instruction::~Instruction() {
}


void Instruction::resolveTwoOperands(){
    
    smatch m;
    if(!regex_search(this->inputLine,m,splitTwoOpr)) throw "Assembler Error: "+this->inputLine+" is not valid.";
    this->op1 = m[3];
    this->op2 = m[4];
    if (!m[2].compare("")) opSz = true;
    else opSz = m[2].compare("b") == 0 ? false : true;
        
    
    
    
}
void Instruction::resolveOneOperand(){
    
    
    smatch m;
    if(!regex_search(this->inputLine,m,splitOneOpr)) throw "Assembler Error: "+this->inputLine+" is not valid.";
    this->op1 = m[3];
    if (!m[2].compare("")) opSz = true;
    else opSz = m[2].compare("b") == 0 ? false : true;
      
    
}

void Instruction::parseOp1(){
    
    smatch m;
    
    if(regex_match(op1,isImm)){
    
        regex_search(op1,m,isImm);
        op1Val = m[1];
        op1SynType = IMM_VAL;
        op1Type = IMM;
    
    }
    else if(regex_match(op1,isImmSym)){
        
        regex_search(op1,m,isImmSym);
        op1Sym = m[1];
        op1SynType = IMM_SYM;
        op1Type = IMM;
        
    }
    else if(regex_match(op1,isRegDirNum)){
        regex_search(op1,m,isRegDirNum);
        op1Num = m[1];
        op1HL = m[2];
        if (!op1HL.compare("")) op1HL = opSz ? "w" : "l";
        op1SynType = REG_DIR;
        op1Type = REGDIR;
    }
    else if(regex_match(op1,isRegDirAlph)){
        regex_search(op1,m,isRegDirAlph);
        if(!m[1].compare("sp")) op1Num = "6";
        else if (!m[1].compare("pc")) op1Num = "7";
        else if (!m[1].compare("psw")) op1Num = "15";
        op1HL = m[2];
        if (!op1HL.compare("")) op1HL = opSz ? "w" : "l";
        op1SynType = REG_DIR;
        op1Type = REGDIR;
    }
    else if(regex_match(op1,isRegNumOff)){
        regex_search(op1,m,isRegNumOff);
        op1Num = m[1];
        op1HL = m[2];
        op1Val = m[3];
        if (!op1HL.compare("")) op1HL = opSz ? "w" : "l";
 
        op1SynType = REG_VAL;
        op1Type = (isOverflow(_parseVal(op1Val))) ? REGIND_OFF16 : REGIND_OFF8;
    }
    else if(regex_match(op1,isRegAlphOff)){
        
        regex_search(op1,m,isRegAlphOff);
        if(!m[1].compare("sp")) op1Num = "6";
        else if (!m[1].compare("pc")) op1Num = "7";
        else if (!m[1].compare("psw")) op1Num = "15";
        op1HL = m[2];
        op1Val = m[3];
        if (!op1HL.compare("")) op1HL = opSz ? "w" : "l";
        op1SynType = REG_VAL;
        op1Type = (isOverflow(_parseVal(op1Val))) ? REGIND_OFF16 : REGIND_OFF8;
    }
    else if(regex_match(op1,isRegNumSym)){
        
        regex_search(op1,m,isRegNumSym);
        op1Num = m[1];
        op1HL = m[2];
        op1Sym = m[3];
        if (!op1HL.compare("")) op1HL = opSz ? "w" : "l";
        op1SynType = REG_SYM;
        op1Type = REGIND_OFF16;
    }
    else if(regex_match(op1,isRegAlphSym)){
        
        regex_search(op1,m,isRegAlphSym);
        if(!m[1].compare("sp")) op1Num = "6";
        else if (!m[1].compare("pc")) op1Num = "7";
        else if (!m[1].compare("psw")) op1Num = "15";
        op1HL = m[2];
        op1Sym = m[3];
        if (!op1HL.compare("")) op1HL = opSz ? "w" : "l";
        op1SynType = REG_SYM;
        op1Type = REGIND_OFF16;
        
    }
    else if (regex_match(op1,isPcRelSym)){
        regex_search(op1,m,isPcRelSym);
        op1Sym = m[1];
        op1Num = "7";
        op1SynType = SYM_PC;
        op1Type = REGIND_OFF16;
    }
    else if (regex_match(op1,isAbsData)){
        regex_search(op1,m,isAbsData);
        op1Val = m[1];
        op1SynType = ABS_DATA;
        op1Type = MEM;
    }
    else if(regex_match(op1,isAbsSym)){
        regex_search(op1,m,isAbsSym);
        op1Sym = m[1];
        op1SynType = SYM_ABS;
        op1Type = MEM;
    }
    else throw "Assembler Error: Invalid addresing mode "+inputLine+" -> "+op1; 
    
}
void Instruction::parseOp2(){
 
    
    smatch m;
    
    if(regex_match(op2,isImm)){
    
        
        
        regex_search(op2,m,isImm);
        op2Val = m[1];
        op2SynType = IMM_VAL;
        op2Type = IMM;
    
    }
    else if(regex_match(op2,isImmSym)){
        
  
        
        regex_search(op2,m,isImmSym);
        op2Sym = m[1];
        op2SynType = IMM_SYM;
        op2Type = IMM;
        
    }
    else if(regex_match(op2,isRegDirNum)){
        
        
        
        regex_search(op2,m,isRegDirNum);
        op2Num = m[1];
        op2HL = m[2];
        if (!op2HL.compare("")) op2HL = opSz ? "w" : "l";
        op2SynType = REG_DIR;
        op2Type = REGDIR;
    }
    else if(regex_match(op2,isRegDirAlph)){
        regex_search(op2,m,isRegDirAlph);
        if(!m[1].compare("sp")) op2Num = "6";
        else if (!m[1].compare("pc")) op2Num = "7";
        else if (!m[1].compare("psw")) op2Num = "15";
        op2HL = m[2];
        if (!op2HL.compare("")) op2HL = opSz ? "w" : "l";
        op2SynType = REG_DIR;
        op2Type = REGDIR;
    }
    else if(regex_match(op2,isRegNumOff)){
        regex_search(op2,m,isRegNumOff);
        op2Num = m[1];
        op2HL = m[2];
        op2Val = m[3];
        if (!op2HL.compare("")) op2HL = opSz ? "w" : "l";
 
        op2SynType = REG_VAL;
        op2Type = (isOverflow(_parseVal(op2Val))) ? REGIND_OFF16 : REGIND_OFF8;
    }
    else if(regex_match(op2,isRegAlphOff)){
        
        
        regex_search(op2,m,isRegAlphOff);
        if(!m[1].compare("sp")) op2Num = "6";
        else if (!m[1].compare("pc")) op2Num = "7";
        else if (!m[1].compare("psw")) op2Num = "15";
        op2HL = m[2];
        op2Val = m[3];
        if (!op2HL.compare("")) op2HL = opSz ? "w" : "l";
        op2SynType = REG_VAL;
        op2Type = (isOverflow(_parseVal(op2Val))) ? REGIND_OFF16 : REGIND_OFF8;
    }
    else if(regex_match(op2,isRegNumSym)){
 
        
        
        regex_search(op2,m,isRegNumSym);
        
        op2Num = m[1];
       
        op2HL = m[2];
        op2Sym = m[3];
        if (!op2HL.compare("")) op2HL = opSz ? "w" : "l";
        op2SynType = REG_SYM;
        op2Type = REGIND_OFF16;
    }
    else if(regex_match(op2,isRegAlphSym)){
 
        
        
        regex_search(op2,m,isRegAlphSym);
        if(!m[1].compare("sp")) op2Num = "6";
        else if (!m[1].compare("pc")) op2Num = "7";
        else if (!m[1].compare("psw")) op2Num = "15";
        op2HL = m[2];
        op2Sym = m[3];
        if (!op2HL.compare("")) op2HL = opSz ? "w" : "l";
        op2SynType = REG_SYM;
        op2Type = REGIND_OFF16;
        
    }
    else if (regex_match(op2,isPcRelSym)){
        
        regex_search(op2,m,isPcRelSym);
        op2Num = "7";
        op2Sym = m[1];
        op2SynType = SYM_PC;
        op2Type = REGIND_OFF16;
    }
    else if (regex_match(op2,isAbsData)){
        regex_search(op2,m,isAbsData);
        op2Val = m[1];
        op2SynType = ABS_DATA;
        op2Type = MEM;
    }
    else if(regex_match(op2,isAbsSym)){
        regex_search(op2,m,isAbsSym);
        op2Sym = m[1];
        op2SynType = SYM_ABS;
        op2Type = MEM;
    }
    else throw "Assembler Error: Invalid addressing mode "+inputLine+" -> "+op2; 
    
};
void Instruction::encode(){
    
    //InstrDescr
    instrDescr = instrType;
    instrDescr <<= 1;
    instrDescr |= opSz;
    instrDescr <<= 2;
    
    byteVector.push_back(instrDescr);
    
    if (instrType == INT || instrType == NOT || (instrType >= PUSH && instrType <= CALL ) ){
    
        encodeOp1();
        
    }
    else if (instrType < RET && instrType > HALT){
        
        encodeOp1();
        encodeOp2();
        
    }
    
    
}

void Instruction::encodeOp1(){
    
    op1Descr = op1Type;
    
        if(op1Type >= REGDIR && op1Type <= REGIND_OFF16){
            op1Descr <<= 4;
            uint8_t regVal = stoi(op1Num);
            op1Descr |= regVal;
            op1Descr <<= 1;
            if(!opSz) op1Descr |= (bool)(!op1HL.compare("h"));    
        }
        else op1Descr <<= 5;
  
    
    
        byteVector.push_back(op1Descr);
    
        instrSz+=1;    
    
        uint64_t val;
        
        switch (op1SynType){
            
        
            case IMM_VAL:
                
                val = _parseVal(op1Val);
                op1B1 = val;
                op1B2 = val>>8;
                instrSz += (opSz) ? 2 : 1;
                byteVector.push_back(op1B1);
                if(opSz) byteVector.push_back(op1B2);
                break;
                
            case IMM_SYM:
      
                op1RelType = ALERT_AS;
                instrSz += (opSz) ? 2 : 1;
                byteVector.push_back(op1B1);
                if(opSz) byteVector.push_back(op1B2);
                
                break;
            
            case REG_DIR:    
                break;
                
            case REG_VAL:
                
                val = _parseVal(op1Val);
                op1B1 = val;
                op1B2 = val >> 8;
                instrSz += (op1Type == REGIND_OFF16) ? 2 : 1;
                byteVector.push_back(op1B1);
                if(op1Type == REGIND_OFF16) byteVector.push_back(op1B2);
                break;
                
            case REG_SYM:
                
                op1RelType = ALERT_AS;
                 byteVector.push_back(op1B1);
                 byteVector.push_back(op1B2);
                instrSz += 2;
                
                break;
                
            case SYM_PC:
              
                
                op1RelType = ALERT_AS;
                instrSz += 2;
                byteVector.push_back(op1B1);
                byteVector.push_back(op1B2);
                break;
                
            case SYM_ABS:
                
                op1RelType = ALERT_AS;
                 byteVector.push_back(op1B1);
                 byteVector.push_back(op1B2);
                instrSz += 2;
                break;
                
            case ABS_DATA:
                
                val = _parseVal(op1Val);
                op1B1 = val;
                op1B2 = val >> 8;
                 byteVector.push_back(op1B1);
                 byteVector.push_back(op1B2);
                instrSz += 2;
                break;
        }
       
    
}
void Instruction::encodeOp2(){
    
    
 
    
    op2Descr = op2Type;
        if(op2Type >= REGDIR && op2Type <= REGIND_OFF16){
            op2Descr <<= 4;
            
            uint8_t regVal = stoi(op2Num);
            
            op2Descr |= regVal;
            op2Descr <<= 1;
            if(!opSz){
                op2Descr |= (bool)(!op2HL.compare("h"));
            }
        }
        else op2Descr <<= 5;
    
    byteVector.push_back(op2Descr);
    
        instrSz+=1;    
    
        uint64_t val;
     
        
        switch (op2SynType){
            
            case IMM_VAL:
                
                val = _parseVal(op2Val);
                op2B1 = val;
                op2B2 = val>>8;
                instrSz += (opSz) ? 2 : 1;
                byteVector.push_back(op2B1);
                if(opSz) byteVector.push_back(op2B2);
                break;
                
            case IMM_SYM:
      
                op2RelType = ALERT_AS;
                instrSz += (opSz) ? 2 : 1;
                byteVector.push_back(op2B1);
                if(opSz) byteVector.push_back(op2B2);
                break;
            case REG_DIR:
                
                break;
            case REG_VAL:
                
                val = _parseVal(op2Val);
                op2B1 = val;
                op2B2 = val >> 8;
                instrSz += (op2Type == REGIND_OFF16) ? 2 : 1;
                byteVector.push_back(op2B1);
                if(op2Type == REGIND_OFF16) byteVector.push_back(op2B2);
                break;
            case REG_SYM:
                
                op2RelType = ALERT_AS;
                byteVector.push_back(op2B1);
                byteVector.push_back(op2B2);
                instrSz +=  2;
                break;
            case SYM_PC:
              
                
                op2RelType = ALERT_AS;
                byteVector.push_back(op2B1);
                byteVector.push_back(op2B2);
                instrSz += 2;
                break;
            case SYM_ABS:
                op2RelType = ALERT_AS;
                byteVector.push_back(op2B1);
                byteVector.push_back(op2B2);
                instrSz += 2;
                break;
            case ABS_DATA:
                val = _parseVal(op2Val);
                op2B1 = val;
                op2B2 = val >> 8;
                byteVector.push_back(op2B1);
                byteVector.push_back(op2B2);
                instrSz += 2;
                break;
        }
    
        
}

void Instruction::test(){
    
    if (!opSz && !op1HL.compare("w")) throw "Assembler Error: Ambiguous operand size "+inputLine+" -> "+op1;
    if (!opSz && !op2HL.compare("w")) throw "Assembler Error: Ambiguous operand size "+inputLine+" -> "+op2;
    
    if(op1Type == IMM && instrType != CMP && instrType != TEST && instrType != PUSH && instrType != HALT && instrType != RET && instrType != IRET && instrType != INT) throw "Assembler Error: Immediate addressing mode not allowed: "+inputLine+" -> "+op1;
    if(op2Type == IMM && instrType == XCHG) throw "Assembler Error: Immediate addressing mode not allowed with xchg: "+inputLine+" -> "+op2;    
    
    if(instrType >= JMP && instrType <=IRET && !opSz) throw "Assembler Error: Instruction cannot be combined with 'b' extension: "+inputLine;
    
    if(instrType >= JMP && instrType <=CALL && op1SynType != SYM_PC &&op1SynType != SYM_ABS)
        throw "Assembler Error: Invalid jump instruction -> "+inputLine;
}

map<string,InstrDescrType> Instruction::StrIDTMap = {};
map<InstrDescrType,void (*)(Instruction*)> Instruction::IDTLambdaMap = {};

void Instruction::_halt(Instruction* _this){
    
};
void Instruction::_xchg(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
    

    
};
void Instruction::_int(Instruction* _this){

    _this->resolveOneOperand();
    _this->parseOp1();
    
};
void Instruction::_mov(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
    
};
void Instruction::_add(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
};
void Instruction::_sub(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
};
void Instruction::_mul(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
};
void Instruction::_div(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
   
};
void Instruction::_cmp(Instruction* _this){

   
    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
   
};
void Instruction::_not(Instruction* _this){


    _this->resolveOneOperand();
    _this->parseOp1();
    
};

void Instruction::_and(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
    
};
void Instruction::_or(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
   
};
void Instruction::_xor(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
   
};
void Instruction::_test(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
   
};
void Instruction::_shl(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
    
};
void Instruction::_shr(Instruction* _this){

    _this->resolveTwoOperands();
    _this->parseOp1();
    _this->parseOp2();
    
};
void Instruction::_push(Instruction* _this){


    _this->resolveOneOperand();
    _this->parseOp1();
    
};
void Instruction::_pop(Instruction* _this){

    _this->resolveOneOperand();
    _this->parseOp1();

};
void Instruction::_jmp(Instruction* _this){

    _this->resolveOneOperand();
    _this->parseOp1();
    
};
void Instruction::_jeq(Instruction* _this){

    _this->resolveOneOperand();
    _this->parseOp1();
    
};
void Instruction::_jne(Instruction* _this){

    _this->resolveOneOperand();
    _this->parseOp1();

};
void Instruction::_jgt(Instruction* _this){

   _this->resolveOneOperand();
    _this->parseOp1();
    
};
void Instruction::_call(Instruction* _this){

    _this->resolveOneOperand();
    _this->parseOp1();
    
};
void Instruction::_ret(Instruction* _this){

    
};
void Instruction::_iret(Instruction* _this){

    
};

int Instruction::numOfOprs(){
    
    if(instrType == HALT || instrType == RET || instrType == IRET ) return 0;
    if(instrType >= PUSH && instrType <= CALL || instrType == INT || instrType == NOT) return 1;
    return 2;
    
}

int Instruction::getOffOpr2(){
    
    switch (op1Type) {
        case IMM:
            return opSz ? 5 : 4;
        case REGDIR:
            return 3;
        case REGIND_OFF8:
            return 4;
        case REGIND_OFF16:
            return 5;
        case MEM:
            return 5;
        default :
            throw "FATAL ERROR getOffOpr2(): Unknown operand type.";
    }
    
}

uint64_t Instruction::_parseVal(string s){
   
            
            uint64_t ret;
            regex dec("^ *-?[[:digit:]]+$");
            regex hex("^ *-?0x[[:xdigit:]]+$");
            if(regex_match(s,dec)) return stoul(s,nullptr,10);
            if(regex_match(s,hex)) return stoul(s,nullptr,16);
            throw "Assembler Error: Could not parse value "+s;
}

bool Instruction::isOverflow(uint64_t x){
    
    int16_t xShort = x;
    return (xShort > CHAR_MAX || xShort < CHAR_MIN);
  
    
}

void Instruction::initMaps(){
    
    StrIDTMap = {
        {"halt",HALT},{"xchg",XCHG},{"int",INT},{"mov",MOV},
        {"add",ADD},{"sub",SUB},{"mul",MUL},{"div",DIV},
        {"not",NOT},{"and",AND},{"or",OR},{"xor",XOR},
        {"test",TEST},{"shl",SHL},{"shr",SHR},{"push",PUSH},{"pop",POP},
        {"jmp",JMP},{"jeq",JEQ},{"jne",JNE},{"jgt",JGT},{"call",CALL},
        {"ret",RET},{"iret",IRET},{"cmp",CMP}
    };
    IDTLambdaMap = {
        {HALT,_halt},{XCHG,_xchg},{INT,_int},{MOV,_mov},
        {ADD,_add},{SUB,_sub},{MUL,_mul},{DIV,_div},
        {CMP,_cmp},{NOT,_not},{AND,_and},{OR,_or},{XOR,_xor},
        {TEST,_test},{SHL,_shl},{SHR,_shr},{PUSH,_push},{POP,_pop},
        {JMP,_jmp},{JEQ,_jeq},{JNE,_jne},{JGT,_jgt},{CALL,_call},
        {RET,_ret},{IRET,_iret}
    };
    
}



regex Instruction::splitOneOpr("^(halt|xchg|int|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|push|pop|jmp|jeq|jne|jgt|call|ret|iret)(b|w)? +([^ ]*)$");
regex Instruction::splitTwoOpr("^(halt|xchg|int|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|push|pop|jmp|jeq|jne|jgt|call|ret|iret)(b|w)? +([^ ]*), *([^ ]*)$");
regex Instruction::isImm("^(-?((0x[[:xdigit:]]+)|([[:digit:]]+)))$");
regex Instruction::isImmDec("^(-?[[:digit:]]+)$");
regex Instruction::isImmSym("^&(_?[[:alpha:]][[:alnum:]]*)$");
regex Instruction::isRegDirNum("^r(0|1|2|3|4|5|6|7)(h|l)?$");
regex Instruction::isRegDirAlph("^(sp|pc|psw)(h|l)?$");
regex Instruction::isRegNumOff("^r(0|1|2|3|4|5|6|7)(h|l)?\\[(-?((0x[[:xdigit:]]+)|([[:digit:]]+)))\\]$");
regex Instruction::isRegAlphOff("^(sp|pc|psw)(h|l)?\\[(-?((0x[[:xdigit:]]+)|([[:digit:]]+)))\\]$");
regex Instruction::isRegNumSym("^r(0|1|2|3|4|5|6|7)(h|l)?\\[(_?[[:alpha:]][[:alnum:]]*)\\]$");
regex Instruction::isRegAlphSym("^(sp|pc|psw)(h|l)?\\[(_?[[:alpha:]][[:alnum:]]*)\\]$");
regex Instruction::isPcRelSym("^\\$(_?[[:alnum:]]+)$");
regex Instruction::isAbsSym("^(_?[[:alpha:]][[:alnum:]]*)$");
regex Instruction::isAbsData("^\\*(0x[[:xdigit:]]{4})$");
