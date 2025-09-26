// Simple calculator in CJ Language

// Basic arithmetic functions
function add(a, b) {
    return a + b;
}

function subtract(a, b) {
    return a - b;
}

function multiply(a, b) {
    return a * b;
}

function divide(a, b) {
    if (b == 0) {
        print("Error: Division by zero!");
        return nil;
    }
    return a / b;
}

function power(base, exponent) {
    if (exponent == 0) {
        return 1;
    }
    
    var result = 1;
    var i = 0;
    while (i < exponent) {
        result = result * base;
        i = i + 1;
    }
    return result;
}

// Test the calculator
var x = 15;
var y = 3;

print("Calculator Demo");
print("===============");
print("x = " + x);
print("y = " + y);
print("");

print("Addition: " + x + " + " + y + " = " + add(x, y));
print("Subtraction: " + x + " - " + y + " = " + subtract(x, y));
print("Multiplication: " + x + " * " + y + " = " + multiply(x, y));
print("Division: " + x + " / " + y + " = " + divide(x, y));
print("Power: " + x + " ^ " + y + " = " + power(x, y));

// Test division by zero
print("");
print("Testing division by zero:");
var result = divide(10, 0);
if (result == nil) {
    print("Division by zero handled correctly");
}

// More complex expressions
var complex = add(multiply(x, y), subtract(power(2, 3), divide(16, 4)));
print("");
print("Complex expression: (" + x + " * " + y + ") + (2^3 - 16/4) = " + complex);