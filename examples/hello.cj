// Hello World in CJ Language - Now with Static Typing!
// This demonstrates the new static typing features

// Variables with explicit type annotations
var message: string = "Hello, World!";
print(message);

// Variables and arithmetic with types
var a: int = 10;
var b: int = 20;
var sum: int = a + b;

print("The sum of " + a + " and " + b + " is " + sum);

// Boolean expressions with type annotation
var is_positive: bool = sum > 0;
print("Sum is positive: " + is_positive);

// Mixed: some with types, some inferred
var typed_counter: int = 0;
var inferred_multiplier = 2;  // Type inferred as int

// Conditional statements
if (sum > 25) {
    print("Sum is greater than 25");
} else {
    print("Sum is 25 or less");
}