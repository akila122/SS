
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Directive.cpp                           ****
//****             Info:    Directive.cpp class Source              ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#include "Directive.h"

Directive::Directive(string toParse, Section* _sec) : inputLine(toParse),alertAssembler(false),sec(_sec)
{
  
    if(DTLambdaMap.empty()) initMaps();
    regex e("^\\.(global|extern|equ|text|data|bss|section|byte|word|align|skip) *.*$");
    smatch m;
    regex_search(inputLine,m,e);
    dirType = strDTMap.at(m[1]);
    DTLambdaMap.at(dirType)(this);
    
    
}


Directive::~Directive() {
}



void Directive::initMaps(){
    
    strDTMap = {
        
        {"global",GLOBAL},{"extern",EXTERN},{"equ",EQU},
        {"text",TEXT},{"data",DATA},{"bss",BSS},{"section",CUSTOM},
        {"byte",BYTE},{"word",WORD},{"align",ALIGN},{"skip",SKIP}
        
    };
    
    DTLambdaMap = {
        
        {GLOBAL,_global},{EXTERN,_extern},{EQU,_equ},
        {TEXT,_text},{DATA,_data},{BSS,_bss},{CUSTOM,_custom},
        {BYTE,_byte},{WORD,_word},{ALIGN,_align},{SKIP,_skip}
        
    };
}

void Directive::_global(Directive* _this){
    
    
    _this->symbolVector = split("global",_this->inputLine,",");
    regex isSym("^_?[[:alpha:]][[:alnum:]]*$");
    if(_this->symbolVector.empty()) throw "Assembler Error: "+_this->inputLine+" is not valid. Symbol names expected.";
    for(string s : _this->symbolVector)
        if(!regex_match(s,isSym)) throw "Assembler Error: "+_this->inputLine+" -> "+s+" is not a valid symbol name.";
    
    
    
    
};
void Directive::_extern(Directive* _this){
    
    _this->symbolVector = split("extern",_this->inputLine,",");
    regex isSym("^_?[[:alpha:]][[:alnum:]]*$");
    if(_this->symbolVector.empty()) throw "Assembler Error: "+_this->inputLine+" is not valid. Symbol names expected.";
    for(string s : _this->symbolVector)
        if(!regex_match(s,isSym)) throw "Assembler Error: "+_this->inputLine+" -> "+s+" is not a valid symbol name.";
    
    
};
void Directive::_equ(Directive* _this){
    
    _this->symbolVector = split("equ",_this->inputLine,",");
    if (_this->symbolVector.size()!=2)
        throw "Assembler Error: Invalid .equ directive -> "+_this->inputLine;
    regex isSym("^_?[[:alpha:]][[:alnum:]]*$");
    if(!regex_match(_this->symbolVector[0],isSym))
        throw "Assembler Error: Invalid .equ directive -> "+_this->inputLine+". "+_this->symbolVector[0]+" is not valid symbol declaration.";
    _this->exprs.push_back(Expression(_this->symbolVector[1]));
    for(string s : _this->exprs[0].symbolOperands)
        if (s == _this->symbolVector[0])
            throw "Assembler Error: Invalid .equ directive, circular definition encountered -> "+_this->inputLine;
    if (_this->exprs[0].canEval()){
        uint16_t val = _this->exprs[0].eval();
        _this->byteVector.push_back(val);
        _this->byteVector.push_back(val>>8);
    }
    else _this->alertAssembler = true;

    
};
void Directive::_text(Directive* _this){
    
    _this->symbolVector.push_back(".text");
    _this->byteVector.push_back(1);
    
};
void Directive::_data(Directive* _this){
    
    _this->symbolVector.push_back(".data");
    _this->byteVector.push_back(6);
    
};
void Directive::_bss(Directive* _this){

    _this->symbolVector.push_back(".bss");
    _this->byteVector.push_back(6);
    
};
void Directive::_custom(Directive* _this){
    
    regex newSection("^\\.section *([[:alnum:]]+),?((r?w?x?)|(w?r?x?)|(w?x?r?)|(r?x?w?)|(x?w?r?)|(x?r?w?))?$");
    if(!regex_match(_this->inputLine,newSection))
        throw "Assembler Error: Unable to parse custom section "+_this->inputLine;
    smatch m;
    regex_search(_this->inputLine,m,newSection);
    string name = ".";
    name += m[1].str();
    _this->symbolVector.push_back(name);
    _this->byteVector.push_back(0);
    
    if(m[2].compare("")){    
        string str = m[2];
        for(char x: str){
            switch(x){
                case 'r':
                    _this->byteVector[0]+=4;
                    break;
                case 'w':
                    _this->byteVector[0]+=2;
                    break;
                case 'x':
                    _this->byteVector[0]+=1;
                    break;
            }
        }
    }
    else _this->byteVector[0] = 7;
    
};

