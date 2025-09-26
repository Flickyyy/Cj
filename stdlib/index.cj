// CJ Standard Library - Main Index
// Author: CJ Language Team
// Version: 0.2.0-dev
//
// This is the main entry point for the CJ standard library.
// When the module system is implemented, this will provide
// convenient access to all standard library modules.

// Re-export core modules
export { Array } from "./core/array";

// Re-export collections
export { Stack } from "./collections/stack";
export { Queue } from "./collections/queue";

// Re-export math functions
export { 
    abs, max, min, pow, sqrt, floor, ceil, round, 
    clamp, lerp, PI, E 
} from "./math/basic";

// Re-export I/O functions
export { 
    println, print_error, print_warning, print_info, print_debug,
    printf, readline, read_number, read_boolean, clear_console 
} from "./io/console";

// Standard library version
const VERSION = "0.2.0-dev";

// Get standard library version
function version() {
    return VERSION;
}

export version;