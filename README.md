# HACK Virtual Machine Translator
A two-stage translator that converts a stack-based virtual machine language into HACK assembly, bridging
high-level program logic and the HACK computer's native instruction set. This program can translate arithmetic/logical commands and basic
memory access commands (push/pop across all segments) as well as program flow commands (label, goto, if-goto) and function calling commands (function, call, return).
The test programs available by default here include: BasicLoop, FibonacciSeries, SimpleFunction, NestedCall,
FibonacciElement, and StaticsTest.

## Project Structure
- emulator-logic: Contains the translator functionality itself (a parser class to process and classify raw VM language instructions, a codeWriter class to convert each VM instruction
into HACK assembly, and a VMTranslator class which contains the main function and handles input/output duties). Also contains 
sample .vm test programs and their expected .cmp/.tst/.asm output, organized by category (ProgramFlow, FunctionCalls).

- testing suites: Contains the .bat and .sh files required for running the CPUEmulator and VMEmulator testing suites locally. In order to run each respective testing suite, you'll need to add the "testing-suites/" directory to your system's PATH variable. Next, type 'CPUEmulator', and 'VMEmulator' into your local terminal to launch each respective application separately.

### Build

```bash
make vmtranslator
```

### Usage (Running the executable)
The translator executable accepts either a single .vm file or a directory containing multiple .vm files. In the
directory case, all files are translated into a single combined .asm file named after the directory. After program execution, take the output (.asm file) and compare it against its
respective .tst file in the CPUEmulator test suite (see "Project Structure" above).

```bash
./VMTranslator input.vm
./VMTranslator input_directory/
```
