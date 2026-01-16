// 性能统计模块
#ifndef STATISTICS_H
#define STATISTICS_H

#include <cstdint>
#include <iostream>

class Statistics {
private:
    uint64_t total_accesses;
    uint64_t page_faults;
    uint64_t page_hits;
    double total_fault_time;  // 毫秒
    
public:
    Statistics();
    
    void recordAccess() { total_accesses++; }
    void recordHit() { page_hits++; }
    void recordFault(double time_ms);
    
    void print(uint64_t disk_reads, uint64_t disk_writes, 
              uint32_t frames_used) const;
    void reset();
    
    // Getters
    uint64_t getTotalAccesses() const { return total_accesses; }
    uint64_t getPageFaults() const { return page_faults; }
    double getHitRate() const;
};

#endif // STATISTICS_H
