# CJ Language

A modern, JIT-compilable programming language with automatic memory management (garbage collection) and a virtual machine runtime.

[![Build Status](https://github.com/Flickyyy/Cj/workflows/CI/badge.svg)](https://github.com/Flickyyy/Cj/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Overview

CJ is a dynamically-typed programming language designed with modern language features:

- **Virtual Machine**: Custom bytecode VM with stack-based execution
- **Garbage Collection**: Automatic memory management with configurable GC algorithms
- **JIT Compilation**: Just-in-time compilation for performance optimization
- **Modern Syntax**: Clean, expressive syntax similar to JavaScript/TypeScript
- **Built-in Types**: Numbers, strings, booleans, arrays, objects, functions
- **Error Handling**: Comprehensive error reporting and recovery

## Features

### Language Features
- Variables (`var`, `const`)
- Functions with closures
- Control flow (`if`/`else`, `while`, `for`)
- Object-oriented programming (`class`, inheritance)
- Exception handling (`try`/`catch`/`finally`)
- Module system (`import`/`export`)

### Runtime Features
- **Virtual Machine**: Stack-based bytecode interpreter
- **Garbage Collector**: Mark-and-sweep, copying, and generational GC
- **JIT Compiler**: x86-64 native code generation with optimizations
- **Memory Management**: Automatic allocation and deallocation
- **Built-in Functions**: Type checking, I/O, string manipulation

### Development Tools
- **Compiler**: Source-to-bytecode compilation
- **REPL**: Interactive read-eval-print loop
- **Debugger**: Breakpoints, stack traces, variable inspection
- **Profiler**: Performance analysis and optimization hints

## Quick Start

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.20+
- Git

### Building

```bash
git clone https://github.com/Flickyyy/Cj.git
cd Cj
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running Examples

```bash
# Run the REPL
./bin/cj-repl

# Execute a CJ file
./bin/cj-vm ../examples/hello.cj

# Compile a CJ file
./bin/cj-compiler ../examples/fibonacci.cj -o fibonacci.cjb

# Run the complete language demo
./bin/examples/complete_demo

# Run the SSA (Static Single Assignment) demo
./bin/examples/ssa_demo
```

## Language Syntax

### Variables and Types

```javascript
// Variables
var name = "CJ Language";
var version = 0.1;
var is_ready = true;

// Constants
const PI = 3.14159;
const MAX_SIZE = 1000;
```

### Functions

```javascript
// Function declaration
function greet(name) {
    return "Hello, " + name + "!";
}

// Function expression
var add = function(a, b) {
    return a + b;
};

// Arrow function (planned)
var multiply = (a, b) => a * b;
```

### Control Flow

```javascript
// Conditional statements
if (age >= 18) {
    print("Adult");
} else {
    print("Minor");
}

// Loops
for (var i = 0; i < 10; i++) {
    print(i);
}

while (condition) {
    // loop body
}
```

### Objects and Arrays

```javascript
// Arrays
var numbers = [1, 2, 3, 4, 5];
var mixed = [1, "hello", true, nil];

// Objects
var person = {
    name: "Alice",
    age: 30,
    greet: function() {
        return "Hi, I'm " + this.name;
    }
};
```

### Classes (Planned)

```javascript
class Animal {
    constructor(name) {
        this.name = name;
    }
    
    speak() {
        print(this.name + " makes a sound");
    }
}

class Dog extends Animal {
    speak() {
        print(this.name + " barks");
    }
}
```

## Architecture

### Project Structure

```
Cj/
├── include/cj/          # Header files
│   ├── common.h         # Common definitions
│   ├── lexer/           # Lexical analysis
│   ├── parser/          # Syntax analysis
│   ├── ast/             # Abstract syntax tree
│   ├── ir/              # Intermediate representation
│   ├── vm/              # Virtual machine
│   ├── gc/              # Garbage collector
│   ├── jit/             # JIT compiler
│   └── types/           # Type system
├── src/                 # Source files
│   ├── lexer/           # Lexer implementation
│   ├── parser/          # Parser implementation  
│   ├── ast/             # AST implementation
│   ├── ir/              # IR implementation
│   ├── vm/              # VM implementation
│   ├── gc/              # GC implementation
│   ├── jit/             # JIT implementation
│   └── utils/           # Utilities
├── tests/               # Test files
├── examples/            # Example programs
├── docs/                # Documentation
└── tools/               # Development tools
```

### Components

#### Lexer
- Tokenizes source code into lexical tokens
- Handles keywords, operators, literals, identifiers
- Supports comments and whitespace handling

#### Parser
- Recursive descent parser
- Generates Abstract Syntax Tree (AST)
- Error recovery and reporting

#### AST
- Tree representation of parsed code
- Visitor pattern for traversal
- Pretty printing and analysis

#### IR (Intermediate Representation)
- Stack-based bytecode instructions
- Control flow graph representation
- Optimization passes

#### Virtual Machine
- Stack-based execution engine
- Built-in function support
- Error handling and debugging

#### Garbage Collector
- Automatic memory management
- Multiple GC algorithms (mark-and-sweep, copying, generational)
- Concurrent and incremental collection

#### JIT Compiler
- Native code generation
- Optimization passes (constant folding, dead code elimination)
- Platform-specific backends (x86-64, ARM64)

## Development

### Building with Debug Information

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

### Running Tests

```bash
# Build and run all tests
make test

# Run specific test
./bin/cj-tests

# Run manual tests
./bin/tests/manual_test_lexer
./bin/tests/manual_test_parser
```

### Code Style

The project follows the Google C++ Style Guide. Use clang-format for consistent formatting:

```bash
# Format all source files
find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format -i
```

### Documentation

Generate documentation with Doxygen:

```bash
# In build directory
make docs
# Open docs/html/index.html
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Coding Guidelines

- Follow Google C++ Style Guide
- Write comprehensive tests for new features
- Document public APIs with Doxygen comments
- Use meaningful variable and function names
- Keep functions small and focused

## Roadmap

### Version 0.1.0 (Current)
- [x] Basic lexer and parser
- [x] AST generation and traversal
- [x] IR generation
- [x] Virtual machine with basic instructions
- [x] Garbage collector (mark-and-sweep)
- [x] JIT compiler framework
- [x] **SSA (Static Single Assignment) form**
- [x] **SSA-based optimizations (constant propagation, dead code elimination)**
- [x] REPL and command-line tools

### Version 0.2.0 (Planned)
- [ ] Complete language feature implementation
- [ ] Module system
- [ ] Standard library
- [ ] Improved error messages
- [ ] Performance optimizations

### Version 0.3.0 (Future)
- [ ] Object-oriented programming
- [ ] Advanced GC algorithms
- [ ] Debugging tools
- [ ] Package manager
- [ ] Language server protocol

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by modern language design principles
- Built with modern C++ best practices
- Uses Google Test for unit testing
- Documentation generated with Doxygen

## Contact

- GitHub: [Flickyyy/Cj](https://github.com/Flickyyy/Cj)
- Issues: [GitHub Issues](https://github.com/Flickyyy/Cj/issues)

---

**CJ Language** - *A modern language for the future* ✨