# CJ Compiler Architecture & Toolchain

This document provides a comprehensive overview of the CJ compiler toolchain, its components, and the underlying computer science concepts that make modern compilers work.

## Table of Contents

- [What is a Compiler Toolchain?](#what-is-a-compiler-toolchain)
- [CJ Toolchain Overview](#cj-toolchain-overview)
- [Compilation Pipeline](#compilation-pipeline)
- [Core Components](#core-components)
- [Advanced Concepts](#advanced-concepts)
- [Optimization Techniques](#optimization-techniques)
- [Runtime System](#runtime-system)

---

## What is a Compiler Toolchain?

A **compiler toolchain** is a collection of programming tools that work together to transform source code written in a high-level programming language into executable machine code or bytecode. Modern compiler toolchains are complex systems that perform multiple stages of analysis, transformation, and optimization.

### Why Do We Need Multiple Stages?

1. **Separation of Concerns**: Each stage handles a specific aspect of compilation
2. **Modularity**: Components can be developed and tested independently
3. **Reusability**: Stages can be shared between different languages or targets
4. **Optimization**: Different optimization techniques work better at different levels
5. **Error Handling**: Each stage can provide targeted error messages

---

## CJ Toolchain Overview

The CJ compiler follows a modern multi-stage architecture:

```
┌─────────────┐    ┌──────────┐    ┌─────────┐    ┌────────────┐
│ Source Code │ -> │  Lexer   │ -> │ Parser  │ -> │    AST     │
│   (.cj)     │    │ (Tokens) │    │ (Syntax)│    │ (Tree Rep) │
└─────────────┘    └──────────┘    └─────────┘    └────────────┘
                                                        │
┌─────────────┐    ┌──────────┐    ┌─────────┐         │
│   Machine   │ <- │   JIT    │ <- │   IR    │ <-------┘
│    Code     │    │Compiler  │    │  Gen    │
└─────────────┘    └──────────┘    └─────────┘
                        │               │
                   ┌─────────┐    ┌──────────┐
                   │   VM    │    │ Type     │
                   │Execution│    │ Checker  │
                   └─────────┘    └──────────┘
```

### Key Tools in the CJ Toolchain

1. **`cj-compiler`**: Main compiler executable
2. **`cj-vm`**: Virtual machine for bytecode execution
3. **`cj-repl`**: Interactive read-eval-print loop
4. **`cj-jit`**: Just-in-time compiler for performance

---

## Compilation Pipeline

### Stage 1: Lexical Analysis (Lexer)
**Location**: `src/lexer/`, `include/cj/lexer/`

The lexer breaks source code into **tokens** - the smallest meaningful units of the language.

```cj
var x = 42 + y;
```

Becomes tokens:
```
[VAR] [IDENTIFIER:x] [ASSIGN] [NUMBER:42] [PLUS] [IDENTIFIER:y] [SEMICOLON]
```

**Purpose**:
- Remove whitespace and comments
- Identify keywords, operators, literals, identifiers
- Detect basic syntax errors (invalid characters)
- Provide position information for error reporting

### Stage 2: Syntax Analysis (Parser)
**Location**: `src/parser/`, `include/cj/parser/`

The parser takes tokens and builds an **Abstract Syntax Tree (AST)** representing the program's structure.

**Purpose**:
- Enforce grammar rules
- Build hierarchical representation of code
- Detect syntax errors
- Handle operator precedence and associativity

**Grammar Example**:
```
expression := term (('+' | '-') term)*
term       := factor (('*' | '/') factor)*
factor     := NUMBER | IDENTIFIER | '(' expression ')'
```

### Stage 3: Semantic Analysis
**Location**: `src/semantic/`, `include/cj/semantic/`

The semantic analyzer performs **type checking** and builds a **symbol table**.

**Key Responsibilities**:
- **Type Checking**: Ensure operations are valid for their operand types
- **Symbol Resolution**: Match identifiers to their declarations
- **Scope Management**: Handle variable visibility rules
- **Type Inference**: Deduce types when not explicitly specified

**Example**:
```cj
var x: int = 42;
var y: float = 3.14;
var z = x + y;  // Type checker infers z: float
```

### Stage 4: IR Generation
**Location**: `src/ir/`, `include/cj/ir/`

The IR (Intermediate Representation) generator converts the AST into a lower-level, platform-independent representation.

**Benefits of IR**:
- **Platform Independence**: Same IR can target multiple architectures
- **Optimization**: Many optimizations work better on IR than source code
- **Analysis**: Easier to perform data flow and control flow analysis

**IR Example**:
```
function main() {
entry:
    LOAD_CONST 42
    STORE_LOCAL 0        // x = 42
    LOAD_CONST 3.14
    STORE_LOCAL 1        // y = 3.14
    LOAD_LOCAL 0
    LOAD_LOCAL 1
    ADD
    STORE_LOCAL 2        // z = x + y
    RETURN
}
```

### Stage 5: Optimization
**Location**: Integrated throughout, especially in `src/jit/`

Multiple optimization passes improve code performance:

1. **Dead Code Elimination**: Remove unreachable code
2. **Constant Folding**: Evaluate constant expressions at compile time
3. **Copy Propagation**: Replace variable copies with original values
4. **Loop Optimization**: Unroll loops, move invariant code out of loops

### Stage 6: Code Generation
**Location**: `src/jit/`, `src/vm/`

Two main approaches:
1. **Bytecode Generation**: For the virtual machine
2. **Native Code Generation**: Direct machine code via JIT compilation

---

## Core Components

### Lexer & Parser (`lexer/`, `parser/`)

**Lexer Responsibilities**:
- Character stream → Token stream
- Handle string literals, numbers, identifiers
- Skip whitespace and comments
- Error recovery for invalid characters

**Parser Responsibilities**:
- Token stream → Abstract Syntax Tree (AST)
- Recursive descent parsing
- Error recovery for syntax errors
- Operator precedence handling

### Type System (`types/`, `semantic/`)

The CJ type system provides:
- **Static Typing**: Types checked at compile time
- **Type Inference**: Automatic type deduction
- **Type Compatibility**: Rules for type conversions
- **Generic Types**: Template-like type parameters (planned)

**Core Types**:
```cj
// Primitive types
var i: int = 42;
var f: float = 3.14;
var s: string = "hello";
var b: bool = true;

// Composite types  
var arr: array<int> = [1, 2, 3];
var obj: object = { x: 10, y: 20 };
```

### Abstract Syntax Tree (`ast/`)

The AST represents program structure in a tree format:

```
Assignment
├── Variable: "x"
└── BinaryOp: "+"
    ├── Literal: 10
    └── Variable: "y"
```

**Node Types**:
- **Literals**: Numbers, strings, booleans
- **Identifiers**: Variable and function names
- **Expressions**: Binary ops, function calls, array access
- **Statements**: Assignments, if/while, function definitions

### Virtual Machine (`vm/`)

The CJ VM is a **stack-based virtual machine** that executes bytecode:

**Stack Operations**:
```
LOAD_CONST 10    // Push 10 onto stack
LOAD_CONST 20    // Push 20 onto stack  
ADD              // Pop two values, push sum
STORE_LOCAL 0    // Pop value, store in local variable
```

**VM Features**:
- **Garbage Collection**: Automatic memory management
- **Exception Handling**: Try/catch mechanism
- **Call Stack**: Function call management
- **Bytecode Interpreter**: Executes IR instructions

### Garbage Collector (`gc/`)

Automatic memory management using **mark-and-sweep** algorithm:

1. **Mark Phase**: Traverse all reachable objects from roots
2. **Sweep Phase**: Deallocate unmarked objects
3. **Compaction**: Reduce memory fragmentation (planned)

**GC Triggers**:
- Allocation pressure
- Explicit `gc_collect()` calls
- Memory threshold exceeded

---

## Advanced Concepts

### Static Single Assignment (SSA) Form

**What is SSA?**

SSA is an intermediate representation where each variable is assigned exactly once and every use is reached by exactly one definition.

**Non-SSA Example**:
```
x = 10
y = x + 5
x = x * 2    // x is reassigned
z = x + y
```

**SSA Form**:
```
x₁ = 10
y₁ = x₁ + 5
x₂ = x₁ * 2
z₁ = x₂ + y₁
```

**Benefits of SSA**:
1. **Simplified Analysis**: Each variable has exactly one definition
2. **Better Optimizations**: Enables aggressive constant propagation
3. **Precise Data Flow**: Clear variable dependencies
4. **Efficient Algorithms**: Many optimization algorithms work better on SSA

**Phi Functions**:
When control flow merges, we need φ (phi) functions:

```cj
if (condition) {
    x = 10;      // x₁ = 10
} else {
    x = 20;      // x₂ = 20
}
// Merge point
print(x);        // x₃ = φ(x₁, x₂)
```

**SSA Construction Algorithm**:
1. **Dominance Analysis**: Find which blocks dominate others
2. **Phi Placement**: Insert φ functions at dominance frontiers
3. **Variable Renaming**: Assign unique versions to variables

### Dominance Analysis

**Dominance**: Block A dominates block B if every path to B passes through A.

**Dominance Frontier**: The set of blocks where dominance ends - where φ functions are needed.

**Example**:
```
     [Entry]
        │
     [Block1]
      /    \
  [Block2] [Block3]
      \    /
     [Block4]
```

- Entry dominates all blocks
- Block4's dominance frontier is empty
- Block2 and Block3 need φ functions for variables modified in both branches

### Control Flow Graph (CFG)

The CFG represents possible execution paths through a program:

**Nodes**: Basic blocks (sequences of instructions with no branches)
**Edges**: Control flow transfers (jumps, branches, calls)

**Basic Block Properties**:
- Single entry point (first instruction)
- Single exit point (last instruction)
- No internal branches or jumps

### Data Flow Analysis

Techniques for gathering information about variable usage:

**Reaching Definitions**: Which assignments can reach a given point
**Live Variables**: Which variables are used before being redefined
**Available Expressions**: Which expressions are computed and not killed

---

## Optimization Techniques

### SSA-Based Optimizations

**Constant Propagation**:
```
// Before
x₁ = 5
y₁ = x₁ + 3
z₁ = y₁ * 2

// After  
x₁ = 5
y₁ = 8        // 5 + 3
z₁ = 16       // 8 * 2
```

**Dead Code Elimination**:
```
// Before
x₁ = 5
y₁ = 10       // y₁ is never used
z₁ = x₁ + 3

// After
x₁ = 5
z₁ = x₁ + 3   // y₁ assignment removed
```

**Copy Propagation**:
```
// Before
x₁ = y₁
z₁ = x₁ + 5

// After
z₁ = y₁ + 5   // x₁ eliminated
```

### Loop Optimizations

**Loop Invariant Code Motion**:
Move calculations that don't change inside the loop outside:

```cj
// Before
for (i = 0; i < n; i++) {
    x = y * z;  // y and z don't change in loop
    arr[i] = x + i;
}

// After  
x = y * z;      // Moved outside loop
for (i = 0; i < n; i++) {
    arr[i] = x + i;
}
```

**Loop Unrolling**:
Reduce loop overhead by duplicating loop body:

```cj
// Before
for (i = 0; i < 4; i++) {
    process(arr[i]);
}

// After (unrolled)
process(arr[0]);
process(arr[1]);
process(arr[2]);
process(arr[3]);
```

### Just-In-Time (JIT) Compilation

The JIT compiler generates native machine code at runtime:

**Benefits**:
- **Performance**: Native code runs faster than bytecode
- **Runtime Optimization**: Can optimize based on actual usage patterns
- **Adaptive**: Can recompile with better optimizations

**JIT Pipeline**:
1. **Profiling**: Identify frequently executed code ("hot spots")
2. **Compilation**: Convert hot IR to native machine code
3. **Optimization**: Apply aggressive optimizations
4. **Caching**: Store compiled code for reuse

**Optimization Levels**:
- **None**: Direct interpretation
- **Basic**: Simple optimizations, fast compilation
- **Standard**: Balanced optimization and compile time
- **Aggressive**: Maximum optimization, slower compilation

---

## Runtime System

### Memory Management

**Heap Layout**:
```
┌─────────────┐  <- High Address
│    Stack    │  (Local variables, call frames)
│      ↓      │
│             │
│      ↑      │
│    Heap     │  (Dynamically allocated objects)
└─────────────┘  <- Low Address
```

**Object Layout**:
```
Object Header:
├── Type Information (vtable pointer)
├── Reference Count / GC Bits
├── Object Size
└── Object Data...
```

### Exception Handling

**Try-Catch Mechanism**:
```cj
try {
    var result = risky_operation();
    print(result);
} catch (error) {
    print("Error: " + error.message);
} finally {
    cleanup();
}
```

**Implementation**:
- **Exception Tables**: Map instruction ranges to handlers
- **Stack Unwinding**: Clean up stack frames during exception propagation
- **RAII**: Resource cleanup through destructors

### Standard Library Architecture

**Module System** (Planned for v0.2.0):
```cj
// stdlib/core/array.cj
export class Array {
    function push(element) { /* ... */ }
    function pop() { /* ... */ }
}

// user code
import { Array } from "stdlib/core/array";
var arr = new Array();
```

**Built-in Functions**:
- Memory allocation (`make_array`, `make_object`)
- Type conversion (`int`, `float`, `string`)
- I/O operations (`print`, `input`)
- Math functions (`abs`, `sqrt`, `sin`)

---

## Future Directions

### Planned Features

**v0.2.0**:
- Complete module system with imports/exports
- Native CJ standard library implementation
- Advanced error messages with suggestions
- Performance profiling tools

**v0.3.0**:
- Object-oriented programming (classes, inheritance)
- Generic types and template system
- Advanced garbage collection (generational GC)
- Debugging tools and language server protocol

### Research Areas

**Advanced Optimizations**:
- **Escape Analysis**: Determine if objects can be stack-allocated
- **Inlining**: Replace function calls with function body
- **Vectorization**: Use SIMD instructions for parallel operations

**Static Analysis**:
- **Null Safety**: Prevent null pointer dereferences at compile time
- **Memory Safety**: Detect buffer overflows and use-after-free
- **Concurrency Analysis**: Detect race conditions and deadlocks

---

## Learning Resources

### Books
- "Compilers: Principles, Techniques, and Tools" (Dragon Book)
- "Modern Compiler Implementation" by Andrew Appel
- "Engineering a Compiler" by Cooper & Torczon

### Online Resources
- [LLVM Tutorial](https://llvm.org/docs/tutorial/)
- [Crafting Interpreters](http://craftinginterpreters.com/)
- [Static Single Assignment Book](http://ssabook.gforge.inria.fr/latest/book.pdf)

### Academic Papers
- "Efficiently Computing Static Single Assignment Form and the Control Dependence Graph" (Cytron et al.)
- "A Simple, Fast Dominance Algorithm" (Cooper, Harvey, Kennedy)
- "Global Value Numbers and Redundant Computations" (Alpern, Wegman, Zadeck)

---

## Conclusion

The CJ compiler represents a modern approach to language implementation, incorporating decades of research in compiler design. From the initial lexical analysis to advanced SSA-based optimizations, each component serves a specific purpose in transforming high-level source code into efficient executable programs.

The modular architecture allows for independent development and testing of components, while the comprehensive optimization pipeline ensures high performance. The addition of SSA form and advanced optimization passes brings CJ closer to production-quality compiler performance.

Understanding these concepts is crucial for:
- **Language Designers**: Making informed decisions about language features
- **Compiler Developers**: Implementing efficient compilation strategies
- **Performance Engineers**: Understanding how code transformations affect runtime performance
- **Computer Science Students**: Grasping fundamental concepts in programming language implementation

The CJ project serves as both a practical compiler implementation and an educational resource for understanding modern compiler construction techniques.