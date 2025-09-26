// CJ Standard Library Usage Demo
// This demonstrates how the CJ standard library would be used
// once the module system and class system are implemented.

import { Array } from "stdlib/core/array";
import { Stack, Queue } from "stdlib/collections";
import { abs, max, sqrt, PI } from "stdlib/math/basic";
import { println, printf } from "stdlib/io/console";

function main() {
    println("=== CJ Standard Library Demo ===");
    
    // Test Array
    println("\n📦 Testing Array:");
    var numbers = new Array();
    numbers.push(10);
    numbers.push(20);
    numbers.push(30);
    
    printf("Array: {0}\n", numbers.toString());
    printf("Length: {0}\n", numbers.length());
    printf("Element at index 1: {0}\n", numbers.get(1));
    
    // Test Stack
    println("\n📚 Testing Stack:");
    var stack = new Stack();
    stack.push("first");
    stack.push("second");
    stack.push("third");
    
    printf("Stack: {0}\n", stack.toString());
    printf("Top element: {0}\n", stack.peek());
    printf("Popped: {0}\n", stack.pop());
    printf("Stack after pop: {0}\n", stack.toString());
    
    // Test Queue
    println("\n🚶 Testing Queue:");
    var queue = new Queue();
    queue.enqueue("Alice");
    queue.enqueue("Bob");
    queue.enqueue("Charlie");
    
    printf("Queue: {0}\n", queue.toString());
    printf("Front: {0}\n", queue.front());
    printf("Dequeued: {0}\n", queue.dequeue());
    printf("Queue after dequeue: {0}\n", queue.toString());
    
    // Test Math functions
    println("\n🧮 Testing Math:");
    var x = -16;
    var y = 25;
    
    printf("abs({0}) = {1}\n", x, abs(x));
    printf("max({0}, {1}) = {2}\n", x, y, max(x, y));
    printf("sqrt({0}) = {1}\n", y, sqrt(y));
    printf("PI = {0}\n", PI);
    
    // Test complex operations
    println("\n🔀 Complex Operations:");
    
    // Calculate area of circle
    var radius = 5;
    var area = PI * radius * radius;
    printf("Circle area (radius={0}): {1}\n", radius, area);
    
    // Process array with math
    var processed = new Array();
    var i = 0;
    while (i < numbers.length()) {
        var value = abs(numbers.get(i) * 2 - 50);
        processed.push(value);
        i = i + 1;
    }
    
    printf("Original: {0}\n", numbers.toString());
    printf("Processed: {0}\n", processed.toString());
    
    println("\n✅ All tests completed!");
    println("Note: This is a demonstration of intended syntax.");
    println("Actual implementation requires module system completion.");
}

// Call main function
main();