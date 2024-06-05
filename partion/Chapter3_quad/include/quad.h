#ifndef QUADS
#define QUADS

#include "my_header.h"

struct QUAD {
    string op;
    string arg1;
    string arg2;
    string result;
    //op, left, right, temp

    QUAD(const string& op, const string& arg1, const string& arg2, const string& result)
        : op(op), arg1(arg1), arg2(arg2), result(result) {}

    void print(ofstream& fout) const {
       fout << "(" << op << ", " << arg1 << ", " << arg2 << ", " << result << ")" << endl;
    }

};

#endif // QUADS