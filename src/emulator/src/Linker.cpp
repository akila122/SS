
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    Linker.cpp                              ****
//****             Info:    Linker class Source                     ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#include "Linker.h"
#include "MemAllocator.h"

Linker::Linker(vector<string> files, map<string, uint16_t> preff) {



    for (auto& sec : preff){
	sections.insert({
            {sec.first, SectionInfo(sec.first, 0, sec.second, true)}
        });
    }
	
    mem = new uint8_t[0x10000]();
    flags = new uint8_t[0x10000]();
    


    for (auto& f : files)
        objFiles.push_back(ObjectFile(f));

    resolveGlobals();
    resolveSections();
    resolveSymbols();
    mapGlobals();
    resolveTNS();
    resolveRelocs();
    generateMem();
    resolveIVT();



}

Linker::~Linker() {
}

void Linker::resolveGlobals() {

    for (auto& f : objFiles) {
        for (auto& s : f.symTable) {
            if (s.second.defined && s.second.scope) {
                if (globals.find(s.first) != globals.end())
                    throw "Linker Error: Multiple definition of global symbol encountered -> " + s.first;
                globals.insert({
                    {s.first, 0}});
            }
        }
        for (auto& s : f.TNS) {
            if (s.second.promoteGlobal)
                if (globals.find(s.first) != globals.end())
                    throw "Linker Error: Multiple definition of global symbol encountered -> " + s.first;
        }
    }

    for (auto& f : objFiles)
        for (auto& s : f.symTable)
            if (s.second.scope && globals.find(s.first) == globals.end()) {
                bool ok = false;
                for (auto& f2 : objFiles) {
                    if (f2.TNS.find(s.first) != f2.TNS.end() && f2.TNS.at(s.first).promoteGlobal) {
                        ok = true;
                        break;
                    }
                }
                if (!ok)
                    throw "Linker Error: Definition of symbol not found -> " + s.first;
            }


}

void Linker::resolveSections() {


    for (auto& f : objFiles)
        for (auto& sym : f.symTable) {
            if (sym.second.name == sym.second.section) {
                if (sections.find(sym.first) == sections.end())
                    sections.insert({
                        {sym.first, SectionInfo(sym.first, 0, 0, false)}
                    });
                sections.at(sym.first).totalSz += sym.second.size;
            }
        }


}

void Linker::resolveSymbols() {

    MemAllocator memAlloc(CODE_START_ADR, CODE_SZ);

    for (auto& sec : sections) {

        if (sec.second.preff) {

            for (auto& f : objFiles) {

                if (f.symTable.find(sec.first) != f.symTable.end()) {

                    
                    
                    uint16_t startAdr;
                    try {
                        startAdr = memAlloc.allocAt(sec.second.nextAdr, f.symTable.at(sec.first).size, f.symTable.at(sec.first).align);
                    }                    catch (string s) {
                        throw "Linker Error: Allocation failed -> " + sec.first + "\n" + s;
                    }
                    sec.second.nextAdr = startAdr + f.symTable.at(sec.first).size;

                    for (auto& sym : f.symTable)
                        if (sym.second.section == sec.first) {
                            sym.second.val += startAdr;
                        }
                }
            }
        }
    }

    for (auto& sec : sections) {


        if (!sec.second.preff) {

            bool init = true;

            for (auto& f : objFiles) {

                if (f.symTable.find(sec.first) != f.symTable.end()) {


                    uint16_t startAdr;

                    if (init) {

                        try {
                            startAdr = memAlloc.allocFree(f.symTable.at(sec.first).size, f.symTable.at(sec.first).align);
                            init = false;
                        }                        catch (string s) {
                            throw "Linker Error: Allocation failed. There is no space for -> " + sec.first + "\n" + s;
                        }

                    } else {
                        try {
                            startAdr = memAlloc.allocAt(sec.second.nextAdr, f.symTable.at(sec.first).size, f.symTable.at(sec.first).align);
                        }                        catch (string s) {
                            throw "Linker Error: Allocation failed -> " + sec.first + "\n" + s;
                        }
                    }

                    sec.second.nextAdr = startAdr + f.symTable.at(sec.first).size;
                    for (auto& sym : f.symTable)
                        if (sym.second.section == sec.first) {
                            sym.second.val += startAdr;
                        }

                }

            }

        }
    }
}

void Linker::mapGlobals() {

    for (auto& sym : globals) {
        for (auto& f : objFiles) {
            if (f.symTable.find(sym.first) != f.symTable.end() && f.symTable.at(sym.first).defined && f.symTable.at(sym.first).scope) {
                sym.second = f.symTable.at(sym.first).val;
                break;
            }
        }
    }

    for (auto& f : objFiles) {
        for (auto& sym : f.symTable) {
            if (sym.second.scope && !sym.second.defined && globals.find(sym.first) != globals.end()) {
                sym.second.val = globals.at(sym.first);
                sym.second.defined = true;
            }
        }
    }
}

