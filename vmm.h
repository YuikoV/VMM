// =======================================================
// 虚拟内存管理器主类（Virtual Memory Manager）
// 功能：
// 1. 管理二级页表结构（页目录 + 页表）
// 2. 完成虚拟地址到物理地址的转换
// 3. 处理缺页中断并进行页面置换
// 4. 模拟物理内存与磁盘交换区
// 5. 统计并输出性能指标
// =======================================================
#ifndef VMM_H
#define VMM_H

#include "config.h"
#include "page_table.h"
#include "address_translator.h"
#include "swap_manager.h"
#include "page_replacement.h"
#include "statistics.h"
#include <vector>
#include <string>

class VirtualMemoryManager {
private:
    // ================= 页表结构 =================
    // 一级页表（页目录），共 1024 项
    // 每一项指向一个二级页表
    PageDirEntry page_directory[1024];

    // ================= 物理内存模拟 =================
    // 物理页框数组（用于反向映射和置换算法）
    PhysicalFrame physical_memory[PHYSICAL_FRAMES];

    // 实际物理内存数据区
    // 每个物理页框大小为 PAGE_SIZE (4KB)
    uint8_t memory_data[PHYSICAL_FRAMES][PAGE_SIZE];

    // ================= 功能模块组件 =================
    // 交换区管理器（模拟磁盘 I/O）
    SwapManager swap_manager;

    // 页面置换算法（改进型 Clock / 二次机会）
    PageReplacement page_replacement;

    // 性能统计模块
    Statistics stats;

    // ================= 空闲页框管理 =================
    // 维护当前未被占用的物理页框编号
    std::vector<uint32_t> free_frames;

public:
    /*
     * 构造函数
     * 参数：
     * - swap_file：交换区文件名（用于模拟磁盘）
     * 功能：
     * - 初始化各个模块
     * - 建立空闲物理页框列表
     */
    VirtualMemoryManager(const std::string& swap_file = "swap.dat");

    /*
     * 地址转换函数（核心接口）
     * 功能：
     * - 将 32 位虚拟地址转换为物理地址
     * - 若访问的页面不在内存中，触发缺页中断
     * 参数：
     * - vaddr：虚拟地址
     * - is_write：是否为写操作（影响 dirty 位）
     */
    uint32_t translateAddress(uint32_t vaddr, bool is_write = false);

    // ================= 内存访问接口 =================
    // 按字节读取虚拟地址对应的数据
    uint8_t readByte(uint32_t vaddr);

    // 按字节写入虚拟地址对应的数据
    void writeByte(uint32_t vaddr, uint8_t value);

    // ================= 性能统计接口 =================
    // 输出当前统计信息（命中率、缺页率、磁盘 I/O 等）
    void printStatistics();

    // 重置统计信息（用于多组测试）
    void resetStatistics();

private:
    /*
     * 缺页中断处理函数
     * 功能：
     * - 为缺页分配物理页框
     * - 从磁盘加载页面或初始化新页面
     * - 更新页表项和物理页框信息
     */
    void handlePageFault(uint32_t pde_idx,
                         uint32_t pte_idx,
                         PageTableEntry& pte);

    /*
     * 物理页框分配函数
     * 功能：
     * - 优先使用空闲页框
     * - 若物理内存已满，调用页面置换算法选择牺牲页
     */
    uint32_t allocateFrame();
};

#endif // VMM_H
