#include "../include/my_header.h"
#include "../include/lexer.h"
#include "../include/cmdprocessor.h"
#include "../include/quad.h"
#include <string>

class Parser{
public:
    Parser(Lexer &lexer): lexer(lexer),  tempCount(0), labelCount(0){
        curtk=0;
        toptk=lexer.getToptk();
        nextToken();
        quads.clear();
        topType=0;
    }

    void work(){
        prog();
        if(curtk!=toptk+1){
            ceerError();
        }
    }

    void printIntermediateCode(string fileout) const{
        cerr<<std::left;
        ofstream fout(fileout);
        if(!fout.is_open()){
            cerr<<"Error: fail to open the file: "+fileout<<endl;
            exit(1);
        }
        cout<<"Target file open successfully"<<endl;
        fout<<std::left;

        for (const auto &quad : quads){
            quad.print(fout);
        }

        fout.close();
        cout<<"PrintQuad finished"<<endl;
    }

    TOKEN getCurToken(){
        return currentToken;
    }

    void ceerError() {
        cerr << std::left;
        cerr << "Error: parse error in: row:" << setw(4) << currentToken.idx 
            << " col:"   << setw(4) << currentToken.idy 
            << " word: " << currentToken.word << endl;
    }

private:
    Lexer &lexer;
    int curtk, toptk;
    TOKEN currentToken;
    int tempCount, labelCount;
    vector<QUAD> quads;

    int topType=0;// delare //,topName=0
    TOKEN delareType[10];
    // string delareName[100];

    int deBugOP=0;//deBug  1: open  0:close

    void nextToken(){
        if(curtk==toptk+1){
            ceerError();
            throw runtime_error("Expected need more token");
        } 
        currentToken=lexer.getToken(++curtk);
    }
    void preToken(){
        if(curtk==1){
            ceerError();
            throw runtime_error("Expected token before 0");
        } 
        currentToken=lexer.getToken(--curtk);
    }


    void prog(){
        while(currentToken.word!="{") nextToken();// need to do   # | Fuction | Struct |
        cell();
    }

    void cell(){
        if(currentToken.word=="{"){block();return;}
        if(currentToken.word==";"){nextToken();return;}
        statement();return;
    }

    void block(){
        dealBL();seqs();dealBR();
    }


    void seqs(){
        while(currentToken.word!="}"){//seq
            if(isDecl()){//decl
                decl();dealRR();
                continue;
            }
            else{statement();continue;}//statement
        }
    }



   

    void decl(){
        types();names();
        reSetDecl();
    }

    void types(){
        reSetDecl();
        checkDecl();
        while(isDecl()){//type
            delareType[++topType]=currentToken;
            nextToken();
        } 
    }
    void names(){
        name();
        while(currentToken.word==","){
            nextToken();
            name();
        }
    }

    void name(){
        checkID();
        string var=currentToken.word;
        delareNameWork(var);
        nextToken();
        if(currentToken.word=="="){
            preToken();
            expression();
        }
        
    }

    void delareNameWork(string varName){
        // type stk
        string var=newVar(varName);
    }



    void statement(){
        if(currentToken.word=="if"){
            ifStatement();return;
        }
        if(currentToken.word=="while"){
            whileStatement();return;
        }
        if(currentToken.word=="do"){
            doWhileStatement();return;
        } 
        if(currentToken.word=="for"){
            forStatement();return;
        }
        if(isExp()){
            assignment();return;
        }
        if(currentToken.word=="{"){
            block();return;
        }
        ceerError();
        throw runtime_error("Unexpected keyword");
    }

    void assignment(){
        expressions();dealRR();
    }

    void ifStatement(){
        dealIf();
        dealSL();
        string condResult=expression();
        dealSR();
        string labelElse=newLabel();
        string labelEnd=newLabel();
        quads.push_back(QUAD{"ifn", condResult, "goto", labelElse});//fail: jump to else
        cell();
        quads.push_back(QUAD{"goto", "", "", labelEnd});
        quads.push_back(QUAD{labelElse+":", "", "", ""});
        if(currentToken.word=="else"){
            nextToken();
            cell();
        }
        quads.push_back(QUAD{labelEnd+":", "", "", ""});
    }

    void whileStatement(){
        dealWhile();
        dealSL();
        string labelStart=newLabel();
        string labelEnd=newLabel();
        quads.push_back(QUAD{labelStart+":", "", "", ""});
        string condResult=expression();
        dealSR();   
        quads.push_back(QUAD{"ifn", condResult, "goto", labelEnd});//fail: jump to end
        cell();
        quads.push_back(QUAD{"goto", "", "", labelStart});
        quads.push_back(QUAD{labelEnd+":", "", "", ""});
    }

