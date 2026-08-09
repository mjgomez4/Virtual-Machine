#include "parser.h"

using namespace std;

parser::parser(string rawVMInstruction) {
    cleanVMCommand = removeComments(rawVMInstruction);
}

string parser::removeComments(string raw_instruction) {
    //string clean_command;
    if (raw_instruction.empty()) {
        return raw_instruction;
    }

    // remove comments
    for (unsigned int i = 0; i < raw_instruction.length()-1; i++) {
        if (raw_instruction[i] == '/' && raw_instruction[i+1] == '/') {
            // remove every character that's part of the comment
            raw_instruction.erase(i);
            break;
        }
    }

    // strip trailing \r (Windows line endings) and any other trailing whitespace
    while (!raw_instruction.empty() &&
           (raw_instruction.back() == '\r' || raw_instruction.back() == ' ' || raw_instruction.back() == '\t')) {
        raw_instruction.pop_back();
    }

    // remove leading tabs
    if (!raw_instruction.empty() && raw_instruction.front() == '\t') {
        raw_instruction.erase(0, 1);
    }
    
    return raw_instruction;
}



string parser::getCommandType() {  

    
    if (cleanVMCommand.find("push") < cleanVMCommand.size()) {
        return "C_PUSH";
    } else if(cleanVMCommand.find("pop") < cleanVMCommand.size()) {
        return "C_POP";
    } else if (cleanVMCommand.find("label") < cleanVMCommand.size()) {
        return "C_LABEL";
    } else if (cleanVMCommand.find("if-goto") < cleanVMCommand.size()) {
        return "C_IF";
    } else if (cleanVMCommand.find("call") < cleanVMCommand.size()) {
        return "C_CALL";
    } else if (cleanVMCommand.find("function") < cleanVMCommand.size()) {
        return "C_FUNCTION";
    } else if (cleanVMCommand.find("return") < cleanVMCommand.size()) {
        return "C_RETURN";
    } else if (cleanVMCommand.find("goto") < cleanVMCommand.size()) {
        return "C_GOTO";
    } else if (cleanVMCommand.find("add") < cleanVMCommand.size()) {
        return "C_ARITHMETIC";
    } else if (cleanVMCommand.find("sub") < cleanVMCommand.size()) {
        return "C_ARITHMETIC";
    } else if (cleanVMCommand.find("not") < cleanVMCommand.size()) {
        return "C_ARITHMETIC";
    } else if (cleanVMCommand.find("or") < cleanVMCommand.size()) {
        return "C_ARITHMETIC";
    } else if (cleanVMCommand.find("and") < cleanVMCommand.size()) {
        return "C_ARITHMETIC";
    } else if (cleanVMCommand.find("neg") < cleanVMCommand.size()) {
        return "C_ARITHMETIC";
    } else if (cleanVMCommand.find("lt") < cleanVMCommand.size()) {
        return "C_ARITHMETIC";
    } else if (cleanVMCommand.find("gt") < cleanVMCommand.size()) {
        return "C_ARITHMETIC";
    } else if (cleanVMCommand.find("eq") < cleanVMCommand.size()) {
        return "C_ARITHMETIC";
    } else {
        return "";
    }
}


string parser::arg1() {
    if (getCommandType() == "C_ARITHMETIC") {
        return cleanVMCommand;
    } 

    size_t firstSpaceIndex = cleanVMCommand.find(' ');
    // start searching for the secondSpaceIndex starting immediately after the firstSpaceIndex
    size_t secondSpaceIndex = cleanVMCommand.find(' ', firstSpaceIndex + 1);

    // the first argument will always include everything after the firstSpaceIndex and everything before the secondSpaceIndex
    return cleanVMCommand.substr(firstSpaceIndex + 1, secondSpaceIndex - firstSpaceIndex - 1);
}

int parser::arg2() {
    size_t firstSpaceIndex = cleanVMCommand.find(' ');
    size_t secondSpaceIndex = cleanVMCommand.find(' ', firstSpaceIndex + 1);

    if (secondSpaceIndex == string::npos) {
        return 0;
    }

    string arg2string = cleanVMCommand.substr(secondSpaceIndex + 1);
    return arg2string.empty() ? 0 : stoi(arg2string);
    
}