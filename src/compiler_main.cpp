/**
 * @file compiler_main.cpp
 * @brief Main entry point for CJ compiler
 */

#include "cj/cj.h"
#include <iostream>
#include <cstring>

using namespace cj;

void PrintUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] <input-file>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help       Show this help message" << std::endl;
    std::cout << "  -v, --version    Show version information" << std::endl;
    std::cout << "  -o <file>        Output file" << std::endl;
    std::cout << "  --debug          Enable debug mode" << std::endl;
    std::cout << "  --optimize       Enable optimizations" << std::endl;
    std::cout << "  --dump-ast       Dump AST to stdout" << std::endl;
    std::cout << "  --dump-ir        Dump IR to stdout" << std::endl;
}

void PrintVersion() {
    std::cout << "CJ Compiler v" << VERSION << std::endl;
    std::cout << "Built with C++ " << __cplusplus << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }
    
    String input_file;
    String output_file;
    bool debug_mode = false;
    bool optimize = false;
    bool dump_ast = false;
    bool dump_ir = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0) {
            PrintUsage(argv[0]);
            return 0;
        } else if (std::strcmp(argv[i], "-v") == 0 || std::strcmp(argv[i], "--version") == 0) {
            PrintVersion();
            return 0;
        } else if (std::strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                std::cerr << "Error: -o requires output filename" << std::endl;
                return 1;
            }
        } else if (std::strcmp(argv[i], "--debug") == 0) {
            debug_mode = true;
        } else if (std::strcmp(argv[i], "--optimize") == 0) {
            optimize = true;
        } else if (std::strcmp(argv[i], "--dump-ast") == 0) {
            dump_ast = true;
        } else if (std::strcmp(argv[i], "--dump-ir") == 0) {
            dump_ir = true;
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option " << argv[i] << std::endl;
            return 1;
        } else {
            input_file = argv[i];
        }
    }
    
    if (input_file.empty()) {
        std::cerr << "Error: No input file specified" << std::endl;
        PrintUsage(argv[0]);
        return 1;
    }
    
    try {
        // Create engine
        auto engine = debug_mode ? CJUtils::CreateDebugEngine() : CJUtils::CreateEngine();
        
        std::cout << "Compiling " << input_file << "..." << std::endl;
        
        // Parse source file
        auto program = engine->ParseFile(input_file);
        if (!program) {
            std::cerr << "Error: Failed to parse " << input_file << std::endl;
            return 1;
        }
        
        if (dump_ast) {
            std::cout << "\n--- AST ---" << std::endl;
            ASTPrettyPrinter printer;
            program->Accept(printer);
            std::cout << printer.GetOutput() << std::endl;
        }
        
        // Compile to IR
        auto ir_function = engine->Compile(program.get());
        if (!ir_function) {
            std::cerr << "Error: Failed to compile " << input_file << std::endl;
            return 1;
        }
        
        if (dump_ir) {
            std::cout << "\n--- IR ---" << std::endl;
            std::cout << ir_function->ToString() << std::endl;
        }
        
        // Generate output
        if (output_file.empty()) {
            // Default output file
            output_file = input_file;
            size_t dot_pos = output_file.find_last_of('.');
            if (dot_pos != String::npos) {
                output_file = output_file.substr(0, dot_pos);
            }
            output_file += ".cjb"; // CJ bytecode
        }
        
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "Output: " << output_file << std::endl;
        
    } catch (const CJException& e) {
        std::cerr << "CJ Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}