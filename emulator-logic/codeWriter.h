#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std; 

class codeWriter {
public:
    codeWriter(string outputFilePath);
    ~codeWriter();

    void setFileName(string vmFileName);  // for static segment naming, e.g. Foo.3
    void writeArithmetic(string command);              // add, sub, eq, gt, lt, and, or, not, neg

    string segmentSymbol(string segment);
    void writePushPop(string command, string segment, int index);  // C_PUSH / C_POP

    void close();

    // project 8 functions
    void writeInit();
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);

    void writeFunction(string functionName, int nVars);
    void writeCall(string functionName, int nArgs);

    // writeReturn helper
    void frameManipulation(string offset, string destination);
    void writeReturn();
    // push and pop helpers
    void basicPushTail();
    void basicPop();

private:
    ofstream outFile;
    string currentFileName;
    int labelCounter;  // for generating unique labels in eq/gt/lt comparisons

    // project 8 member variables
    string currentFunction;
    int callCount;
};

#endif