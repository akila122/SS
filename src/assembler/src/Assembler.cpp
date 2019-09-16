//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Assembler.cpp                           ****
//****             Info:    Assembler class Source                  ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#include "Assembler.h"

Assembler::Assembler(string s,string s2) : inputFile(s),outputFile(s2) {

    tokenize();
   
    firstRun();
    
    checkSyms();
    secondRun();
    writeText();


}

Assembler::~Assembler() {}

void Assembler::checkSyms(){
    
    for(auto& x : TNS)
        for(auto& s : x.second.symbolOperands)
            if (symTable.find(s) == symTable.end() && TNS.find(s) == TNS.end())
                throw "Assembler Error: Usage of non declared symbol -> "+s;
        for(auto& x : exprRelocations)
            for(auto& e : x.exprs)
                for(auto& s : e.symbolOperands)
                    if (symTable.find(s) == symTable.end() && TNS.find(s) == TNS.end())
                        throw "Assembler Error: Usage of non declared symbol -> "+s;
                
    bool changed = false;
    do{
        changed = false;
    for(auto& x : TNS){
        bool canEval = true;
        for(auto& s : x.second.symbolOperands){
            if(symTable.find(s) != symTable.end() && symTable.at(s).isEqu) continue;
            else{
                canEval = false;
                break;
            }
        }
        if(canEval){
            auto oprs = x.second.symbolOperands;
            for(auto& s : oprs)
                x.second.addOperand(s,symTable.at(s).val);
            symTable.insert({{x.first, Symbol(x.first, "ABS", true, false, x.second.eval(), 2, true)}});
            TNS.erase(TNS.find(x.first));
            changed = true;
            break;
        }
    }
    }while(changed);
    
}

void Assembler::firstRun() {



    for (string token : tokens) {
        
        
        
        if (regex_match(token, regex("^\\.(global|extern|equ|text|data|bss|section|byte|word|align|skip) *[^ ]*$"))) {
            Directive d(token, currSection);

            switch (d.dirType) {
                case GLOBAL:
                    _global(d, false);
                    break;
                case EXTERN:
                    _extern(d, false);
                    break;
                case EQU:
                    _equ(d, false);
                    break;
                case BYTE:
                    _byte(d, false);
                    break;
                case WORD:
                    _word(d, false);
                    break;
                case ALIGN:
                    _align(d, false);
                    break;
                case SKIP:
                    _skip(d, false);
                    break;
                default:
                    _custom(d, false);
            }
        }else if (regex_match(token, regex("^(halt|xchg|int|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|push|pop|jmp|jeq|jne|jgt|call|ret|iret)(b|w)? *[^ ]*$")))
            newInstruction(token, false); 
        else if (regex_match(token, regex("^_?[[:alpha:]][[:alnum:]]*$")))
            newSymbol(token, false);
        
        else throw "Assembler Error: Unknown token -> " + token;

    }


}

void Assembler::secondRun() {
    
    
    
    for(auto& sec : sections){
            
        symTable.at(sec.first).align = sec.second->maxAlign;
        sec.second->lc = 0;
        
    }
    
    for (string token : tokens) {

        

        if (regex_match(token, regex("^\\.(global|extern|equ|text|data|bss|section|byte|word|align|skip) *[^ ]*$"))) {
            Directive d(token, currSection);

            switch (d.dirType) {
                case GLOBAL:
                    _global(d, true);
                    break;
                case EXTERN:
                    _extern(d, true);
                    break;
                case EQU:
                    _equ(d, true);
                    break;
                case BYTE:
                    _byte(d, true);
                    break;
                case WORD:
                    _word(d, true);
                    break;
                case ALIGN:
                    _align(d, true);
                    break;
                case SKIP:
                    _skip(d, true);
                    break;
                default:
                    _custom(d, true);
            }
        } else if (regex_match(token, regex("^(halt|xchg|int|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|push|pop|jmp|jeq|jne|jgt|call|ret|iret)(b|w)? *[^ ]*$")))
            newInstruction(token, true); 
        else if (regex_match(token, regex("^_?[[:alpha:]][[:alnum:]]*$")))
            newSymbol(token, true);
            
        else throw "Assembler Error: Unknown token -> " + token;

    }
    
    for(auto& sec : sections){
            
        symTable.at(sec.first).size = sec.second->data.size();
        
        
    }
    
}

