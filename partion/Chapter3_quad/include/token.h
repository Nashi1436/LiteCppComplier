#ifndef TOKENS
#define TOKENS

#include "my_header.h"

struct TOKEN {
    string type;  // token type
    long long sym;     // hash and token kind
    string word;  // source code token word
    int idx, idy;      // Source code rows and columns
};

#endif // TOKEN