#include "statistics.h"
#include "config.h"   // 使用 PHYSICAL_FRAMES 常量
#include <iomanip>

/**
 * @brief 构造函数
 *
 * 初始化所有统计量为 0，
 * 用于系统启动或统计信息重置。
 */
Statistics::Statistics()
    : total_accesses(0),
      page_faults(0),
      page_hits(0),
      total_fault_time(0.0) {}

/**
 * @brief 记录一次缺页事件
 *
 * @param time_ms 本次缺页处理所消耗的时间（毫秒）
 *
 * 在缺页发生后由虚拟内存管理器调用，
 * 用于统计缺页次数及平均处理时间。
 */
void Statistics::recordFault(double time_ms) {
    page_faults++;
    total_fault_time += time_ms;
}

/**
 * @brief 计算页面命中率
 *
 * @return 命中率（百分比）
 */
double Statistics::getHitRate() const {
    if (total_accesses == 0) return 0.0;
    return 100.0 * page_hits / total_accesses;
}

/**
 * @brief 输出统计信息
 *
 * @param disk_reads  磁盘读取次数
 * @param disk_writes 磁盘写入次数
 * @param frames_used 当前使用的物理页帧数
 *
 * 该函数用于在测试完成后统一输出
 * 虚拟内存管理器的性能指标。
 */
void Statistics::print(uint64_t disk_reads,
                       uint64_t disk_writes,
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
                  << (total_fault_time / page_faults)
                  << " ms" << std::endl;
    }

    std::cout << "磁盘读取次数: " << disk_reads << std::endl;
    std::cout << "磁盘写入次数: " << disk_writes << std::endl;
    std::cout << "当前占用物理帧: "
              << frames_used << "/"
              << PHYSICAL_FRAMES << std::endl;
    std::cout << "==========================================\n";
}

/**
 * @brief 重置所有统计信息
 *
 * 用于多轮测试或系统重新初始化场景。
 */
void Statistics::reset() {
    total_accesses = 0;
    page_faults = 0;
    page_hits = 0;
    total_fault_time = 0.0;
}
