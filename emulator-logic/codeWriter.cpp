#include "codeWriter.h"

using namespace std;

codeWriter::codeWriter(string outputFilePath) {
    outFile.open(outputFilePath);
    labelCounter = 0;
 
    if (!outFile.is_open()) {
        cerr << "Error: could not open output file " << outputFilePath << endl;
    }
}
 
codeWriter::~codeWriter() {
    close();
}
 
void codeWriter::close() {
    if (outFile.is_open()) {
        outFile.close();
    }
}

// bootstrapping code
void codeWriter::writeInit() {
    outFile << "@256\n";
    outFile << "D=A\n";
    outFile << "@SP\n";
    outFile << "M=D\n";

    writeCall("Sys.init", 0);

}

void codeWriter::setFileName(string vmFileName) {
    // Expect something like "Foo.vm" or "path/to/Foo.vm" -> store just "Foo"
    size_t lastSlash = vmFileName.find_last_of("/\\");
    string base = (lastSlash == string::npos) ? vmFileName : vmFileName.substr(lastSlash + 1);
 
    size_t dotPos = base.find_last_of('.');
    currentFileName = (dotPos == string::npos) ? base : base.substr(0, dotPos);
}

void codeWriter::writeArithmetic(string command) {
    if (command == "add" || command == "sub" || command == "and" || command == "or") {
        // Pop y into D, then point A at x's stack slot (without popping x yet)
        outFile << "@SP\n";
        outFile << "M=M-1\n";
        // A = 257
        outFile << "A=M\n";
        // D = RAM[257] or y
        outFile << "D=M\n";
        outFile << "@SP\n";
        outFile << "M=M-1\n";
        // A = 256
        outFile << "A=M\n";
    
        if (command == "add") {
            // RAM[256] = x + y
            outFile << "M=D+M\n";
        } else if (command == "sub") {
            outFile << "M=M-D\n";
        } else if (command == "and") {
            outFile << "M=M&D\n";
        } else {
            outFile << "M=M|D\n";
        }

        // updating the stack pointer one past the top stack value (i.e. 257)
        outFile << "@SP\n";
        outFile << "M=M+1\n";
    } else if (command == "neg" || command == "not") {
        outFile << "@SP\n";
        outFile << "M=M-1\n";
        // A=257
        outFile << "A=M\n";
        
        if (command == "neg") {
            // RAM[257] = -RAM[257]
            outFile << "M=-M\n";
        } else {
            outFile << "M=!M\n";
        }

        outFile << "@SP\n";
        outFile << "M=M+1\n";
        
    } else if (command == "eq" || command == "gt" || command == "lt") {
        // the reason we have "TRUE_0, TRUE_1, END_0, END_1" is to differentiate between different 
        // eq, gt, and lt command instances
        string trueLabel = "TRUE_" + to_string(labelCounter);
        string endLabel = "END_" + to_string(labelCounter);
        labelCounter++;

        outFile << "@SP\n";
        outFile << "M=M-1\n";
        // A = 257
        outFile << "A=M\n";
        // D = RAM[257] or y
        outFile << "D=M\n";
        outFile << "@SP\n";
        outFile << "M=M-1\n";
        // A = 256
        outFile << "A=M\n";
        outFile << "D=M-D\n"; // D = x - y

        outFile << "@" << trueLabel << "\n";
        if (command == "eq") {
            outFile << "D;JEQ\n";
        } else if (command == "gt") {
            outFile << "D;JGT\n";
        } else {
            outFile << "D;JLT\n";
        }

        // handle the false case (push 0 onto the stack)
        outFile << "@SP\n";
        outFile << "A=M\n";
        outFile << "M=0\n";
        outFile << "@" << endLabel << "\n";
        outFile << "0;JMP\n";

        // handle the true case: push -1 onto the stack (all 1 bits, Hack's representation of boolean true)
        outFile << "(" << trueLabel << ")\n";
        outFile << "@SP\n";
        outFile << "A=M\n";
        outFile << "M=-1\n";

        // both paths resolve to endLabel
        outFile << "(" << endLabel << ")\n";
        outFile << "@SP\n";
        outFile << "M=M+1\n";


    } else {
        cerr << "Warning: unrecognized arithmetic command '" << command << "'" << endl;
    }

}

string codeWriter::segmentSymbol(string segment) {
    if (segment == "local") {
        return "LCL";
    } else if (segment == "argument") {
        return "ARG";
    } else if (segment == "this") {
        return "THIS";
    } else if (segment == "that") {
        return "THAT";
    } else {
        return ""; // temp/pointer/static/constant are handled separately
    }
}

void codeWriter::basicPushTail() {
    // tail (push the D value onto the stack)
    outFile << "@SP\n";
    // A = RAM[0]
    outFile << "A=M\n";
    outFile << "M=D\n";
    outFile << "@SP\n";
    // update the stack pointer
    outFile << "M=M+1\n";
}

void codeWriter::basicPop() {
    // regress the stack pointer
    outFile << "@SP\n";
    outFile << "M=M-1\n";
    // RAM[300]
    outFile << "A=M\n";
    outFile << "D=M\n";
}