    void doWhileStatement(){
        dealDo();
        string labelStart=newLabel();
        string labelEnd=newLabel();
        quads.push_back(QUAD{labelStart+":", "", "", ""});
        cell();
        dealWhile();
        dealSL();
        string condResult=expression();
        dealSR();
        quads.push_back(QUAD{"if", condResult, "goto", labelStart});//succeed: jump to start
        quads.push_back(QUAD{labelEnd+":", "", "", ""});
        dealRR();
    }

    void forStatement(){
        dealFor();
        dealSL();
        if(isDecl()) decl();
        else if(isExp()) expressions();
        dealRR();
        string labelStart=newLabel();
        string labelEnd=newLabel();
        string labelUpdateStart=newLabel();
        string labelUpdateEnd=newLabel();
        quads.push_back(QUAD{labelStart+":", "", "", ""});
        string condResult=expression();
        dealRR();
        quads.push_back(QUAD{"if", condResult, "goto", labelUpdateEnd});//succeed: jump to updateend
        quads.push_back(QUAD{"ifn", condResult, "goto", labelEnd});//fail: jump to end
        quads.push_back(QUAD{labelUpdateStart+":", "", "", ""});
        if(isExp()) expressions();//update
        dealSR();
        quads.push_back(QUAD{"goto", "", "", labelStart});
        quads.push_back(QUAD{labelUpdateEnd+":", "", "", ""});
        cell();
        
        quads.push_back(QUAD{"goto", "", "", labelUpdateStart});
        quads.push_back(QUAD{labelEnd+":", "", "", ""});
    }



    // string opUnary(){

    // }
    // string opBinary(){

    // }
    // string opTernary(){

    // }
    // string opSwitch(){// notice 	? :	三元条件操作符	int i = (a > b) ? a : b;	从右到左
        
    // }
    string expLR(int lv){//L->R
        string left=expSwitch(lv-1);
        
        /////////////////
        // if(lexer.checkOPLevel(currentToken,lv)){
        //     cout<<"LV: "<<lv<<" "<<currentToken.word<<endl;
        // }
        // /////////////////

        while(lexer.checkOPLevel(currentToken,lv)){
            string op=currentToken.word;
            nextToken();
            string right="";
            if(lv!=1)  expSwitch(lv-1);
            string temp=newTemp();
            quads.push_back(QUAD{op, left, right, temp});//
            left=temp;
        }
        return left;
    }
        
    string expRL(int lv){//R->L
        string left="";
        if(lv!=2||!lexer.checkOPLevel(currentToken,lv)){
            left=expSwitch(lv-1);
        }
        deBug(1,lv);
        if(!lexer.checkOPLevel(currentToken,lv)) return left;
        string op=currentToken.word;
        nextToken();
        string right=expRL(lv);
        string temp=newTemp();
        quads.push_back(QUAD{op, left, right, temp});//
        return temp;
    }
    string expQuestion(int lv){//? R->L
        string left=expSwitch(lv-1);
        deBug(1,lv);
        if(!lexer.checkOPLevel(currentToken,lv)) return left;
        string op=currentToken.word;
        nextToken();
        string right1=expRL(lv);
        
        dealColon();
        string right2=expRL(lv);
        string temp=newTemp();
        string temp2=newTemp();
        string labelElse=newLabel();
        string labelEnd=newLabel();
        quads.push_back(QUAD{"ifn", left, "goto", labelElse});//fail: jump to else
            quads.push_back(QUAD{"=",temp2,right1,temp});
        quads.push_back(QUAD{"goto", "", "", labelEnd});
        quads.push_back(QUAD{labelElse+":", "", "", ""});
             quads.push_back(QUAD{"=",temp2,right2,temp});
        quads.push_back(QUAD{labelEnd+":", "", "", ""});
        return temp;
    }
    string expSwitch(int lv)// lv: 15-->0
    {  
        if(lv==0) return factor();
        if(lv==14)return expQuestion(lv);
        if(lexer.getOPLR(lv)==1) return expLR(lv);
        if(lexer.getOPLR(lv)==2) return expRL(lv);
        return "";
    }

    string expression(){
        return expSwitch(15);
    }

    void expressions(){
        expression();
        while(currentToken.word==","){
            nextToken();
            expression();
        }
    }

