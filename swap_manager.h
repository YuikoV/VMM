// 磁盘交换区管理
#ifndef SWAP_MANAGER_H
#define SWAP_MANAGER_H

#include "config.h"
#include <fstream>
#include <string>

class SwapManager {
private:
    std::string swap_file_name;
    std::fstream swap_file;
    bool swap_bitmap[SWAP_SIZE];  // 交换区位图
    
public:
    SwapManager(const std::string& filename = "swap.dat");
    ~SwapManager();
    
    // 初始化交换文件
    void initialize();
    
    // 分配交换空间
    uint32_t allocateSpace();
    
    // 释放交换空间
    void freeSpace(uint32_t disk_addr);
    
    // 从磁盘加载页面
    void loadPage(uint8_t* buffer, uint32_t disk_addr);
    
    // 保存页面到磁盘
    void savePage(const uint8_t* buffer, uint32_t disk_addr);
    
    // 获取统计信息
    uint64_t getDiskReads() const { return disk_reads; }
    uint64_t getDiskWrites() const { return disk_writes; }
    void resetStats();
    
private:
    uint64_t disk_reads;
    uint64_t disk_writes;
};

#endif // SWAP_MANAGER_H
