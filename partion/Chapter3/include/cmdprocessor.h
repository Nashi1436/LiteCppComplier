#ifndef CMDP
#define CMDP

#include "my_header.h"
#include <cstdio>

class CMDProcessor {// CMDProcessor
private:
    string file;
    string filein;
    string fileout;
    int usedarg;

public:
    CMDProcessor(int argc, char* argv[]) : file("sample1"),filein("sample1.in"), fileout("sample1.out"), usedarg(0) {process(argc, argv);}

    void process(int argc, char* argv[]) {
        if(argc>20){perror("Error:  Too many parameters");exit(1);}
        for(int i=1;i<=argc-1;i++){
            string tmp=argv[i];
            if(tmp=="-o"){
                if(i+1<argc){fileout=argv[++i];usedarg+=2;continue;}
                cerr<<"Error:  requre parameter (-ParaName FileName) to determine target file"<<endl;exit(1); 
            }
            filein=tmp;
        }
        if(usedarg+2<argc){
            cerr<<"Error:  parameter error, require [SouceFileName (-ParaName FileName)*]"<<endl;exit(1);
        }
        file=removeExtension(filein);
        // cout<<"File Name: "<<file<<endl;
        // cout<<"Input File Name: "<<filein<<endl;
        // cout<<"Output File Name: "<<fileout<<endl;
        // cout<<argv[0]<<" "<<argv[1]<<" : "<<argc<<endl;
    }

    string removeExtension(const string& filename) {
        size_t lastDot = filename.find_last_of(".");
        if (lastDot == string::npos) {
            return filename; 
        }
        return filename.substr(0, lastDot);
    }

    string getFile() const { return file; }
    string getFileIn() const { return filein; }
    string getFileOut() const { return fileout; }
};




#endif // CMDP