void Assembler::writeText(){
    
    ofstream out(outputFile);
    if(out.fail()) 
        throw "Assembler Error: Unable to open output file -> "+outputFile;
    
    out<<"<<<<SYMBOL TABLE ["<<symTable.size()<<"]>>>>"<<endl<<endl;
    
    out<<"SECTION\t\t\t"<<"NAME\t\t\t"<<"DEFINED\t\t\t"<<"SCOPE\t\t\t"<<"VALUE\t\t\t"<<"SIZE\t\t\t"<<"ALIGN\t\t\t"<<endl;
    for(auto& sym : symTable){
        out<<sym.second.section<<"\t\t\t"<<sym.second.name<<"\t\t\t"<<sym.second.defined<<"\t\t\t";
        if(sym.second.scope) out<<"GLOBAL";
        else out<<"LOCAL";
        out<<"\t\t\t"<<sym.second.val<<"\t\t\t"<<sym.second.size<<"\t\t\t"<<(int)sym.second.align<<endl;    
    }
    out<<endl<<"#END#";
    
    out<<endl<<endl<<"<<<<TNS TABLE ["<<TNS.size()<<"]>>>>"<<endl<<endl;
    
    out<<"SYMBOL\t\t\t"<<"SCOPE\t\t\t"<<"EXPRESSION\t\t\t"<<endl;
    for(auto& sym : TNS){
        out<<sym.first<<"\t\t\t";
        if(sym.second.promoteGlobal) out<<"GLOBAL\t\t\t";
        else out<<"LOCAL\t\t\t";
        out<<sym.second.ex<<endl;
    }
    out<<endl<<"#END#";
    out<<endl<<endl<<"<<<<RELOCATION TABLE ["<<exprRelocations.size()<<"]>>>>"<<endl<<endl;
  
    out<<"SECTION\t\t\t"<<"OFFSET\t\t\t"<<"LENGTH\t\t\t"<<"SIZE\t\t\t"<<"TYPE\t\t\t"<<"EXPRESSION\t\t\t"<<endl;
    
    
    for(auto& x : exprRelocations){
        out<<x.section<<"\t\t\t"<<x.offset<<"\t\t\t"<<x.exprs.size()<<"\t\t\t";
        if(x.sz) out<<"WORD";
        else out<<"BYTE";
        out<<"\t\t\t";
                
        if(x.relType == REL_ABS) out<<"REL_ABS";
        else out <<"REL_PC";
        out<<"\t\t\t";
        string s = "";
        for(int i=0; i<x.exprs.size();i++){
            s+=x.exprs[i].ex;
            if(i!=x.exprs.size()-1) s+=",";
        }
        out<<s<<endl;
    }
    out<<endl<<"#END#";
    
    out<<endl<<endl<<"<<<<SECTIONS ["<<sections.size()-(sections.find(".bss")!=sections.end())<<"]>>>>";
    
    for(auto&x : sections){
        
        if(x.first==".bss") continue;
        
        out<<endl<<endl<<"<<<<SECTION - "<<x.first<<">>>>"<<endl;x.second;
        out<<"[Size = "<<x.second->data.size()<<"bytes | Flags = ";
        string f = "";
        if (x.second->flags & 4) f+="r";
        if (x.second->flags & 2) f+="w";
        if (x.second->flags & 1) f+="x";
        out<<f<<" | MaxAlign = "<<(int)x.second->maxAlign<<"]"<<endl<<endl;
        
        int i=0;
        
        ios_base::fmtflags _f(out.flags());
        
        for(auto& b : x.second->data){
            if(b>=0 && b<=15) out<<0;
            out<<hex<<uppercase<<(int)b<<" ";
            if(++i%8 == 0) out<<endl;
        }
        out<<endl<<endl<<"#END#";
        
        out.flags(_f);
        
        
    }
    
        
    
  
    
    
}

void Assembler::newSymbol(string token, bool run) {
    
    if (!run) {
        if (!currSection)
            throw "Assembler Error: Symbol definition found at invalid place -> " + token;
        if (symTable.find(token) != symTable.end() || TNS.find(token) != TNS.end())
            throw "Assembler Error: Multiple definition is not allowed -> " + token;
        symTable.insert({{token, Symbol(token, currSection->name, true, false, currSection->lc, 0)}});
    }



}

