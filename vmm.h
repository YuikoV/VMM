// 虚拟内存管理器主类
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
    // 页表结构
    PageDirEntry page_directory[1024];
    
    // 物理内存
    PhysicalFrame physical_memory[PHYSICAL_FRAMES];
    uint8_t memory_data[PHYSICAL_FRAMES][PAGE_SIZE];
    
    // 模块组件
    SwapManager swap_manager;
    PageReplacement page_replacement;
    Statistics stats;
    
    // 空闲帧管理
    std::vector<uint32_t> free_frames;
    
public:
    VirtualMemoryManager(const std::string& swap_file = "swap.dat");
    
    // 地址转换
    uint32_t translateAddress(uint32_t vaddr, bool is_write = false);
    
    // 内存访问接口
    uint8_t readByte(uint32_t vaddr);
    void writeByte(uint32_t vaddr, uint8_t value);
    
    // 统计信息
    void printStatistics();
    void resetStatistics();
    
private:
    // 缺页处理
    void handlePageFault(uint32_t pde_idx, uint32_t pte_idx, 
                        PageTableEntry& pte);
    
    // 获取物理帧
    uint32_t allocateFrame();
};

#endif // VMM_H
