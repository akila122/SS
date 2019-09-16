
//======================================================================
//======================================================================
//****             Course:  IR3SS                                   ****
//****             File:    main.cpp                                ****
//****             Info:    Emulator main Source                    ****
//****                                                              ****
//****             Student: Aleksa Rajkovic 248|2016      	    ****
//****             Date:    June 2019.                              ****
//======================================================================
//======================================================================

#include <string>
#include <iostream>
#include <regex>
#include "Emulator.h"


using namespace std;

int main(int argc, char** argv) {
   
    try{

        regex getPreff("^-place=(.+)@0x(.+)$");
        
        vector<string> files;
        map<string,uint16_t> preffMap;

	if (system("assembler -o IVTSetup.o /usr/bin/IVTSetup.s")<0)
		throw "Fatal Error: Could not resolve IVTSetup.s";	

	files.push_back("IVTSetup.o");            

        for(int i=1;i<argc;i++){
           
            string s(argv[i]);
            if(regex_match(s,getPreff)){
                smatch m;
                regex_match(s,m,getPreff);
                try{
                    preffMap.insert({{m[1],stoi(m[2],nullptr,16)}});
                }
                catch(...){
                    cout<<"Invalid command unable to parse -> "<<m[2].str()<<endl;
                    return -1;
                }
            }
            else{
                files.push_back(s);
            }
            
            if(files.empty()){
                cout<<"Invalid command no files were given."<<endl;
                return -1;
            }
            
        }   
        
        Emulator e(files,preffMap);
        
	system("rm IVTSetup.o");

    }
    catch(string s){
        cout<<s<<endl;
	return -1;
    }
    catch(char const* s){
        cout<<s<<endl;
	return -1;
    }
        
    
    return 0;
}
