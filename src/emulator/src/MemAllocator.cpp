
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    MemAllocator.cpp                        ****
//****             Info:    MemAllocator class Source               ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#include "MemAllocator.h"

MemAllocator::~MemAllocator() {
}


uint16_t MemAllocator::allocFree(size_t sz,uint8_t align){

    
    for(list<Node>::iterator i = mem.begin(); i!=mem.end(); ++i){
        
        if(sz + alignAt(i->startAdr,align) <= i->size){
    
            
            uint16_t retAdr = alignAt(i->startAdr,align) + i->startAdr;
            
            uint16_t adr1 = i->startAdr;
            size_t sz1 = alignAt(i->startAdr,align);
            
            uint16_t adr2 = retAdr + sz;
            uint16_t sz2 = i->size - sz - sz1;
            
            i = mem.erase(i);
            
            
            if(sz2>0){
               
                i = mem.insert(i,Node(adr2,sz2));
            }
            
            if(sz1>0){
            
                i = mem.insert(i,Node(adr1,sz1));
            }
            
            return retAdr;
        }
        
    }
    
    throw "MemAllocator Error: Could not fit -> "+to_string(sz);
    

};
uint16_t MemAllocator::allocAt(uint16_t adr,size_t sz,uint8_t align){

    if(adr >= maxAdr  || adr+sz+alignAt(adr,align) >= maxAdr)
        throw "MemAllocator Error: "+to_string(adr)+" overflows MaxAdr for the CODE segment -> "+to_string(maxAdr);

    for(list<Node>::iterator i = mem.begin(); i!=mem.end(); ++i){
        
        if(i->startAdr <= adr && i->size >= sz + alignAt(adr,align)){
            
            
            uint16_t retAdr = alignAt(adr,align) + adr;
            
            uint16_t adr1 = i->startAdr;
            size_t sz1 = retAdr - i->startAdr;
            
            uint16_t adr2 = retAdr + sz;
            uint16_t sz2 = i->size - sz - sz1;
            
            i = mem.erase(i);
            
            if(sz2>0){
               
                i = mem.insert(i,Node(adr2,sz2));
            }
            
            if(sz1>0){
            
                i = mem.insert(i,Node(adr1,sz1));
            } 
                 
            return retAdr;
            
        }
        
    }
    
    throw "MemAllocator Error: Could not fit -> "+to_string(sz)+"bytes at wanted address "+to_string(adr);
    
    
    


};
size_t MemAllocator::alignAt(uint16_t adr,uint8_t align){
    
    size_t ret = 0;
    
    uint16_t mask = ~(0xFFFF << align);
    uint16_t x = 1 << (align-1);
    while((mask & (x ^ adr) )!= x) {
        ret++;
        adr++;
    }
    return ret;
        

    
};

void MemAllocator::scan(){
    
    for(auto& s : mem){
        cout<<"ADR: "<<s.startAdr<<" SIZE: "<<s.size<<endl;
    }
    
    
}
