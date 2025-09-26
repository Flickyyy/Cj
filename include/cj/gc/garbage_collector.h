/**
 * @file garbage_collector.h
 * @brief Garbage Collector implementation for CJ language
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_GARBAGE_COLLECTOR_H
#define CJ_GARBAGE_COLLECTOR_H

#include "../common.h"
#include "../types/value.h"
#include <set>
#include <mutex>
#include <atomic>

namespace cj {

// Forward declarations
class VirtualMachine;

/**
 * @brief GC algorithm types
 */
enum class GCAlgorithm {
    MARK_AND_SWEEP,
    COPYING,
    GENERATIONAL,
    INCREMENTAL
};

/**
 * @brief GC configuration options
 */
struct GCOptions {
    GCAlgorithm algorithm = GCAlgorithm::MARK_AND_SWEEP;
    Size initial_heap_size = 1024 * 1024;        // 1MB
    Size max_heap_size = 512 * 1024 * 1024;      // 512MB
    Float64 gc_threshold = 0.75;                  // Trigger GC at 75% heap usage
    Size min_gc_interval = 100;                   // Minimum allocations between GC
    bool concurrent_gc = false;
    bool incremental_gc = false;
    bool generational_gc = false;
    Size young_generation_size = 256 * 1024;     // 256KB
    Size promotion_threshold = 3;                 // Promote after 3 GC cycles
    bool debug_gc = false;
    
    GCOptions() = default;
};

/**
 * @brief Object header for GC-managed objects
 */
struct ObjectHeader {
    enum Flags : UInt8 {
        MARKED = 0x01,
        PINNED = 0x02,
        FINALIZED = 0x04,
        YOUNG = 0x08,
        OLD = 0x10
    };
    
    Size size;
    UInt8 flags;
    UInt8 generation;
    UInt8 mark_count;
    ObjectHeader* next;
    
    ObjectHeader(Size obj_size) 
        : size(obj_size), flags(0), generation(0), mark_count(0), next(nullptr) {}
    
    bool IsMarked() const { return flags & MARKED; }
    void SetMarked(bool marked) { 
        if (marked) flags |= MARKED; 
        else flags &= ~MARKED; 
    }
    
    bool IsPinned() const { return flags & PINNED; }
    void SetPinned(bool pinned) { 
        if (pinned) flags |= PINNED; 
        else flags &= ~PINNED; 
    }
    
    bool IsYoung() const { return flags & YOUNG; }
    void SetYoung(bool young) { 
        if (young) flags |= YOUNG; 
        else flags &= ~YOUNG; 
    }
    
    void* GetObject() { return reinterpret_cast<void*>(this + 1); }
};

/**
 * @brief Allocation statistics
 */
struct AllocationStats {
    std::atomic<Size> total_allocated{0};
    std::atomic<Size> total_freed{0};
    std::atomic<Size> current_usage{0};
    std::atomic<Size> peak_usage{0};
    std::atomic<Size> allocation_count{0};
    std::atomic<Size> deallocation_count{0};
    std::atomic<Size> gc_count{0};
    std::atomic<Size> gc_time_ms{0};
    
    void Reset() {
        total_allocated = 0;
        total_freed = 0;
        current_usage = 0;
        peak_usage = 0;
        allocation_count = 0;
        deallocation_count = 0;
        gc_count = 0;
        gc_time_ms = 0;
    }
};

/**
 * @brief Root set for GC marking
 */
class RootSet {
private:
    std::set<Value*> roots_;
    mutable std::mutex mutex_;

public:
    void AddRoot(Value* value);
    void RemoveRoot(Value* value);
    void Clear();
    
    template<typename Func>
    void ForEach(Func&& func) const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (Value* root : roots_) {
            func(root);
        }
    }
    
    Size GetSize() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return roots_.size();
    }
};

/**
 * @brief Garbage Collector implementation
 */
class GarbageCollector {
private:
    GCOptions options_;
    VirtualMachine* vm_;
    
    // Memory management
    ObjectHeader* heap_start_;
    ObjectHeader* heap_end_;
    ObjectHeader* free_list_;
    Size heap_size_;
    Size used_size_;
    
    // Generational GC
    ObjectHeader* young_generation_;
    ObjectHeader* old_generation_;
    Size young_size_;
    Size old_size_;
    
    // Root set
    RootSet root_set_;
    
    // Statistics
    AllocationStats stats_;
    
    // Threading
    mutable std::mutex gc_mutex_;
    std::atomic<bool> gc_running_{false};
    std::atomic<bool> stop_requested_{false};
    
    // Incremental GC state
    enum class GCPhase {
        IDLE,
        MARKING,
        SWEEPING,
        COMPACTING
    };
    GCPhase current_phase_;
    ObjectHeader* mark_pointer_;
    ObjectHeader* sweep_pointer_;

public:
    /**
     * @brief Construct GC with options
     */
    explicit GarbageCollector(const GCOptions& options = GCOptions());
    
    /**
     * @brief Destructor
     */
    ~GarbageCollector();
    
    /**
     * @brief Initialize GC with VM reference
     */
    void Initialize(VirtualMachine* vm);
    