void Assembler::newInstruction(string token, bool run) {


    
    if (!currSection || !currSection->flags % 2)
        throw "Assembler Error: Instruction found in invalid place -> " + token;

    Instruction i(token);
    

    
    if (run) {

 
        
        if (i.op1RelType == ALERT_AS) {

            if (i.op1SynType == IMM_SYM) {
                if (symTable.find(i.op1Sym) == symTable.end() && TNS.find(i.op1Sym) == TNS.end())
                    throw "Assembler Error: Usage of non declared symbol is forbidden -> " + i.op1Sym;
                if (symTable.find(i.op1Sym) != symTable.end() && symTable.at(i.op1Sym).isEqu) {
                    //Its legal: .equ symbol  address is the same as their value!!!
                    i.byteVector[2] = symTable.at(i.op1Sym).val;
                    if (i.opSz) i.byteVector[3] = symTable.at(i.op1Sym).val >> 8;
                } else exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + 2,Expression(i.op1Sym), i.opSz, REL_ABS));
                
            }
            if (i.op1SynType == REG_SYM) {
                if (symTable.find(i.op1Sym) == symTable.end() && TNS.find(i.op1Sym) == TNS.end())
                    throw "Assembler Error: Usage of non declared symbol is forbidden -> " + i.op1Sym;
                if (symTable.find(i.op1Sym) != symTable.end() && symTable.at(i.op1Sym).isEqu) {
                    //Its legal: .equ symbols  address is the same as their value!!!
                    i.byteVector[2] = symTable.at(i.op1Sym).val;
                    i.byteVector[3] = symTable.at(i.op1Sym).val >> 8;
                } else exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + 2, Expression(i.op1Sym), true, REL_ABS));
            }
            if (i.op1SynType == SYM_PC) {

                if (symTable.find(i.op1Sym) == symTable.end() && TNS.find(i.op1Sym) == TNS.end())
                    throw "Assembler Error: Usage of non declared symbol is forbidden -> " + i.op1Sym;
                if (symTable.find(i.op1Sym) != symTable.end()) {

                    if (symTable.at(i.op1Sym).isEqu) {

                        i.byteVector[2] = symTable.at(i.op1Sym).val;
                        i.byteVector[3] = symTable.at(i.op1Sym).val >> 8;
                        
                    } else if (symTable.at(i.op1Sym).defined && symTable.at(i.op1Sym).section==currSection->name) {
                        
                        
                        
                        int16_t toAdd = symTable.at(i.op1Sym).val - (currSection->lc + i.byteVector.size());
                        i.byteVector[2] = toAdd;
                        i.byteVector[3] = toAdd >> 8;
                    } else {
                        int16_t toAdd = -(i.byteVector.size()-2);
                        exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + 2,Expression(i.op1Sym), true, REL_PC,i.op1Sym));
                        i.byteVector[2] = toAdd;
                        i.byteVector[3] = toAdd >> 8;
                    }
                } else {
                    
                    int16_t toAdd = -(i.byteVector.size()-2);
                    exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + 2,Expression(i.op1Sym), true, REL_PC,i.op1Sym));
                    i.byteVector[2] = toAdd;
                    i.byteVector[3] = toAdd >> 8;
                }
            }
            if (i.op1SynType == SYM_ABS) {

                if (symTable.find(i.op1Sym) == symTable.end() && TNS.find(i.op1Sym) == TNS.end())
                    throw "Assembler Error: Usage of non declared symbol is forbidden -> " + i.op1Sym;
                if (symTable.find(i.op1Sym) != symTable.end() && symTable.at(i.op1Sym).isEqu) {

                    i.byteVector[2] = symTable.at(i.op1Sym).val;
                    i.byteVector[3] = symTable.at(i.op1Sym).val >> 8;

                } else exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + 2, Expression(i.op1Sym), true, REL_ABS));
            }
        }
        if (i.op2RelType == ALERT_AS) {

            uint16_t start = i.getOffOpr2();

            if (i.op2SynType == IMM_SYM) {
                if (symTable.find(i.op2Sym) == symTable.end() && TNS.find(i.op2Sym) == TNS.end())
                    throw "Assembler Error: Usage of non declared symbol is forbidden -> " + i.op2Sym;
                if (symTable.find(i.op2Sym) != symTable.end() && symTable.at(i.op2Sym).isEqu) {
                    //Its legal: .equ symbol  address is the same as their value!!!

                    i.byteVector[start] = symTable.at(i.op2Sym).val;
                    if (i.opSz) i.byteVector[start + 1] = symTable.at(i.op2Sym).val >> 8;
                } else exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + start, Expression(i.op2Sym), i.opSz, REL_ABS));
            }
            if (i.op2SynType == REG_SYM) {
                if (symTable.find(i.op2Sym) == symTable.end() && TNS.find(i.op2Sym) == TNS.end())
                    throw "Assembler Error: Usage of non declared symbol is forbidden -> " + i.op2Sym;
                if (symTable.find(i.op2Sym) != symTable.end() && symTable.at(i.op2Sym).isEqu) {
                    //Its legal: .equ symbols  address is the same as their value!!!
                    i.byteVector[start] = symTable.at(i.op2Sym).val;
                    i.byteVector[start + 1] = symTable.at(i.op2Sym).val >> 8;
                } else exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + start, Expression(i.op2Sym), true, REL_ABS));
            }
            if (i.op2SynType == SYM_PC) {

                if (symTable.find(i.op2Sym) == symTable.end() && TNS.find(i.op2Sym) == TNS.end())
                    throw "Assembler Error: Usage of non declared symbol is forbidden -> " + i.op2Sym;
                if (symTable.find(i.op2Sym) != symTable.end()) {
                    
                    if (symTable.at(i.op2Sym).isEqu) {

                        
                        i.byteVector[start] = symTable.at(i.op2Sym).val;
                        i.byteVector[start + 1] = symTable.at(i.op2Sym).val >> 8;
                        

                    } else if (symTable.at(i.op2Sym).defined && symTable.at(i.op2Sym).section==currSection->name) {
                        int16_t toAdd = symTable.at(i.op2Sym).val - (currSection->lc + i.byteVector.size());
                        i.byteVector[start] = toAdd;
                        i.byteVector[start + 1] = toAdd >> 8;
                    }
                    else {
                        
                    int16_t toAdd = -(i.byteVector.size() - start);
                    exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + start, Expression(i.op2Sym), true,REL_PC,i.op2Sym));
                    i.byteVector[start] = toAdd;
                    i.byteVector[start + 1] = toAdd >> 8;
                        
                    }
                } else {
                    
                    
                    int16_t toAdd = -(i.byteVector.size() - start);
                    exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + start, Expression(i.op2Sym), true,REL_PC,i.op2Sym));
                    i.byteVector[start] = toAdd;
                    i.byteVector[start + 1] = toAdd >> 8;
                }
            }
            if (i.op2SynType == SYM_ABS) {

                if (symTable.find(i.op2Sym) == symTable.end() && TNS.find(i.op2Sym) == TNS.end())
                    throw "Assembler Error: Usage of non declared symbol is forbidden -> " + i.op2Sym;
                if (symTable.find(i.op2Sym) != symTable.end() && symTable.at(i.op2Sym).isEqu) {

                    i.byteVector[start] = symTable.at(i.op2Sym).val;
                    i.byteVector[start] = symTable.at(i.op2Sym).val >> 8;

                } else exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc + start, Expression(i.op2Sym), true, REL_ABS));
            }
        }
    
        for(auto& b : i.byteVector)
            currSection->data.push_back(b);    
        
    }

    
    currSection->lc += i.byteVector.size();


}

