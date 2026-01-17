#include "vmm.h"
#include <chrono>
#include <cstring>

using namespace std::chrono;

VirtualMemoryManager::VirtualMemoryManager(const std::string& swap_file)
    : swap_manager(swap_file),
      page_replacement(physical_memory, page_directory) {
    
    // 初始化空闲帧列表
    for (uint32_t i = 0; i < PHYSICAL_FRAMES; i++) {
        free_frames.push_back(i);
    }
    
    swap_manager.initialize();
}

uint32_t VirtualMemoryManager::translateAddress(uint32_t vaddr, 
                                                bool is_write) {
    stats.recordAccess();
    
    uint32_t pde_idx, pte_idx, offset;
    AddressTranslator::parseVirtualAddress(vaddr, pde_idx, pte_idx, offset);
    
    // 检查页目录项
    if (!page_directory[pde_idx].valid) {
        page_directory[pde_idx].page_table = new PageTableEntry[1024];
        page_directory[pde_idx].valid = true;
    }
    
    PageTableEntry& pte = page_directory[pde_idx].page_table[pte_idx];
    
    // 检查页表项
    if (!pte.valid) {
        handlePageFault(pde_idx, pte_idx, pte);
    } else {
        stats.recordHit();
    }
    
    // 更新访问信息
    pte.referenced = true;
    if (is_write) {
        pte.dirty = true;
    }
    
    return AddressTranslator::constructPhysicalAddress(pte.pfn, offset);
}

void VirtualMemoryManager::handlePageFault(uint32_t pde_idx, 
                                          uint32_t pte_idx,
                                          PageTableEntry& pte) {
    auto start = high_resolution_clock::now();
    
    uint32_t vpn = AddressTranslator::getVPN(pde_idx, pte_idx);
    uint32_t frame = allocateFrame();
    
    // 加载页面数据
    if (pte.on_disk) {
    swap_manager.loadPage(memory_data[frame], pte.disk_addr);
} else {
    std::memset(memory_data[frame], 0, PAGE_SIZE);
}
    
    // 更新页表项
    pte.valid = true;
    pte.pfn = frame;
    pte.referenced = false;
    pte.dirty = false;
    
    // 更新物理帧信息
    physical_memory[frame].occupied = true;
    physical_memory[frame].vpn = vpn;
    
    auto end = high_resolution_clock::now();
    duration<double, std::milli> elapsed = end - start;
    stats.recordFault(elapsed.count());
}

uint32_t VirtualMemoryManager::allocateFrame() {
    if (!free_frames.empty()) {
        uint32_t frame = free_frames.back();
        free_frames.pop_back();
        return frame;
    }
    
    // 页置换
    uint32_t victim = page_replacement.selectVictim();
    
    uint32_t old_vpn = physical_memory[victim].vpn;
    uint32_t old_pde = old_vpn >> PAGE_TABLE_BITS;
    uint32_t old_pte = old_vpn & 0x3FF;
    
    PageTableEntry& old_entry = page_directory[old_pde].page_table[old_pte];
    
    // 写回脏页
    if (old_entry.dirty) {
    if (!old_entry.on_disk) {
        old_entry.disk_addr = swap_manager.allocateSpace();
        old_entry.on_disk = true;
    }
    swap_manager.savePage(memory_data[victim], old_entry.disk_addr);
}
    
    old_entry.valid = false;
    return victim;
}

uint8_t VirtualMemoryManager::readByte(uint32_t vaddr) {
    uint32_t paddr = translateAddress(vaddr, false);
    uint32_t frame = paddr >> PAGE_OFFSET_BITS;
    uint32_t offset = paddr & 0xFFF;
    return memory_data[frame][offset];
}

void VirtualMemoryManager::writeByte(uint32_t vaddr, uint8_t value) {
    uint32_t paddr = translateAddress(vaddr, true);
    uint32_t frame = paddr >> PAGE_OFFSET_BITS;
    uint32_t offset = paddr & 0xFFF;
    memory_data[frame][offset] = value;
}

void VirtualMemoryManager::printStatistics() {
    stats.print(swap_manager.getDiskReads(), 
               swap_manager.getDiskWrites(),
               PHYSICAL_FRAMES - free_frames.size());
}

void VirtualMemoryManager::resetStatistics() {
    stats.reset();
    swap_manager.resetStats();
}
