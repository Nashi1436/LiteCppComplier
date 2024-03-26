#include<bits/stdc++.h>
using namespace std;

#define dout fout
#define ll long long
#define pb push_back
#define rep(i,st,ed) for(int i=st,edi=ed;i<=edi;i++)
int argflag[22],usedarg;

const int MX_TK=1e6+10,MX_OP=3;
const int DI_SZ=50,bsop=100,bsdeli=200,bshaku=300,bscg=400;
const int bsnum=501,bsid=601;

//table
string keywords[DI_SZ] = { //1-100
    "auto", "short", "int", "long", "float", "double", "char", "struct",
    "union", "enum", "typedef", "const", "unsigned", "signed", "extern",
    "register", "static", "volatile", "void", "if", "else", "switch",
    "case", "for", "do", "while", "goto", "continue", "break", "default",
    "sizeof", "return", "main"
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
int topkey,topop=bsop,topdeli=bsdeli,tophaku=bshaku,topcg=bscg;
unordered_map<string,ll>mp;
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

//token
int toptk;
struct TOKEN{
    string type;
    ll sym;
    string word;
}token[MX_TK];

void getTokens(const string& input,ofstream& fout) {
    buildmap();
    string word;int len=input.length();
    rep(i,0,len-1){
        int c=input[i],mpc=0,nxtc=(i+1<len)?input[i+1]:0,tmpi=i;word.clear();
        if(mp.find(string(1,c))!=mp.end()) mpc=mp[string(1,c)];

        //get next token
        if(c=='\"'||c=='\''){//Strings
            word.pb(c);
            while(++i<len&&input[i]!=c) word.pb(input[i]);
            word.pb(c);
            if(i==len){perror("Error: Unclosed string literal");exit(1);}
            token[++toptk]=TOKEN{"Strings",mpc,word};
            continue;
        }
        if(isdigit(c)/*||(i+1<len&&(c=='-'||c=='+')&&isisdigit(nxtc))*/){//Numbers
            word.pb(c);
            while(++i<len&&(isValidChar(input[i])||input[i]=='.')) word.pb(input[i]);
            i--;
            token[++toptk]=TOKEN{"Numbers",bsnum,word};
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
                 token[++toptk]=TOKEN{"Keywords",mp[word],word};
            else token[++toptk]=TOKEN{"Identifiers",bsid,word};//Identifiers
            continue;
        }
        if(bsop<mpc&&mpc<=bshaku){//Operators or delimiters
            int oplen=MX_OP+1,flag=0;
            while(--oplen&&!flag) if(mp.find(word=input.substr(i,oplen))!=mp.end()){//find from long
                int mpv=mp[word];
                if(!(bsop<mpv&&mpv<=bshaku)) continue;
                flag=1;
                token[++toptk]=TOKEN{(mpv<=bsdeli)?("Operators"):("Delimiters"),mpv,word};
                i+=oplen-1;
            }  
            if(!flag){perror("Error: Operators or delimiters unknown");exit(1);}
            continue;
        }
    }

}


int main(int argc, char *argv[]) {

    string filein="p3.in",fileout="p3.out";

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
    getTokens(code,fout);
    dout << std::left;
    rep(i,1,toptk) {
        dout<<"("<<setw(3)<<token[i].sym<<" , "<<setw(25)<<token[i].word<<")"<<endl;
    }


    //end
    fout.close();
    cout<<"Process finished"<<endl;
    return 0;

}