void Assembler::tokenize() {

    ifstream in(inputFile);
    
    if(in.fail()){
        throw "Assembler Error: Unable to open file -> "+inputFile;
    }

    regex isSym("^(_?[[:alpha:]][[:alnum:]]*) *: *$");
    regex isSymElse("^(_?[[:alpha:]][[:alnum:]]*) *: *([^ ][^:]+[^ ]) *$");
    regex removeTab("\\t");
    regex removeSpaces(" {2,}");
    regex fix(" ?, ?");
    regex removeComm("([^#]*)#.*");
    regex removeAtEnd("^(.*[^ ])( *)$");
    regex removeAtStart("^( *)([^ ].*)$");


    for (string l; getline(in, l);) {

        l = regex_replace(l, removeTab, "");
        l = regex_replace(l, removeSpaces, " ");
        l = regex_replace(l, fix, ",");
        l = regex_replace(l, removeComm, "$1", regex_constants::format_first_only);
        l = regex_replace(l,removeAtEnd,"$1");
        l = regex_replace(l,removeAtStart,"$2");
        
        if (l == " " || l == "") continue;
        else if (l == ".end") break;

        else if (regex_match(l, isSymElse)) {

            smatch m;
            regex_search(l, m, isSymElse);
            if (m[1] != " " && m[1] != "")tokens.push_back(m[1]);
            if (m[1] != " " && m[2] != "") tokens.push_back(m[2]);

        } else if (regex_match(l, isSym)) {

            smatch m;
            regex_search(l, m, isSym);
            if (m[1] != " " && m[1] != "") tokens.push_back(m[1]);

        } else {
            tokens.push_back(l);
        }

    }

}

