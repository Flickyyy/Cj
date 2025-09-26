// CJ Standard Library - Console I/O Module
// Author: CJ Language Team
// Version: 0.2.0-dev

// Console output functions
function println(message) {
    print(message + "\n");
}

function print_error(message) {
    // In a full implementation, this would write to stderr
    print("ERROR: " + message);
}

function print_warning(message) {
    print("WARNING: " + message);
}

function print_info(message) {
    print("INFO: " + message);
}

function print_debug(message) {
    print("DEBUG: " + message);
}

// Formatted printing
function printf(format, ...args) {
    // Simple printf implementation
    // In a full implementation, this would support format specifiers
    var result = format;
    var i = 0;
    while (i < args.length) {
        var placeholder = "{" + i + "}";
        result = result.replace(placeholder, args[i]);
        i = i + 1;
    }
    print(result);
}

// Console input functions (placeholders)
function readline() {
    // In a full implementation, this would read from stdin
    return input();
}

function read_number() {
    var line = readline();
    return number(line);
}

function read_boolean() {
    var line = readline();
    return line == "true" || line == "1" || line == "yes";
}

// Clear console (platform-specific)
function clear_console() {
    // In a full implementation, this would clear the console
    print("\n\n\n");
}

// Export all functions
export { 
    println, print_error, print_warning, print_info, print_debug,
    printf, readline, read_number, read_boolean, clear_console 
};