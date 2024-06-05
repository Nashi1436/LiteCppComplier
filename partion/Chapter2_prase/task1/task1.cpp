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
        rt=expr();
        if(curtk!=toptk+1){//error  random char  
            cerr<<std::left;
            cerr<<"Error:  parse error in: row:"<<currentToken.idx<<" col:"<<currentToken.idy<<endl;exit(1);
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

private:
    Lexer& lexer;
    int curtk, toptk;
    TOKEN currentToken;
    shared_ptr<Node> rt;

    void nextToken(){
        if(curtk==toptk+1){
            cerr<<std::left;
            cerr<<"Error:  parse error in: row:"<<currentToken.idx<<" col:"<<currentToken.idy<<endl;exit(1);
        }
        currentToken=lexer.getToken(++curtk);
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
        if(isalpha(currentToken.word[0])||isdigit(currentToken.word[0])){
            node->addChild(make_shared<Node>(currentToken.word));
            nextToken();
        }else if(currentToken.word=="("){
            nextToken();
            node->addChild(expr());
            if(currentToken.word==")"){
                nextToken();
            }else{
                throw runtime_error("Mismatched parenthesis");
            }
        }else{
            throw runtime_error("Unexpected token: "+currentToken.word);
        }
        return node;
    }
};

int main(int argc, char* argv[]){
    CMDProcessor cmdp(argc,argv);
    Lexer lexer;
    lexer.work(cmdp.getFileIn());
    // lexer.printTokens(cmdp.getFile()+".lex");
    Parser parser(lexer);
    parser.work();
    parser.printNodesTree(cmdp.getFile()+".parse");

    return 0;
}
