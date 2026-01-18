// 页表与物理内存数据结构定义
// 用于模拟 32 位系统下的二级分页机制

#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "config.h"

/**
 * @struct PageTableEntry
 * @brief 二级页表项（PTE）
 *
 * 描述一个虚拟页到物理页框的映射关系。
 * 该结构包含页面状态位以及物理页框号，
 * 用于支持页面置换与缺页处理。
 */
struct PageTableEntry {
    bool valid;        // 有效位：是否驻留在物理内存
    bool dirty;        // 脏位：页面是否被写过
    bool referenced;   // 引用位：用于页面置换算法
    bool on_disk;      // 是否已写入交换区

    uint32_t pfn;      // 物理页框号（Page Frame Number）
    uint32_t disk_addr;// 页面在交换区中的地址

    // 构造函数：初始化为无效页
    PageTableEntry();
};

/**
 * @struct PageDirEntry
 * @brief 页目录项（PDE）
 *
 * 页目录项用于定位二级页表。
 * 当 valid 为 false 时，表示该页目录项尚未分配页表。
 */
struct PageDirEntry {
    bool valid;                  // 是否存在对应的二级页表
    PageTableEntry* page_table;  // 指向二级页表数组（1024 项）

    PageDirEntry();

    /**
     * 析构函数说明：
     * 不在此释放 page_table，
     * 页表内存由 VirtualMemoryManager 统一管理，
     * 避免数组析构引发的 double-free 问题。
     */
    ~PageDirEntry();
};

/**
 * @struct PhysicalFrame
 * @brief 物理页框结构
 *
 * 用于描述物理内存中的一个页框，
 * 支持从物理页框反向定位虚拟页（反向映射）。
 */
struct PhysicalFrame {
    bool occupied;      // 是否被占用
    uint32_t vpn;       // 当前映射的虚拟页号（反向映射）
    uint32_t last_access; // 最后访问时间（用于扩展算法）

    PhysicalFrame();
};

#endif // PAGE_TABLE_H
