/**
 * @file vm_main.cpp
 * @brief Main entry point for CJ virtual machine
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
    std::cout << "  --debug          Enable debug mode" << std::endl;
    std::cout << "  --trace          Enable execution tracing" << std::endl;
    std::cout << "  --no-jit         Disable JIT compilation" << std::endl;
    std::cout << "  --stats          Show statistics after execution" << std::endl;
    std::cout << "  --gc-stats       Show GC statistics" << std::endl;
}

void PrintVersion() {
    std::cout << "CJ Virtual Machine v" << VERSION << std::endl;
    std::cout << "Built with C++ " << __cplusplus << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }
    
    String input_file;
    bool debug_mode = false;
    bool trace_execution = false;
    bool disable_jit = false;
    bool show_stats = false;
    bool show_gc_stats = false;
    
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
        } else if (std::strcmp(argv[i], "--trace") == 0) {
            trace_execution = true;
        } else if (std::strcmp(argv[i], "--no-jit") == 0) {
            disable_jit = true;
        } else if (std::strcmp(argv[i], "--stats") == 0) {
            show_stats = true;
        } else if (std::strcmp(argv[i], "--gc-stats") == 0) {
            show_gc_stats = true;
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
        // Configure VM options
        VMOptions vm_opts;
        vm_opts.debug_mode = debug_mode;
        vm_opts.trace_execution = trace_execution;
        vm_opts.enable_jit = !disable_jit;
        
        GCOptions gc_opts;
        gc_opts.debug_gc = debug_mode;
        
        JITOptions jit_opts;
        jit_opts.debug_jit = debug_mode;
        
        // Create engine
        auto engine = cj_make_unique<CJEngine>(vm_opts, gc_opts, jit_opts);
        
        std::cout << "Executing " << input_file << "..." << std::endl;
        
        // Execute the file
        Value result = engine->ExecuteFile(input_file);
        
        std::cout << "Execution completed." << std::endl;
        
        if (!result.IsNil()) {
            std::cout << "Result: " << result.ToString() << std::endl;
        }
        
        if (show_stats || show_gc_stats) {
            std::cout << "\n--- Statistics ---" << std::endl;
            engine->PrintStats();
        }
        
    } catch (const CJException& e) {
        std::cerr << "CJ Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}