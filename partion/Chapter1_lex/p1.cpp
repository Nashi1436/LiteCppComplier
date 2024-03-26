#include<bits/stdc++.h>
using namespace std;

#define dout fout
#define ll long long
#define pb push_back
#define rep(i,st,ed) for(int i=st,edi=ed;i<=edi;i++)
int argflag[22],usedarg;

const int TXT_LEN=1e6+10;
int rex[TXT_LEN],rey[TXT_LEN];

string remove_comments(const string &src) {     //  notice /*/ condition
    string res="";int len=src.size(),lines=1,linsti=-1,topres=0;
    rep(i,0,len-1){
        if(i+1<len&&src[i]=='/'&&src[i+1]=='/'){//  //**********
            i++;
            while(++i<len&&src[i]!='\n');
            i--;continue;
        }
        if(i+1<len&&src[i]=='/'&&src[i+1]=='*'){// /**********/
            i++;
            while((++i)+1<len&&!(src[i]=='*'&&src[i+1]=='/')) if(src[i]=='\n'){
                res+=src[i],rex[topres]=lines,rey[topres]=i-linsti,++topres;
                lines++;linsti=i;
            }
            i++;continue;
        }
        if(src[i]=='\"'||src[i]=='\''){         // "********"  '*************'
            int tmpc=src[i];
            res+=src[i],rex[topres]=lines,rey[topres]=i-linsti,++topres;
            while(++i<len&&src[i]!=tmpc){
                res+=src[i],rex[topres]=lines,rey[topres]=i-linsti,++topres;
                if(src[i]=='\n') lines++,linsti=i;
            }
            res+=src[i],rex[topres]=lines,rey[topres]=i-linsti,++topres;
            continue;
        }
        res+=src[i],rex[topres]=lines,rey[topres]=i-linsti,++topres;
        if(src[i]=='\n') lines++,linsti=i;
    }
    return res;
}


int main(int argc, char *argv[]) {

    string filein="p2.in",fileout="p2.out";

    //complie process
    if(argc>20){perror("Error:  Too many parameters");exit(1);}
    rep(i,1,argc-1){
        string tmp=argv[i];
        if(tmp=="-o"){
            if(i+1<argc){fileout=argv[++i];usedarg+=2;continue;}
            cerr<<"Error:  requre parameter (-ParaName FileName) to determine target file"<<endl;exit(1);
        }
        filein=tmp;
    }
    if(usedarg+2!=argc){
        cerr<<"Error:  parameter error, require [SouceFileName (-ParaName FileName)*]"<<endl;exit(1);
    }


    //file open
    ifstream file(filein);
    if (!file.is_open()) {
        cerr<<"Error:  fail to open the file: "+filein<<endl;exit(1);
    }
    string code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();cout<<"Source file read successfully"<<endl;

    ofstream fout(fileout);
    if (!fout.is_open()) {
        cerr<<"Error:  fail to open the file: "+filein<<endl;exit(1);
    }
    cout<<"Target file open successfully"<<endl;


    //work
    string no_cmts = remove_comments(code);
    dout << no_cmts;


    //end
    fout.close();
    cout<<"Process finished"<<endl;
    return 0;
}
