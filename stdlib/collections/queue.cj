// CJ Standard Library - Collections Queue Module
// Author: CJ Language Team
// Version: 0.2.0-dev

import { Array } from "../core/array";

// Queue implementation using CJ Array
class Queue {
    var items;
    var front_index;
    
    // Constructor
    function Queue() {
        this.items = new Array();
        this.front_index = 0;
    }
    
    // Add element to back of queue
    function enqueue(element) {
        this.items.push(element);
    }
    
    // Remove element from front of queue
    function dequeue() {
        if (this.empty()) {
            return nil;
        }
        
        var element = this.items.get(this.front_index);
        this.front_index = this.front_index + 1;
        
        // Cleanup when queue becomes mostly empty
        if (this.front_index > this.items.length() / 2) {
            this.compact();
        }
        
        return element;
    }
    
    // Peek at front element without removing
    function front() {
        if (this.empty()) {
            return nil;
        }
        return this.items.get(this.front_index);
    }
    
    // Check if queue is empty
    function empty() {
        return this.front_index >= this.items.length();
    }
    
    // Get queue size
    function size() {
        return this.items.length() - this.front_index;
    }
    
    // Clear all elements
    function clear() {
        this.items.clear();
        this.front_index = 0;
    }
    
    // Private: Compact queue by removing processed elements
    function compact() {
        var new_items = new Array();
        var i = this.front_index;
        while (i < this.items.length()) {
            new_items.push(this.items.get(i));
            i = i + 1;
        }
        this.items = new_items;
        this.front_index = 0;
    }
    
    // Convert to string representation
    function toString() {
        var result = "Queue[";
        var i = this.front_index;
        var first = true;
        while (i < this.items.length()) {
            if (!first) {
                result = result + ", ";
            }
            result = result + this.items.get(i);
            first = false;
            i = i + 1;
        }
        result = result + "]";
        return result;
    }
}

// Export the Queue class
export Queue;