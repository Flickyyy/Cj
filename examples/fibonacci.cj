// Fibonacci sequence generator in CJ

function fibonacci(n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

// Calculate first 10 Fibonacci numbers
var i = 0;
while (i < 10) {
    var fib = fibonacci(i);
    print("fib(" + i + ") = " + fib);
    i = i + 1;
}

// Iterative version (more efficient)
function fibonacci_iterative(n) {
    if (n <= 1) {
        return n;
    }
    
    var a = 0;
    var b = 1;
    var i = 2;
    
    while (i <= n) {
        var temp = a + b;
        a = b;
        b = temp;
        i = i + 1;
    }
    
    return b;
}

print("\nIterative version:");
var j = 0;
while (j < 10) {
    var fib = fibonacci_iterative(j);
    print("fib(" + j + ") = " + fib);
    j = j + 1;
}