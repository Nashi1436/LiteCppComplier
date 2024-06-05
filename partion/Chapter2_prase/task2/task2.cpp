#include "../include/my_header.h"
#include "../include/lexer.h"
#include "../include/cmdprocessor.h"

// #include "my_header.h"
// #include "lexer.h"

class Node{
public:
    string value;
    vector<shared_ptr<Node>> children;

    Node(const string& val):value(val){}

    void addChild(shared_ptr<Node> child){
        children.push_back(child);
    }

    void printTrace(ofstream& fout,int level=0)const{
        for(int i=0;i<level;++i)fout<<"\t";
        fout<<value<<"\n";
        for(const auto& child:children){
            child->printTrace(fout,level+1);
        }
    }
};

class Parser{
public:
    Parser(Lexer& lexer):lexer(lexer){
        curtk=0;toptk=lexer.getToptk();
        nextToken();
    }
    shared_ptr<Node> work(){
        rt=block();
        if(curtk!=toptk+1){//error  random char  
            ceerError();
        }
        return rt;
    }
    void printNodesTree(string fileout)const{
        cerr<<std::left;
        ofstream fout(fileout);
        if(!fout.is_open()){
            cerr<<"Error:  fail to open the file: "+fileout<<endl;exit(1);
        }
        cout<<"Target file open successfully"<<endl;
        fout<<std::left;
        rt->printTrace(fout);
        fout.close();
        cout<<"PrintTokens finished"<<endl;
    }
    TOKEN getCurToken(){
        return currentToken;
    }

    void ceerError(){
        cerr<<std::left;
        cerr<<"Error:  parse error in: row:"<<currentToken.idx<<" col:"<<currentToken.idy<<" word:"<<currentToken.word<<endl;
        // exit(1);
    }

private:
    Lexer &lexer;
    int curtk,toptk;
    TOKEN currentToken;
    shared_ptr<Node> rt;

    void nextToken(){
        if(curtk==toptk+1) ceerError();
        currentToken=lexer.getToken(++curtk);
    }

    shared_ptr<Node> block(){
        auto node=make_shared<Node>("<Block>");
        if(currentToken.word=="{"){
            nextToken();
            node->addChild(decls());
            node->addChild(statements());
            if(currentToken.word=="}"){
                nextToken();
            }else{
                ceerError();
                throw runtime_error("Expected '}'");
            }
        }else{
            ceerError();
            throw runtime_error("Expected '{'");
        }
        return node;
    }

    shared_ptr<Node> decls(){
        auto node=make_shared<Node>("<Decls>");
        while(currentToken.word=="int"){
            node->addChild(decl());
        }
        node->addChild(make_shared<Node>("empty"));
        return node;
    }

    shared_ptr<Node> decl(){
        auto node=make_shared<Node>("<Decl>");
        node->addChild(make_shared<Node>(currentToken.word));
        nextToken();
        node->addChild(nameList());
        if(currentToken.word==";"){
            nextToken();
        }else{
            ceerError();
            throw runtime_error("Expected ';'");
        }
        return node;
    }

    shared_ptr<Node> nameList(){
        auto node=make_shared<Node>("<NameList>");
        node->addChild(name());
        while(currentToken.word==","){
            nextToken();
            node->addChild(name());
        }
        return node;
    }

    shared_ptr<Node> name(){
        if(currentToken.type=="Identifiers"){
            auto node=make_shared<Node>(currentToken.word);
            nextToken();
            return node;
        }else{
            ceerError();
            throw runtime_error("Expected identifier");
        }
        return nullptr;
    }

    shared_ptr<Node> statements(){
        auto node=make_shared<Node>("<Statements>");
        while(currentToken.type=="Identifiers"||currentToken.word=="if"||currentToken.word=="while"||currentToken.word=="do"||currentToken.word=="for"){
            node->addChild(statement());
        }
        node->addChild(make_shared<Node>("empty"));
        return node;
    }

    shared_ptr<Node> statement(){
        if(currentToken.type=="Identifiers"){
            return assignment();
        }else if(currentToken.word=="if"){
            return ifStatement();
        }else if(currentToken.word=="while"){
            return whileStatement();
        }else{
            ceerError();
            throw runtime_error("Unexpected keyword");
        }
    }

