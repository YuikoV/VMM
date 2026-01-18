// 性能统计模块
// 用于记录虚拟内存管理器运行过程中的关键性能指标

#ifndef STATISTICS_H
#define STATISTICS_H

#include <cstdint>
#include <iostream>

/**
 * @class Statistics
 * @brief 虚拟内存管理器性能统计类
 *
 * 本类用于统计系统运行过程中产生的访问次数、
 * 页面命中次数、缺页次数以及缺页处理时间。
 *
 * 该模块不参与地址转换、页面置换等核心逻辑，
 * 仅作为被动记录者，由 VirtualMemoryManager 调用，
 * 以保证模块之间的低耦合性。
 */
class Statistics {
private:
    uint64_t total_accesses;   // 总内存访问次数
    uint64_t page_faults;      // 缺页次数
    uint64_t page_hits;        // 页面命中次数
    double total_fault_time;   // 累计缺页处理时间（毫秒）

public:
    /**
     * @brief 构造函数
     *
     * 初始化所有统计量为 0。
     */
    Statistics();

    /**
     * @brief 记录一次内存访问
     *
     * 每次虚拟地址访问时调用。
     */
    void recordAccess() { total_accesses++; }

    /**
     * @brief 记录一次页面命中
     *
     * 当访问命中页表项且无需缺页处理时调用。
     */
    void recordHit() { page_hits++; }

    /**
     * @brief 记录一次缺页事件
     *
     * @param time_ms 本次缺页处理消耗的时间（毫秒）
     */
    void recordFault(double time_ms);

    /**
     * @brief 输出统计信息
     *
     * @param disk_reads  磁盘读取次数
     * @param disk_writes 磁盘写入次数
     * @param frames_used 当前占用的物理页帧数
     */
    void print(uint64_t disk_reads,
               uint64_t disk_writes,
               uint32_t frames_used) const;

    /**
     * @brief 重置统计数据
     *
     * 用于多轮测试或系统重新初始化。
     */
    void reset();

    // ===== 查询接口（Getters） =====
    uint64_t getTotalAccesses() const { return total_accesses; }
    uint64_t getPageFaults() const { return page_faults; }
    double getHitRate() const;
};

#endif // STATISTICS_H
