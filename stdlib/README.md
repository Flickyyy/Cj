# CJ Standard Library

This directory contains the CJ language standard library modules written in pure CJ syntax. These are examples of what the future standard library will look like once the language implementation is complete.

## Current Status

**⚠️ IMPORTANT**: These are **prototype/example** modules written in CJ syntax to demonstrate what the standard library should look like. They are **not yet functional** because:

1. The CJ language parser/interpreter doesn't fully support all the syntax used (classes, modules, imports)
2. The VM doesn't have built-in functions like `make_array()`, `int()`, `input()` etc.
3. Module system and imports are not implemented yet

## Planned Structure

### Core Modules (`stdlib/core/`)
- `array.cj` - Dynamic arrays with automatic resizing
- `string.cj` - String manipulation functions (planned)
- `object.cj` - Object utilities (planned)

### Collections (`stdlib/collections/`)
- `stack.cj` - LIFO stack implementation using Array
- `queue.cj` - FIFO queue implementation using Array
- `list.cj` - Linked list implementation (planned)
- `map.cj` - Hash map implementation (planned)

### Math (`stdlib/math/`)
- `basic.cj` - Basic mathematical functions and constants
- `random.cj` - Random number generation (planned)
- `geometry.cj` - Geometric calculations (planned)

### I/O (`stdlib/io/`)
- `console.cj` - Console input/output functions
- `file.cj` - File operations (planned)
- `json.cj` - JSON parsing/serialization (planned)

## Usage Examples

Once the language implementation supports modules and imports, usage would look like:

```cj
import { Array } from "stdlib/core/array";
import { Stack, Queue } from "stdlib/collections";
import { abs, max, sqrt } from "stdlib/math/basic";
import { println } from "stdlib/io/console";

// Create and use a stack
var stack = new Stack();
stack.push(1);
stack.push(2);
stack.push(3);

println("Stack size: " + stack.size());
println("Top element: " + stack.peek());

// Use math functions
var result = sqrt(abs(-16));
println("sqrt(abs(-16)) = " + result);

// Create a dynamic array
var numbers = new Array();
numbers.push(10);
numbers.push(20);
numbers.push(30);

println("Array: " + numbers.toString());
```

## Implementation Roadmap

For Version 0.2.0:
- [ ] Implement module system and import/export
- [ ] Add built-in functions for memory allocation
- [ ] Implement class system with constructors and methods
- [ ] Add string interpolation and manipulation
- [ ] Build VM support for all required operations

## Current Workaround

Until the native CJ standard library is ready, the project uses C++ wrapper classes in `include/cj/stdlib/simple_containers.h` as placeholders. These provide basic functionality for demonstrations but are not part of the actual CJ language.