void Assembler::_global(Directive& d, bool run) {

    if (run) {

        for(auto& sym : d.symbolVector){
        
        if (symTable.find(sym) == symTable.end() && TNS.find(sym) == TNS.end())
            throw "Assembler Error: Definition of symbol not found -> " + sym;
        if (symTable.find(sym) != symTable.end() && symTable.at(sym).scope)
            throw "Assembler Error: Combination of .extern and .global not allowed -> " + d.symbolVector[0];
        if (symTable.find(sym) != symTable.end())
            symTable.at(sym).scope = true;
        else TNS.at(sym).promoteGlobal = true;
        }
    }

};

void Assembler::_extern(Directive& d, bool run) {

    if (!run) {

        for(auto& sym : d.symbolVector){
        if (symTable.find(sym) != symTable.end() || TNS.find(sym) != TNS.end())
            throw "Assembler Error: Definition of extern declared symbol is not allowed -> " + d.inputLine;
        symTable.insert({
            {sym, Symbol(sym, "UND", false, true, 0, 0, false)}});
        }
    }


};

void Assembler::_equ(Directive& d, bool run) {

    if (!run) {
        if (d.alertAssembler) {
            if (symTable.find(d.symbolVector[0]) != symTable.end() || TNS.find(d.symbolVector[0]) != TNS.end())
                throw "Assembler Error: Multiple .equ definition is not allowed -> " + d.inputLine;
            TNS.insert({
                {d.symbolVector[0], d.exprs[0]}});
        } else {
            if (symTable.find(d.symbolVector[0]) != symTable.end() || TNS.find(d.symbolVector[0]) != TNS.end())
                throw "Assembler Error: Multiple definition is not allowed -> " + d.inputLine;
            symTable.insert({
                {d.symbolVector[0], Symbol(d.symbolVector[0], "ABS", true, false, (d.byteVector[1] << 8) | d.byteVector[0], 2, true)}});
        }
    }

};

void Assembler::_custom(Directive& d, bool run) {

    if (!run) {
        if (sections.find(d.symbolVector[0]) != sections.end())
            currSection = sections.at(d.symbolVector[0]);
        else {
            currSection = new Section(d.symbolVector[0], d.byteVector[0]);
            sections.insert({
                {d.symbolVector[0], currSection}});
            symTable.insert({
                {d.symbolVector[0], Symbol(d.symbolVector[0], d.symbolVector[0], true, false, 0, 0)}});
        }
    } else currSection = sections.at(d.symbolVector[0]);




};

void Assembler::_byte(Directive& d, bool run) {

    if (!currSection || currSection->flags == 1)
        throw "Assembler Error: .byte found where it cannot be used -> " + d.inputLine;

    currSection->lc += d.byteVector.size();

    if (!run) {

        if (d.alertAssembler)
            exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc-d.byteVector.size(), d.exprs, false,REL_ABS));
    }
    else {
        for (auto b : d.byteVector)
            currSection->data.push_back(b);
    }

};

void Assembler::_word(Directive& d, bool run) {


    if (!currSection || currSection->flags == 1)
        throw "Assembler Error: .word found where it cannot be used -> " + d.inputLine;

    currSection->lc += d.byteVector.size();

    if (!run) {
        if (d.alertAssembler) {
            exprRelocations.push_back(ExprRelocation(currSection->name, currSection->lc-d.byteVector.size(), d.exprs, true,REL_ABS));
        }
    } else {
        for (auto b : d.byteVector)
            currSection->data.push_back(b);
    }


};

void Assembler::_align(Directive& d, bool run) {

    if (!currSection)
        throw "Assembler Error: .align found where it cannot be used -> " + d.inputLine;
    currSection->lc += d.byteVector.size();
    if (run) {

        for (auto b : d.byteVector)
            currSection->data.push_back(b);

    }


};

void Assembler::_skip(Directive& d, bool run) {

    if (!currSection)
        throw "Assembler Error: .skip found where it cannot be used -> " + d.inputLine;

    currSection->lc += d.byteVector.size();

    if (run) {

        for (auto b : d.byteVector)
            currSection->data.push_back(b);

    }
};

Section* Assembler::currSection = nullptr;

