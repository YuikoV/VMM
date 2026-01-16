// 系统配置和常量定义
#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>

const uint32_t PAGE_SIZE = 4096;           // 4KB页面
const uint32_t PAGE_OFFSET_BITS = 12;      // 页内偏移12位
const uint32_t PAGE_TABLE_BITS = 10;       // 页表索引10位
const uint32_t PAGE_DIR_BITS = 10;         // 页目录索引10位
const uint32_t PHYSICAL_FRAMES = 256;      // 物理内存帧数(1MB)
const uint32_t SWAP_SIZE = 1024;           // 交换区大小(4MB)

#endif // CONFIG_H


// ==================== page_table.h ====================
// 页表数据结构定义
#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "config.h"

// 页表项结构
struct PageTableEntry {
    bool valid;              // 有效位
    bool dirty;              // 脏位
    bool referenced;         // 引用位
    uint32_t pfn;           // 物理页框号
    uint32_t disk_addr;     // 磁盘地址
    
    PageTableEntry();
};

// 页目录项结构
struct PageDirEntry {
    bool valid;
    PageTableEntry* page_table;  // 指向二级页表
    
    PageDirEntry();
    ~PageDirEntry();
};

// 物理页框结构
struct PhysicalFrame {
    bool occupied;           // 是否被占用
    uint32_t vpn;           // 虚拟页号(反向映射)
    uint32_t last_access;   // 最后访问时间
    
    PhysicalFrame();
};

#endif // PAGE_TABLE_H
