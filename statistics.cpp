#include "statistics.h"
#include <iomanip>
#include "config.h"
Statistics::Statistics() 
    : total_accesses(0), page_faults(0), 
      page_hits(0), total_fault_time(0.0) {}

void Statistics::recordFault(double time_ms) {
    page_faults++;
    total_fault_time += time_ms;
}

double Statistics::getHitRate() const {
    if (total_accesses == 0) return 0.0;
    return 100.0 * page_hits / total_accesses;
}

void Statistics::print(uint64_t disk_reads, uint64_t disk_writes,
                      uint32_t frames_used) const {
    std::cout << "\n========== 虚拟内存管理器统计信息 ==========\n";
    std::cout << "总访问次数: " << total_accesses << std::endl;
    std::cout << "页面命中次数: " << page_hits << std::endl;
    std::cout << "缺页次数: " << page_faults << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "页面命中率: " << getHitRate() << "%" << std::endl;
    std::cout << "缺页率: " << (100.0 - getHitRate()) << "%" << std::endl;
    
    if (page_faults > 0) {
        std::cout << "平均缺页处理时间: " 
                  << (total_fault_time / page_faults) << " ms" << std::endl;
    }
    
    std::cout << "磁盘读取次数: " << disk_reads << std::endl;
    std::cout << "磁盘写入次数: " << disk_writes << std::endl;
    std::cout << "当前占用物理帧: " << frames_used 
              << "/" << PHYSICAL_FRAMES << std::endl;
    std::cout << "==========================================\n";
}

void Statistics::reset() {
    total_accesses = 0;
    page_faults = 0;
    page_hits = 0;
    total_fault_time = 0.0;
}