void Linker::resolveTNS() {

    bool changeMade = false;

    do {

        changeMade = false;

        for (auto& f : objFiles) {

            for (auto& entry : f.TNS) {

                bool check = true;

                
                for (auto& sym : entry.second.symbolOperands) {
                    if (f.symTable.find(sym)==f.symTable.end() || !f.symTable.at(sym).defined) {
                        check = false;
                        break;
                    }
                }

                
                
                if (check) {
                  
                    
                    changeMade = true;
                    auto oprs = entry.second.symbolOperands;
                  
                    
                    
                    for (auto& opr : oprs)
                        entry.second.addOperand(opr, f.symTable.at(opr).val);
                    uint16_t eval = entry.second.eval();
          
                    
                    f.symTable.insert({
                        {entry.first, Symbol(entry.first, "ABS", true, entry.second.promoteGlobal, eval, 2, true)}});
                    if (entry.second.promoteGlobal) {
                        
                        globals.insert({
                            {entry.first, eval}});
                         
                        for (auto& f2 : objFiles) {
                           
                            
                            if (f2.symTable.find(entry.first) != f2.symTable.end() && f2.symTable.at(entry.first).scope && !f2.symTable.at(entry.first).defined) {
                                
                                f2.symTable.at(entry.first).defined = true;
                                f2.symTable.at(entry.first).val = eval;
                                f2.symTable.at(entry.first).section = "ABS";
                            }
                        }
                    }
                   
                  f.TNS.erase(f.TNS.find(entry.first));
                  break;
                }
            }
        }

    } while (changeMade);

   
    
    string error="";
    bool ok = true;
    
    for(auto& f : objFiles){
        if(!f.TNS.empty()){
            ok = false;
            for(auto& s : f.TNS){
                error = error +s.first+" = "+ s.second.ex+"\n";
            }
        }
    }
    if(!ok){
        throw "Linker Error : Could not resolve following symbol definitions:\n"+error+"Check for circular definition.";
    }
    
}

void Linker::resolveRelocs() {
    
    
    for(auto& f : objFiles){
        
        for(auto& r : f.exprRelocations){
            
            for(auto& e : r.exprs){
                auto oprs = e.symbolOperands;
                for(auto& opr : oprs)
                    e.addOperand(opr,f.symTable.at(opr).val);
            }
            
            if(r.relType == REL_ABS){
                
                for(auto& e : r.exprs){
                    uint16_t eval = e.eval();
                    f.sections.at(r.section)->data[r.offset++] = eval;
                    if(r.sz) f.sections.at(r.section)->data[r.offset++] = eval>>8;
                }
                    
            }
            else{
                
                
                int16_t oldVal = f.sections.at(r.section)->data[r.offset+1];
                
                oldVal <<= 8;
                oldVal |= f.sections.at(r.section)->data[r.offset];
                
                oldVal += f.symTable.at(r.exprs[0].ex).val;
                
                oldVal -= (f.symTable.at(r.section).val+r.offset);
                
                f.sections.at(r.section)->data[r.offset] = oldVal;
                f.sections.at(r.section)->data[r.offset+1] = oldVal>>8;
                
                
            }
            r.section = "DONE";
        }
        
    }
    
    
    
}

void Linker::generateMem() {
    
    for(auto& f : objFiles)
        for(auto& s : f.sections){
            uint16_t off = f.symTable.at(s.first).val;
            for(uint16_t i = 0; i<s.second->data.size();i++){
                mem[off+i] = s.second->data[i];
		flags[off+i] = s.second->flags;		
		}
        }
            
}

void Linker::resolveIVT(){
    
    
    string s = "_start";
    try{
    _start = globals.at(s);
    }
    catch(...){
        throw "Linker Error: _start entry point not found.";
    }
    string iv = "_IVT";
    
    try{
    for(int i=0; i<8; i++){
        IVT[i] = globals.at(iv+to_string(i));
      
    }
    }
    catch(...){
        throw "Linker Error: IVT initialization failed. Could not find -> _IVT";
    }


    for(int i=0;i<15;i++)
	flags[i] = 4;
    for(uint16_t i=0; i<STACK_SZ; i++)
	flags[i+STACK_START_ADR] = 6;
    for(uint16_t i=0; i<MMREG_SZ; i++)
	flags[i+MMREG_START_ADR] = 6;  


    
    
}

void Linker::writeText(){
    
    for(auto& f : objFiles)
        f.writeText();
    
    cout<<endl;
    
}
