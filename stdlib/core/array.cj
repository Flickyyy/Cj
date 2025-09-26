// CJ Standard Library - Core Array Module
// Author: CJ Language Team
// Version: 0.2.0-dev

// Dynamic array implementation in pure CJ
class Array {
    var data;
    var size;
    var capacity;
    
    // Constructor
    function Array() {
        this.data = nil;
        this.size = 0;
        this.capacity = 0;
    }
    
    // Add element to the end
    function push(element) {
        if (this.size >= this.capacity) {
            this.grow();
        }
        this.data[this.size] = element;
        this.size = this.size + 1;
    }
    
    // Remove element from the end
    function pop() {
        if (this.size > 0) {
            this.size = this.size - 1;
            var element = this.data[this.size];
            this.data[this.size] = nil;
            return element;
        }
        return nil;
    }
    
    // Get element at index
    function get(index) {
        if (index >= 0 && index < this.size) {
            return this.data[index];
        }
        return nil;
    }
    
    // Set element at index
    function set(index, value) {
        if (index >= 0 && index < this.size) {
            this.data[index] = value;
            return true;
        }
        return false;
    }
    
    // Get current size
    function length() {
        return this.size;
    }
    
    // Check if array is empty
    function empty() {
        return this.size == 0;
    }
    
    // Clear all elements
    function clear() {
        var i = 0;
        while (i < this.size) {
            this.data[i] = nil;
            i = i + 1;
        }
        this.size = 0;
    }
    
    // Private: Grow capacity when needed
    function grow() {
        var new_capacity = this.capacity * 2;
        if (new_capacity == 0) {
            new_capacity = 4;
        }
        
        var new_data = make_array(new_capacity);
        var i = 0;
        while (i < this.size) {
            new_data[i] = this.data[i];
            i = i + 1;
        }
        
        this.data = new_data;
        this.capacity = new_capacity;
    }
    
    // Convert to string representation
    function toString() {
        var result = "[";
        var i = 0;
        while (i < this.size) {
            if (i > 0) {
                result = result + ", ";
            }
            result = result + this.data[i];
            i = i + 1;
        }
        result = result + "]";
        return result;
    }
}

// Export the Array class
export Array;