void codeWriter::writePushPop(string command, string segment, int index) {
    if (command == "C_PUSH") {
        if (segment == "constant") {
            outFile << "@" << index << "\n";
            outFile << "D=A\n";
        } else if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
            outFile << "@" << segmentSymbol(segment) << "\n";
            outFile << "D=M\n";
            outFile << "@" << index << "\n";
            // (base + i)
            outFile << "A=D+A\n";
            // RAM[base + i]
            outFile << "D=M\n";
        } else if (segment == "temp") {
            outFile << "@" << (5 + index) << "\n";
            outFile << "D=M\n";
        } else if (segment == "pointer") {
            outFile << "@" << (index == 0 ? "THIS" : "THAT") << "\n";
            outFile << "D=M\n";
        } else if (segment == "static") {
            outFile << "@" << currentFileName << "." << index << "\n";
            outFile << "D=M\n";
        } else {
            cerr << "Warning: unrecognized push segment '" << segment << "'" << endl;
            return;
        }

        basicPushTail();

    } else if (command == "C_POP") {
        if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
            // retrieve the destination address to store the popped value
            outFile << "@" << segmentSymbol(segment) << "\n";
            outFile << "D=M\n";
            outFile << "@" << index << "\n";
            // (base + i)
            outFile << "D=D+A\n";
            outFile << "@R13\n";
            // RAM[base + i]
            outFile << "M=D\n";

            basicPop();

            outFile << "@R13\n";
            outFile << "A=M\n";
            outFile << "M=D\n";

        } else if (segment == "temp") {
            basicPop();
            outFile << "@" << (5 + index) << "\n";
            outFile << "M=D\n";
        } else if (segment == "pointer") {
            basicPop();
            outFile << "@" << (index == 0 ? "THIS" : "THAT") << "\n";
            outFile << "M=D\n";
        } else if (segment == "static") {
            basicPop();
            outFile << "@" << currentFileName << "." << index << "\n";
            outFile << "M=D\n";
        } else {
            cerr << "Warning: unrecognized pop segment '" << segment << "'" << endl;
        }
    }
    else {
        cerr << "Warning: writePushPop called with non-push/pop command type" << endl;
    }

    
}

void codeWriter::writeLabel(string label) {
    outFile << "(" << currentFunction << "$" << label << ")\n";
}

void codeWriter::writeGoto(string label) {
    outFile << "@" << currentFunction << "$" << label << "\n";
    outFile << "0;JMP\n";
}

void codeWriter::writeIf(string label) {
    basicPop();
    outFile << "@" << currentFunction << "$" << label << "\n";
    outFile << "D;JNE\n";
}

void codeWriter::writeFunction(string functionName, int nVars) {
    currentFunction = functionName;
    // unlike writeLabel, functions don't need "$" notation because they aren't nested inside anything else that could cause a naming collision 
    // (the class they're part of differentiates them; e.g. "Main.foo" vs. "parser.foo")
    outFile << "(" << currentFunction << ")\n"; 

    for (int i = 0; i < nVars; i++) {
        outFile << "@SP\n";
        outFile << "A=M\n";
        outFile << "M=0\n";
        outFile << "@SP\n";
        // update stack pointer
        outFile << "M=M+1\n";
    }
}

void codeWriter::writeCall(string functionName, int nArgs) {
    // push the return address onto the stack
    outFile << "@" << functionName << "$" << "ret." << callCount << "\n";
    outFile << "D=A\n";
    basicPushTail();

    // push LCL, ARG, THIS, THAT onto the stack
    outFile << "@" << "LCL\n";
    outFile << "D=M\n";
    basicPushTail();

    outFile << "@" << "ARG\n";
    outFile << "D=M\n";
    basicPushTail();

    outFile << "@" << "THIS\n";
    outFile << "D=M\n";
    basicPushTail();

    outFile << "@" << "THAT\n";
    outFile << "D=M\n";
    basicPushTail();

    // reposition ARG
    outFile << "@SP\n";
    outFile << "D=M\n";
    outFile << "@" << (5 + nArgs) << "\n";
    outFile << "D=D-A\n";
    outFile << "@ARG\n";
    outFile << "M=D\n";

    // Reposition LCL for the callee
    outFile << "@SP\n";
    outFile << "D=M\n";
    outFile << "@LCL\n";
    outFile << "M=D\n";

    // jump to the function
    outFile << "@" << functionName << "\n";
    outFile << "0;JMP\n";

    // declare the return address label (this is the landing spot of the function after "writeReturn" does its job in the callee)
    outFile << "(" << functionName << "$" << "ret." << callCount << ")\n";

    callCount++;    // increment call count for return address once done returning back to the landing spot
}

void codeWriter::frameManipulation(string offset, string destination) {
    outFile << "@frame\n";
    outFile << "D=M\n";
    outFile << "@" << offset << "\n";
    outFile << "D=D-A\n";
    outFile << "A=D\n";
    outFile << "D=M\n";
    outFile << "@" << destination << "\n";
    outFile << "M=D\n";
}

void codeWriter::writeReturn() {
    // save a pointer to the whole frame
    outFile << "@LCL\n";
    outFile << "D=M\n";
    // LCL is now frame
    outFile << "@frame\n";
    outFile << "M=D\n";

    // Extract the return address using frame
    frameManipulation("5", "retAddr");

    // copy the callee's return value to *ARG (return value is at top of stack)
    basicPop();
    outFile << "@ARG\n";
    outFile << "A=M\n";
    outFile << "M=D\n";

    // pop everythng the callee used (reposition SP for the caller)
    outFile << "@ARG\n";
    outFile << "D=M\n";
    outFile << "@SP\n";
    outFile << "M=D+1\n";

    // restore THAT for the caller
    frameManipulation("1", "THAT");

    // restore THIS for the caller
    frameManipulation("2", "THIS");

    // restore ARG for the caller
    frameManipulation("3", "ARG");

    // restore LCL for the caller
    frameManipulation("4", "LCL");

    // go to the return address
    outFile << "@retAddr\n";
    outFile << "A=M\n";
    outFile << "0;JMP\n";

}