void Directive::exprToByte(bool force){

    for(Expression e : exprs){
        int16_t val = force ? e.forceEval() : e.eval();
        byteVector.push_back(val);
        if (dirType == DirectiveType::WORD)
            byteVector.push_back(val>>8);
    }
    
}
void Directive::_byte(Directive* _this){
    
    _this->symbolVector = split("byte",_this->inputLine,",");
    if(_this->symbolVector.empty()) throw "Assembler Error: "+_this->inputLine+" is not valid. Byte values or expressions expected.";
    for(string s : _this->symbolVector){
        
        _this->exprs.push_back(Expression(s));
    }
    for(Expression e : _this->exprs)
        if (!e.canEval()){
            _this->alertAssembler = true;
            _this->exprToByte(true);
            return;
        }
    _this->exprToByte(false);
    
};
void Directive::_word(Directive* _this){
   
    _this->symbolVector = split("word",_this->inputLine,",");
    if(_this->symbolVector.empty()) throw "Assembler Error: "+_this->inputLine+" is not valid. Word values or expressions expected.";
    for(string s : _this->symbolVector){
        _this->exprs.push_back(Expression(s));
    }
    for(Expression e : _this->exprs)
        if (!e.canEval()){
            _this->alertAssembler = true;
            _this->exprToByte(true);
            return;
        }
    _this->exprToByte(false);
    
};
//The alignment requirement of the output section is the strictest alignment of any input section contained within the output section!!!
void Directive::_align(Directive* _this){

    
    uint16_t lc = _this->sec ? _this->sec->lc : 0;
    
    _this->symbolVector = split("align",_this->inputLine,",");
    uint8_t pad = _this->symbolVector.size() > 0 ? Instruction::_parseVal(_this->symbolVector[0]) : 2;
    if(pad == 0 || pad>= 16) throw "Assembler Error: "+_this->inputLine+" : first parameter of .align must be greater than zero and lesser than sixteen.";
    uint8_t fill = _this->symbolVector.size() > 1 ? Instruction::_parseVal(_this->symbolVector[1]) : 0;
    uint8_t max = _this->symbolVector.size() > 2 ? Instruction::_parseVal(_this->symbolVector[2]) : UCHAR_MAX;
    
    if(_this->sec && _this->sec->flags%2)
        fill = 0; //NOPS

    uint16_t mask = ~(0xFFFF << pad);
    uint16_t x = 1 << (pad-1);
    while((mask & (x ^ lc) )!= x){
        lc++;
        _this->byteVector.push_back(fill);
    }
    
    if(_this->byteVector.size() > max) _this->byteVector.clear();
    else if(_this->sec && _this->sec->maxAlign < pad)  _this->sec->maxAlign = pad;
    
    
};
void Directive::_skip(Directive* _this){
    
    _this->symbolVector = split("skip",_this->inputLine,",");
    if (_this->symbolVector.size() > 2 || _this->symbolVector.size() < 1) throw "Assembler Error: "+_this->inputLine+" is not valid.";
    uint8_t fill = _this->symbolVector.size() == 2 ? Instruction::_parseVal(_this->symbolVector[1]) : 0;
    int16_t sz = Instruction::_parseVal(_this->symbolVector[0]);
    if(sz <= 0) throw "Assembler Error: "+_this->inputLine+" : first parameter of .skip must be greater than zero.";
    while(sz--) _this->byteVector.push_back(fill);
    
    
};


map<string,DirectiveType> Directive::strDTMap = {};
map<DirectiveType,void (*)(Directive*)> Directive::DTLambdaMap = {};

vector<string> Directive::split(string dir,string s1,string delimiter){

    regex e("^\\.+"+dir+" (.*)$");
    smatch m;
    regex_match(s1,m,e);
    string s = m[1];
    size_t pos = 0;
    vector<string> ret;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        ret.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    ret.push_back(s);
    return ret;
    
}
    
    
