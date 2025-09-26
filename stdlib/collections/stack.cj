// CJ Standard Library - Collections Stack Module
// Author: CJ Language Team
// Version: 0.2.0-dev

import { Array } from "../core/array";

// Stack implementation using CJ Array
class Stack {
    var items;
    
    // Constructor
    function Stack() {
        this.items = new Array();
    }
    
    // Push element onto stack
    function push(element) {
        this.items.push(element);
    }
    
    // Pop element from stack
    function pop() {
        if (this.empty()) {
            return nil;
        }
        return this.items.pop();
    }
    
    // Peek at top element without removing
    function peek() {
        if (this.empty()) {
            return nil;
        }
        return this.items.get(this.items.length() - 1);
    }
    
    // Check if stack is empty
    function empty() {
        return this.items.empty();
    }
    
    // Get stack size
    function size() {
        return this.items.length();
    }
    
    // Clear all elements
    function clear() {
        this.items.clear();
    }
    
    // Convert to string representation
    function toString() {
        return "Stack" + this.items.toString();
    }
}

// Export the Stack class
export Stack;