    /**
     * @brief Allocate memory for object
     */
    void* Allocate(Size size);
    
    /**
     * @brief Deallocate object (for manual memory management)
     */
    void Deallocate(void* ptr);
    
    /**
     * @brief Trigger garbage collection
     */
    void Collect();
    
    /**
     * @brief Force full garbage collection
     */
    void CollectFull();
    
    /**
     * @brief Root set management
     */
    void AddRoot(Value* value) { root_set_.AddRoot(value); }
    void RemoveRoot(Value* value) { root_set_.RemoveRoot(value); }
    void ClearRoots() { root_set_.Clear(); }
    
    /**
     * @brief Check if collection is needed
     */
    bool ShouldCollect() const;
    
    /**
     * @brief Memory usage information
     */
    Size GetHeapSize() const { return heap_size_; }
    Size GetUsedSize() const { return used_size_; }
    Size GetFreeSize() const { return heap_size_ - used_size_; }
    Float64 GetUsageRatio() const { return static_cast<Float64>(used_size_) / heap_size_; }
    
    /**
     * @brief Statistics
     */
    const AllocationStats& GetStats() const { return stats_; }
    void ResetStats() { stats_.Reset(); }
    
    /**
     * @brief Configuration
     */
    const GCOptions& GetOptions() const { return options_; }
    void SetOptions(const GCOptions& options) { options_ = options; }
    
    /**
     * @brief GC state
     */
    bool IsRunning() const { return gc_running_; }
    GCPhase GetCurrentPhase() const { return current_phase_; }
    
    /**
     * @brief Incremental GC
     */
    void RunIncrementalStep();
    void EnableIncremental(bool enable);
    
    /**
     * @brief Concurrent GC
     */
    void StartConcurrentGC();
    void StopConcurrentGC();
    
    /**
     * @brief Object management
     */
    void PinObject(void* ptr);
    void UnpinObject(void* ptr);
    bool IsObjectAlive(void* ptr) const;
    Size GetObjectSize(void* ptr) const;
    
    /**
     * @brief Debugging
     */
    void DumpHeap() const;
    void PrintStats() const;
    void EnableDebug(bool enable) { options_.debug_gc = enable; }

private:
    // Core GC algorithms
    void MarkAndSweep();
    void CopyingCollect();
    void GenerationalCollect();
    void IncrementalCollect();
    
    // Marking phase
    void MarkRoots();
    void MarkObject(Value* value);
    void MarkObjectHeader(ObjectHeader* header);
    void TraceObject(void* obj, Size size);
    
    // Sweeping phase
    void Sweep();
    void SweepGeneration(ObjectHeader** generation);
    
    // Compaction
    void Compact();
    void UpdateReferences();
    
    // Memory management helpers
    ObjectHeader* FindFreeBlock(Size size);
    void SplitBlock(ObjectHeader* block, Size size);
    void CoalesceBlocks();
    void ExpandHeap(Size min_size);
    
    // Generational GC helpers
    void PromoteObjects();
    void RememberSetUpdate(void* old_obj, void* new_obj);
    
    // Utility methods
    ObjectHeader* GetHeader(void* ptr) const;
    void ValidateHeap() const;
    void UpdateStats(Size allocated, Size freed);
    
    // Debug helpers
    void DebugLog(const String& message) const;
    String FormatSize(Size bytes) const;
};

/**
 * @brief RAII helper for GC roots
 */
class GCRoot {
private:
    GarbageCollector* gc_;
    Value* value_;

public:
    GCRoot(GarbageCollector* gc, Value* value) : gc_(gc), value_(value) {
        if (gc_ && value_) {
            gc_->AddRoot(value_);
        }
    }
    
    ~GCRoot() {
        if (gc_ && value_) {
            gc_->RemoveRoot(value_);
        }
    }
    
    // Non-copyable, movable
    GCRoot(const GCRoot&) = delete;
    GCRoot& operator=(const GCRoot&) = delete;
    
    GCRoot(GCRoot&& other) noexcept : gc_(other.gc_), value_(other.value_) {
        other.gc_ = nullptr;
        other.value_ = nullptr;
    }
    
    GCRoot& operator=(GCRoot&& other) noexcept {
        if (this != &other) {
            if (gc_ && value_) {
                gc_->RemoveRoot(value_);
            }
            gc_ = other.gc_;
            value_ = other.value_;
            other.gc_ = nullptr;
            other.value_ = nullptr;
        }
        return *this;
    }
};

/**
 * @brief GC factory functions
 */
namespace GCFactory {
    /**
     * @brief Create GC with specific algorithm
     */
    UniquePtr<GarbageCollector> Create(GCAlgorithm algorithm, const GCOptions& options = GCOptions());
    
    /**
     * @brief Create GC optimized for development
     */
    UniquePtr<GarbageCollector> CreateDebug();
    
    /**
     * @brief Create GC optimized for production
     */
    UniquePtr<GarbageCollector> CreateRelease();
}

} // namespace cj

#endif // CJ_GARBAGE_COLLECTOR_H