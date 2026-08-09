#include "parser.h"
#include "codeWriter.h"
#include <filesystem>
#include <vector>
using namespace std;

int main(int argc, char* argv[]) {

    namespace fs = std::filesystem;

    if (argc < 2) {
        cerr << "Usage: ./VMTranslator <file.vm>" << endl;
        return 1;
    }

    string inputPath = argv[1];
    vector<string> vmFiles;
    string outputPath;

    // store vm files and establish outputPath
    if (fs::is_directory(inputPath)) {
        for (const auto& entry : fs::directory_iterator(inputPath)) {
            if (entry.path().extension() == ".vm") {
                vmFiles.push_back(entry.path().string());
            }
        }
        outputPath = inputPath;
        // clean the trailing "/" (somethimes exists)
        if (inputPath.substr(inputPath.size()-1) == "/") {
            outputPath = outputPath.erase(outputPath.find_last_of("/"));
        }
        outputPath += ".asm";
    } else {
        vmFiles.push_back(inputPath);
        size_t dotPos = inputPath.find_last_of('.');
        outputPath = inputPath.substr(0, dotPos);
        outputPath += ".asm";
    }

    codeWriter cw(outputPath);
    if (fs::is_directory(inputPath)) {
        cw.writeInit();
    }

    // translate vm code to assembly and output to outputPath
    for (const string& vmFile : vmFiles) {
        cw.setFileName(vmFile);
        ifstream inFile(vmFile);
        if (!inFile.is_open()) { 
            continue; 
        }

        string line;
        while (getline(inFile, line)) {
            parser p(line);
            string type = p.getCommandType();

            if (type == "") {
                continue;
            } else if (type == "C_ARITHMETIC") {
                cw.writeArithmetic(p.arg1());
            } else if (type == "C_PUSH" || type == "C_POP") {
                cw.writePushPop(type, p.arg1(), p.arg2());
            }  else if (type == "C_LABEL") {
                cw.writeLabel(p.arg1());
            } else if (type == "C_GOTO") {
                cw.writeGoto(p.arg1());
            } else if (type == "C_IF") {
                cw.writeIf(p.arg1());
            } else if (type == "C_FUNCTION") {
                cw.writeFunction(p.arg1(), p.arg2());
            } else if (type == "C_CALL") {
                cw.writeCall(p.arg1(), p.arg2());
            } else if (type == "C_RETURN") {
                cw.writeReturn();
            } else {
                cerr << "Warning: command type '" << type << "' not yet supported, skipping: " << line << endl;
            }
           
        }
    }
    cw.close();
    cout << "Translation complete: " << outputPath << endl;
    return 0;
}