    shared_ptr<Node> assignment(){// need to add for a=b=1;
        auto node=make_shared<Node>("<Assignment>");
        node->addChild(make_shared<Node>(currentToken.word));
        nextToken();
        if(currentToken.word=="="){
            nextToken();
            node->addChild(expr());
            if(currentToken.word==";"){
                nextToken();
            }else{
                ceerError();
                throw runtime_error("Expected ';'");
            }
        }else{
            ceerError();
            throw runtime_error("Expected '='");
        }
        return node;
    }

    shared_ptr<Node> ifStatement(){
        auto node=make_shared<Node>("<IfStatement>");
        nextToken();
        if(currentToken.word=="("){
            nextToken();
            node->addChild(rel());
            if(currentToken.word==")"){
                nextToken();
                if(currentToken.word=="{"){
                    nextToken();
                    node->addChild(statements());
                    if(currentToken.word=="}"){
                        nextToken();
                    }else{
                        ceerError();
                        throw runtime_error("Expected '}'");
                    }
                }else{
                    ceerError();
                    throw runtime_error("Expected '{'");
                }
                if(currentToken.word=="else"){
                    nextToken();
                    if(currentToken.word=="{"){
                        nextToken();
                        node->addChild(statements());
                        if(currentToken.word=="}"){
                            nextToken();
                        }else{
                            ceerError();
                            throw runtime_error("Expected '}'");
                        }
                    }else{
                        ceerError();
                        throw runtime_error("Expected '{'");
                    }
                }
            }else{
                ceerError();
                throw runtime_error("Expected ')'");
            }
        }else{
            ceerError();
            throw runtime_error("Expected '('");
        }
        return node;
    }

    shared_ptr<Node> whileStatement(){
        auto node=make_shared<Node>("<WhileStatement>");
        nextToken();
        if(currentToken.word=="("){
            nextToken();
            node->addChild(rel());
            if(currentToken.word==")"){
                nextToken();
                if(currentToken.word=="{"){
                    nextToken();
                    node->addChild(statements());
                    if(currentToken.word=="}"){
                            nextToken();
                    }else{
                        ceerError();
                        throw runtime_error("Expected '}'");
                    }
                }else{
                    ceerError();
                    throw runtime_error("Expected '{'");
                }
            }else{
                ceerError();
                throw runtime_error("Expected ')'");
            }
        }else{
            ceerError();
            throw runtime_error("Expected '('");
        }
        return node;
    }

    shared_ptr<Node> expr(){
        auto node=make_shared<Node>("<Expr>");
        node->addChild(term());
        return expr1(node); // 修改成左结合
    }

    shared_ptr<Node> expr1(shared_ptr<Node> node){
        if(currentToken.word=="+"||currentToken.word=="-"){
            auto newNode=make_shared<Node>("<Expr1>");
            newNode->addChild(node);
            newNode->addChild(make_shared<Node>(currentToken.word));
            nextToken();
            newNode->addChild(term());
            return expr1(newNode);
        }
        return node;
    }

    shared_ptr<Node> term(){
        auto node=make_shared<Node>("<Term>");
        node->addChild(factor());
        return term1(node); // 修改成左结合
    }

    shared_ptr<Node> term1(shared_ptr<Node> node){
        if(currentToken.word=="*"||currentToken.word=="/"){
            auto newNode=make_shared<Node>("<Term1>");
            newNode->addChild(node);
            newNode->addChild(make_shared<Node>(currentToken.word));
            nextToken();
            newNode->addChild(factor());
            return term1(newNode);
        }
        return node;
    }

    shared_ptr<Node> factor(){
        auto node=make_shared<Node>("<Factor>");
        if(currentToken.type=="Identifiers"||currentToken.type=="Numbers"){
            node->addChild(make_shared<Node>(currentToken.word));
            nextToken();
        }else if(currentToken.word=="("){
            nextToken();
            node->addChild(expr());
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
        return node;
    }

    shared_ptr<Node> rel(){
        auto node=make_shared<Node>("<Rel>");
        node->addChild(expr());
        if(currentToken.word=="<"||currentToken.word==">"||currentToken.word=="<="||currentToken.word==">="||currentToken.word=="=="||currentToken.word=="!="){
            node->addChild(make_shared<Node>(currentToken.word));
            nextToken();
            node->addChild(expr());
        }else{
            ceerError();
            throw runtime_error("Expected relational operator");
        }
        return node;
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
        parser.printNodesTree(cmdp.getFile()+".parse");
    }catch(const runtime_error& e){
        cerr<<e.what()<<endl;
        return 1;
    }catch(const exception& e){
        cerr<<"Error: "<<e.what()<<endl;
        return 1;
    }
    return 0;
}
