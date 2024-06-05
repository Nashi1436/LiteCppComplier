#ifndef LEXER
#define LEXER

#include "my_header.h"
#include "token.h"


class Lexer {

private:
    int argflag[22],usedarg;            // used for CMD arg
    static const int TXT_LEN=1e4+10;           // the max len limit for souce code
    int rex[TXT_LEN],rey[TXT_LEN];
    static const int MX_TK=1e4+10,MX_OP=3;     // max number of tokens      max len of  operator 
    static const int DI_SZ=50,bsop=100,bsdeli=200,bshaku=300,bscg=400;     //deliver different kinds of table   bs<-base
    static const int bsnum=501,bsid=601;       

    // table
    string keywords[DI_SZ] = { //1-100
        "auto",  "typedef", "const", "unsigned", "signed", "extern",
        "register", "static", "volatile", "union",
        
        "void", "short", "int", "long", "float", "double", "char",
        "struct", "enum", 

        "if", "else", "switch","case", "for", "do", 
        "while", "goto", "continue", "break", "default",

        "sizeof", "return",
        "main"
    };
    string operators[DI_SZ] = {  //101--200 
        "<<=", ">>=", 
        "++","--", "==","&&", "||","!=",">=","<=","+=","-=","*=", "%=", "&=","/=", "^=", "|=","<<", ">>", "->","::",
        "+", "-", "*", "/", "%", ">",  "<", "!", "=",  "&", "|", "^", "~", "?", ":", ",", ".", 
    };
    string delimiters[DI_SZ] = { "(", ")", "[", "]", "{", "}" , ";"}; //201--300
    string haku[DI_SZ]={ " ", "\n" , "\t", "\r"};//301--400
    string change[DI_SZ]={ "\"", "\'" , };//401--500
    // number 501   identifier 601

    //map hash
    int topkey,topop,topdeli,tophaku,topcg;
    unordered_map<string,ll>mp;

    TOKEN token[MX_TK];

    int toptk;              // the top of stack

    inline void buildmap() {
        rep(i,0,DI_SZ-1) if(sizeof(keywords[i])) mp[keywords[i]]=++topkey;
        rep(i,0,DI_SZ-1) if(sizeof(operators[i])) mp[operators[i]]=++topop;  
        rep(i,0,DI_SZ-1) if(sizeof(delimiters[i])) mp[delimiters[i]]=++topdeli;
        rep(i,0,DI_SZ-1) if(sizeof(haku[i])) mp[haku[i]]=++tophaku;
        rep(i,0,DI_SZ-1) if(sizeof(change[i])) mp[change[i]]=++topcg;
    }

    bool isStValidChar(char c) {
        return isalpha(c)||c=='_';
    }
    bool isValidChar(char c) {
        return isalpha(c)||isdigit(c)||c=='_';
    }

