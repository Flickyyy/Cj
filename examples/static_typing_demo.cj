// Static Typing Demo for CJ Language
// This demonstrates the new static typing features

// Variable declarations with explicit type annotations
var x: int = 42;
var y: float = 3.14;
var message: string = "Hello, CJ with static types!";
var is_active: bool = true;

// Variable without initializer but with type annotation
var counter: int;

// This should work - type inference from initializer
var inferred = 100;

// This should cause a type error (when type checker is integrated)
// var bad_assignment: int = "this is a string";

print("Static typing demo:");
print("x (int): " + x);
print("y (float): " + y);  
print("message (string): " + message);
print("is_active (bool): " + is_active);