    string factor(){
        string result;
        if(currentToken.type=="Identifier"||currentToken.type=="Number"||currentToken.type=="String"){
            deBug(2);
            result=addSign(currentToken.word,currentToken.type);
            nextToken();
            return result;
        } 
        if(currentToken.word=="("){
            dealSL();
            result=expression();
            dealSR();
            return result;
        }
        ceerError();
        throw runtime_error("Unexpected token: "+currentToken.word);
        return result;
    }

    string addSign(string tmp,string type){
        if(type=="Var")         return "V"+tmp;
        if(type=="Lable")       return "L"+tmp;
        if(type=="Temp")        return "T"+tmp;
        if(type=="Identifier") return "V"+tmp;
        if(type=="Number")      return "N"+tmp;
        if(type=="String")      return "S"+tmp;
        ceerError();
        throw runtime_error("error lable: "+type);
        return "";
    }

    string newVar(string varName){
        string idx=addSign(varName,"Var");
        quads.push_back(QUAD{idx+":", "", "", ""});
        return idx;
    }

    string newTemp(){
        string idx=addSign(to_string(tempCount++),"Temp");
        quads.push_back(QUAD{idx+":", "", "", ""});
        return idx;
    }

    string newLabel(){
        string idx=addSign(to_string(labelCount++),"Lable");
        return idx;
    }



    void deBug(int op,int lv=0){
        if(!deBugOP) return;
        if(op==1){
            if(lexer.checkOPLevel(currentToken,lv)) { 
            cout<<"LV: "<<lv<<" "<<currentToken.word<<endl;
        }
        }
        if(op==2){
            cout<<currentToken.word<<" "<<currentToken.type<<endl;
        }
    }


    bool dealBL(){//deal {
        if(currentToken.word=="{"){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected '{'");
        return 0;
    }
    bool dealBR(){//deal }
        if(currentToken.word=="}"){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected '}'");
        return 0;
    }
    bool dealSL(){//deal (
        if(currentToken.word=="("){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected '('");
        return 0;
    }
    bool dealSR(){//deal )
        if(currentToken.word==")"){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected )'");
        return 0;
    }
    bool dealRR(){//deal ;
        if(currentToken.word==";"){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected ';'");
        return 0;
    }
    bool dealColon(){//deal :
        if(currentToken.word==":"){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected ':'");
        return 0;
    } 
    bool dealIf(){//deal if
        if(currentToken.word=="if"){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected 'if'");
        return 0;
    }
    bool dealWhile(){//deal while
        if(currentToken.word=="while"){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected 'while'");
        return 0;
    }
    bool dealDo(){//deal do
        if(currentToken.word=="do"){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected 'do'");
        return 0;
    }
    bool dealFor(){//deal for
        if(currentToken.word=="for"){
            nextToken();
            return 1;
        }
        ceerError();
        throw runtime_error("Expected 'for'");
        return 0;
    }
    bool checkID(){//check Identifier
        if(currentToken.type=="Identifier") return 1;
        else{
            ceerError();
            throw runtime_error("Expected identifier");
            return 0;
        }
    }

    
    void reSetDecl(){topType=0;}//topName=0;
    bool isDecl(){
        if(lexer.isStorageClassSpecifiers(currentToken)||
           lexer.isTypeSpecifiers(currentToken)||
           lexer.isTypeQualifiers(currentToken)||
           lexer.isClassesAndObjects(currentToken)
        ) return 1;
        return 0;
    }
    bool checkDecl(){
        if(isDecl())return 1;
        ceerError();
        throw runtime_error("Expected Decl");
        return 0;
    }

    bool isExp(){
        if(
            currentToken.word!=","&&
            currentToken.word!=";"&&//!!
            currentToken.word!=")"&&//!!
            currentToken.word!="{"&&//!!
            currentToken.word!="}"&&//!!
            currentToken.word!="#"
        ) return 1;
        return 0;
    }
    
};

int main(int argc, char *argv[]){
    try{
        CMDProcessor cmdp(argc, argv);
        Lexer lexer;
        lexer.work(cmdp.getFileIn());
        // lexer.printTokens(cmdp.getFile()+".lex");
        Parser parser(lexer);
        parser.work();
        parser.printIntermediateCode(cmdp.getFile()+".quad");
    } catch (const runtime_error &e){
        cerr<<e.what()<<endl;
        return 1;
    } catch (const exception &e){
        cerr<<"Error: "<<e.what()<<endl;
        return 1;
    }
    return 0;
}
