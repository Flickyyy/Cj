/**
 * @file repl_main.cpp
 * @brief Main entry point for CJ REPL (Read-Eval-Print Loop)
 */

#include "cj/cj.h"
#include <iostream>
#include <cstring>

using namespace cj;

void PrintUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help       Show this help message" << std::endl;
    std::cout << "  -v, --version    Show version information" << std::endl;
    std::cout << "  --debug          Enable debug mode" << std::endl;
    std::cout << "  --no-jit         Disable JIT compilation" << std::endl;
}

void PrintVersion() {
    std::cout << "CJ REPL v" << VERSION << std::endl;
    std::cout << "Built with C++ " << __cplusplus << std::endl;
}

void PrintHelp() {
    std::cout << "\nCJ REPL Commands:" << std::endl;
    std::cout << "  exit, quit       Exit REPL" << std::endl;
    std::cout << "  help             Show this help" << std::endl;
    std::cout << "  clear            Clear screen" << std::endl;
    std::cout << "  stats            Show VM statistics" << std::endl;
    std::cout << "  gc               Force garbage collection" << std::endl;
    std::cout << "  reset            Reset VM state" << std::endl;
    std::cout << "\nEnter CJ expressions to evaluate them." << std::endl;
}

int main(int argc, char* argv[]) {
    bool debug_mode = false;
    bool disable_jit = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0) {
            PrintUsage(argv[0]);
            return 0;
        } else if (std::strcmp(argv[i], "-v") == 0 || std::strcmp(argv[i], "--version") == 0) {
            PrintVersion();
            return 0;
        } else if (std::strcmp(argv[i], "--debug") == 0) {
            debug_mode = true;
        } else if (std::strcmp(argv[i], "--no-jit") == 0) {
            disable_jit = true;
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option " << argv[i] << std::endl;
            return 1;
        }
    }
    
    try {
        // Configure engine options
        VMOptions vm_opts;
        vm_opts.debug_mode = debug_mode;
        vm_opts.enable_jit = !disable_jit;
        
        GCOptions gc_opts;
        gc_opts.debug_gc = debug_mode;
        
        JITOptions jit_opts;
        jit_opts.debug_jit = debug_mode;
        
        // Create engine
        auto engine = make_unique<CJEngine>(vm_opts, gc_opts, jit_opts);
        
        // Print welcome message
        std::cout << "CJ Language REPL v" << VERSION << std::endl;
        std::cout << "Type 'help' for commands, 'exit' to quit" << std::endl;
        
        String line;
        while (true) {
            std::cout << "cj> ";
            if (!std::getline(std::cin, line)) {
                break;
            }
            
            if (line.empty()) {
                continue;
            }
            
            // Handle special commands
            if (line == "exit" || line == "quit") {
                break;
            } else if (line == "help") {
                PrintHelp();
                continue;
            } else if (line == "clear") {
                #ifdef _WIN32
                system("cls");
                #else
                system("clear");
                #endif
                continue;
            } else if (line == "stats") {
                engine->PrintStats();
                continue;
            } else if (line == "gc") {
                if (engine->GetVM()->GetGC()) {
                    engine->GetVM()->GetGC()->Collect();
                    std::cout << "Garbage collection completed." << std::endl;
                } else {
                    std::cout << "Garbage collector not available." << std::endl;
                }
                continue;
            } else if (line == "reset") {
                engine->GetVM()->Reset();
                std::cout << "VM state reset." << std::endl;
                continue;
            }
            
            // Execute CJ code
            try {
                Value result = engine->Evaluate(line);
                if (!result.IsNil()) {
                    std::cout << result.ToString() << std::endl;
                }
            } catch (const CJException& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        
        std::cout << "\nGoodbye!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}