    void getTokens(const string& input) {
        buildmap();
        string word;int len=input.length();
        rep(i,0,len-1){
            int c=input[i],mpc=0,nxtc=(i+1<len)?input[i+1]:0,tmpi=i;word.clear();
            if(mp.find(string(1,c))!=mp.end()) mpc=mp[string(1,c)];

            //get next token
            if(c=='\"'||c=='\''){//Strings
                word.pb(c);int reindex=-2;
                while(++i<len&&input[i]!=c||(input[i]==c&&reindex==i-1)){
                    word.pb(input[i]);
                    if(reindex!=i-1&&input[i]=='\\') reindex=i;
                } 
                word.pb(c);
                if(i==len){cerr<<"Line:"<<rex[tmpi]<<" COL:"<<rey[tmpi]<<"    Error: Unclosed string literal"<<endl;exit(1);}
                token[++toptk]=TOKEN{"Strings",mpc,word,rex[tmpi],rey[tmpi]};
                continue;
            }
            if(isdigit(c)){//Numbers
                word.pb(c);
                while(++i<len&&(isValidChar(input[i])||input[i]=='.')) word.pb(input[i]);
                i--;
                token[++toptk]=TOKEN{"Numbers",bsnum,word,rex[tmpi],rey[tmpi]};
                continue;
                /* and other number type    
                    0x3f  
                    032
                    0.25f
                    0.25lf
                    0.25ll
                    1.2e9
                */
            }
            if(isStValidChar(c)){//Keywords or Identifiers
                word.pb(c);
                while(++i<len&&isValidChar(input[i])) word.pb(input[i]);
                i--;
                if(mp.find(word)!=mp.end())//Keywords
                    token[++toptk]=TOKEN{"Keywords",mp[word],word,rex[tmpi],rey[tmpi]};
                else token[++toptk]=TOKEN{"Identifiers",bsid,word,rex[tmpi],rey[tmpi]};//Identifiers
                continue;
            }
            if(bsop<mpc&&mpc<=bshaku){//Operators or delimiters
                int oplen=MX_OP+1,flag=0;
                while(--oplen&&!flag) if(mp.find(word=input.substr(i,oplen))!=mp.end()){//find from long
                    int mpv=mp[word];
                    if(!(bsop<mpv&&mpv<=bshaku)) continue;
                    flag=1;
                    token[++toptk]=TOKEN{(mpv<=bsdeli)?("Operators"):("Delimiters"),mpv,word,rex[tmpi],rey[tmpi]};
                    i+=oplen-1;
                }  
                if(!flag){cerr<<"Line:"<<rex[tmpi]<<" COL:"<<rey[tmpi]<<"    Error: Operators or delimiters unknown"<<endl;exit(1);}
                continue;
            }
            if(c=='#'){//#define ** **
                word.pb(c);
                while(++i<len&&input[i]!='\n') word.pb(input[i]);
                i--;
                /*need to be processed*/
            }
            if(c=='\n'){continue;}
        }
    }

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

    string low_string(const string &src) {            //  down A-Z  to a-z
        string res="";
        rep(i,0,src.size()) res+=src[i]+('a'-'A')*('A'<=src[i]&&src[i]<='Z');
        return res;
    }


public:
    Lexer() : usedarg(0), topkey(0), topop(bsop), topdeli(bsdeli), tophaku(bshaku), topcg(bscg), toptk(0) {}

    void work(string filein){

        //file open
        cerr << std::left;
        ifstream file(filein);
        if (!file.is_open()) {
            cerr<<"Error:  fail to open the file: "+filein<<endl;exit(1);
        }
        string code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();cout<<"Source file read successfully"<<endl;

        //work
        string coderemove=remove_comments(code);// pre process
        string codelow=low_string(coderemove);//should to be closed
        getTokens(codelow);
        token[0].word=token[toptk+1].word="#";// boundary case
        cout<<"Lexer finished"<<endl;
    }

    void printTokens(string fileout){
        //file open
        cerr << std::left;
        ofstream fout(fileout);
        if (!fout.is_open()) {
            cerr<<"Error:  fail to open the file: "+fileout<<endl;exit(1);
        }
        cout<<"Target file open successfully"<<endl;

        //work
        fout << std::left;
        rep(i,1,toptk) {
            fout<<"("<<setw(3)<<token[i].sym<<" , "<<setw(25)<<token[i].word<<")"<<endl;
        }

        //end
        fout.close();
        cout<<"PrintTokens finished"<<endl;
    }


    int getToptk(){return toptk;}
    TOKEN getToken(int idx){return token[idx];}

};



#endif // LEXER_H







// #include "MY_H"

// int main(int argc, char *argv[]) {
//     string filein = "lex.in", fileout = "lex.out";

//     // Compile cmd process
//     if (argc > 20) {
//         perror("Error:  Too many parameters");
//         exit(1);
//     }
//     for (int i = 1; i < argc; i++) {
//         string tmp = argv[i];
//         if (tmp == "-o") {
//             if (i + 1 < argc) {
//                 fileout = argv[++i];
//                 continue;
//             }
//             cerr << "Error:  require parameter (-ParaName FileName) to determine target file" << endl;
//             exit(1);
//         }
//         filein = tmp;
//     }

//     if ((argc - 1) % 2 != 0) {
//         cerr << "Error:  parameter error, require [SourceFileName (-ParaName FileName)*]" << endl;
//         exit(1);
//     }

//     // Work
//     Lexer lexer;
//     lexer.work(filein, fileout);

//     return 0;
// }
