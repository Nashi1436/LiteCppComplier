#include "../include/my_header.h"
#include "../include/lexer.h"
#include "../include/cmdprocessor.h"
#include "../include/quad.h"


// #include "my_header.h"
// #include "lexer.h"

class Parser{
public:
    Parser(Lexer& lexer):lexer(lexer),tempCount(0){
        curtk=0;toptk=lexer.getToptk();
        nextToken();
    }
    void work(){
        expr();
        if(curtk!=toptk+1){
            ceerError();
        }
    }
    void printIntermediateCode(string fileout)const{
        cerr<<std::left;
        ofstream fout(fileout);
        if(!fout.is_open()){
            cerr<<"Error:  fail to open the file: "+fileout<<endl;exit(1);
        }
        cout<<"Target file open successfully"<<endl;
        fout<<std::left;

        for(const auto& quad:quads){
            quad.print(fout);
        }
        
        fout.close();
        cout<<"PrintQuad finished"<<endl;
        
    }
    TOKEN getCurToken(){
        return currentToken;
    }

    void ceerError(){
        cerr<<std::left;
        cerr<<"Error:  parse error in: row:"<<currentToken.idx<<" col:"<<currentToken.idy<<" word:"<<currentToken.word<<endl;
    }

private:
    Lexer &lexer;
    int curtk,toptk;
    TOKEN currentToken;
    int tempCount;
    vector<QUAD> quads;

    void nextToken(){
        if(curtk==toptk+1) ceerError();
        currentToken=lexer.getToken(++curtk);
    }


    string expr(){
        string left=term();
        while(currentToken.word=="+"||currentToken.word=="-"){
            string op=currentToken.word;
            nextToken();
            string right=term();
            string temp=newTemp();
            quads.push_back(QUAD{op, left, right, temp});
            left=temp;
        }
        return left;
    }

    string term(){
        string left=factor();
        while(currentToken.word=="*"||currentToken.word=="/"){
            string op=currentToken.word;
            nextToken();
            string right=factor();
            string temp=newTemp();
            quads.push_back(QUAD{op, left, right, temp});
            left=temp;
        }
        return left;
    }

    string factor(){
        string result;
        if(currentToken.type=="Identifiers"||currentToken.type=="Numbers"){
            result=currentToken.word;
            nextToken();
        }else if(currentToken.word=="("){
            nextToken();
            result=expr();
            if(currentToken.word==")"){
                nextToken();
            }else{
                ceerError();
                throw runtime_error("Mismatched parenthesis");
            }
        }else{
            ceerError();
            throw runtime_error("Unexpected token: "+currentToken.word);
        }
        return result;
    }

    string newTemp(){
        return "t"+to_string(tempCount++);
    }
};

int main(int argc, char *argv[]){
    try{
        CMDProcessor cmdp(argc,argv);
        Lexer lexer;
        lexer.work(cmdp.getFileIn());
        // lexer.printTokens(cmdp.getFile()+".lex");
        Parser parser(lexer);
        parser.work();
        parser.printIntermediateCode(cmdp.getFile()+".quad");
    }catch(const runtime_error& e){
        cerr<<e.what()<<endl;
        return 1;
    }catch(const exception& e){
        cerr<<"Error: "<<e.what()<<endl;
        return 1;
    }
    return 0;
}
