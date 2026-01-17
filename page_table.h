// 页表数据结构定义
#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "config.h"

// 页表项结构
struct PageTableEntry {
    bool valid;              // 有效位
    bool dirty;              // 脏位
    bool referenced;         // 引用位
    bool on_disk;        // 是否在磁盘上
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