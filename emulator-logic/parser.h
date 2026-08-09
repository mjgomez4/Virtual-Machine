#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>


using namespace std;

class parser
{
    public:
        parser(string rawVMInstruction);
        string removeComments(string raw_instruction);
        string getCommandType();
        string arg1();
        int arg2();
    private:
        string cleanVMCommand;
};

#endif