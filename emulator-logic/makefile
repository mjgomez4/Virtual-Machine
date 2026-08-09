# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra

# Main target
vmtranslator: VMTranslator.cpp parser.cpp codeWriter.cpp
	$(CXX) $(CXXFLAGS) VMTranslator.cpp parser.cpp codeWriter.cpp -o VMTranslator

# Clean up binaries
clean:
	rm -f VMTranslator