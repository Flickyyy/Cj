// CJ Standard Library - Basic Math Module
// Author: CJ Language Team
// Version: 0.2.0-dev

// Mathematical constants
const PI = 3.14159265358979323846;
const E = 2.71828182845904523536;

// Absolute value
function abs(x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

// Maximum of two numbers
function max(a, b) {
    if (a > b) {
        return a;
    }
    return b;
}

// Minimum of two numbers
function min(a, b) {
    if (a < b) {
        return a;
    }
    return b;
}

// Power function (integer exponents only for now)
function pow(base, exponent) {
    if (exponent == 0) {
        return 1;
    }
    
    var result = 1;
    var exp = abs(exponent);
    var i = 0;
    
    while (i < exp) {
        result = result * base;
        i = i + 1;
    }
    
    if (exponent < 0) {
        return 1 / result;
    }
    return result;
}

// Square root (using Newton's method)
function sqrt(x) {
    if (x < 0) {
        return nil; // Invalid input
    }
    if (x == 0) {
        return 0;
    }
    
    var guess = x / 2;
    var epsilon = 0.000001;
    
    while (abs(guess * guess - x) > epsilon) {
        guess = (guess + x / guess) / 2;
    }
    
    return guess;
}

// Floor function
function floor(x) {
    var int_part = int(x);
    if (x >= 0 || x == int_part) {
        return int_part;
    }
    return int_part - 1;
}

// Ceiling function  
function ceil(x) {
    var int_part = int(x);
    if (x <= 0 || x == int_part) {
        return int_part;
    }
    return int_part + 1;
}

// Round to nearest integer
function round(x) {
    return floor(x + 0.5);
}

// Clamp value between min and max
function clamp(value, min_val, max_val) {
    return max(min_val, min(value, max_val));
}

// Linear interpolation
function lerp(a, b, t) {
    return a + (b - a) * t;
}

// Export all functions
export { abs, max, min, pow, sqrt, floor, ceil, round, clamp, lerp, PI, E };