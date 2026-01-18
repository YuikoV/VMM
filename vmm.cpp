#include "vmm.h"
#include <chrono>
#include <cstring>

using namespace std::chrono;

/*
 * VirtualMemoryManager 构造函数
 * 设计思路：
 * 1. 初始化交换区管理器（模拟磁盘）
 * 2. 初始化页面置换算法（改进型 Clock）
 * 3. 初始化空闲物理页框列表
 */
VirtualMemoryManager::VirtualMemoryManager(const std::string& swap_file)
    : swap_manager(swap_file),
      page_replacement(physical_memory, page_directory) {

    // 初始化所有物理页框为空闲
    for (uint32_t i = 0; i < PHYSICAL_FRAMES; i++) {
        free_frames.push_back(i);
    }

    // 初始化交换文件（预分配交换区空间）
    swap_manager.initialize();
}

/*
 * 地址转换主流程（核心函数）
 * 功能：
 * 1. 将 32 位虚拟地址拆分为 PDE / PTE / Offset
 * 2. 进行二级页表查找
 * 3. 若发生缺页，触发缺页中断处理
 * 4. 返回最终物理地址
 */
uint32_t VirtualMemoryManager::translateAddress(uint32_t vaddr,
                                                bool is_write) {
    // 记录一次访存
    stats.recordAccess();

    uint32_t pde_idx, pte_idx, offset;

    // 虚拟地址拆分：10 位页目录 + 10 位页表 + 12 位页内偏移
    AddressTranslator::parseVirtualAddress(vaddr, pde_idx, pte_idx, offset);

    /*
     * 一级页表（页目录）检查
     * 若该页目录项无效，则动态分配一个二级页表
     */
    if (!page_directory[pde_idx].valid) {
        page_directory[pde_idx].page_table = new PageTableEntry[1024];
        page_directory[pde_idx].valid = true;
    }

    // 定位到具体的页表项
    PageTableEntry& pte = page_directory[pde_idx].page_table[pte_idx];

    /*
     * 二级页表检查
     * 若页表项无效，说明发生缺页
     */
    if (!pte.valid) {
        handlePageFault(pde_idx, pte_idx, pte);
    } else {
        // 命中
        stats.recordHit();
    }

    /*
     * 更新页表项访问信息
     * referenced：用于 Clock 页面置换算法
     * dirty：写操作时置脏
     */
    pte.referenced = true;
    if (is_write) {
        pte.dirty = true;
    }

    // 拼接物理地址：PFN + offset
    return AddressTranslator::constructPhysicalAddress(pte.pfn, offset);
}

/*
 * 缺页中断处理函数
 * 设计思路：
 * 1. 分配物理页框（必要时进行页面置换）
 * 2. 若页面在磁盘中，读取；否则初始化为 0
 * 3. 更新页表项和物理页框反向映射
 * 4. 统计缺页处理时间
 */
void VirtualMemoryManager::handlePageFault(uint32_t pde_idx,
                                           uint32_t pte_idx,
                                           PageTableEntry& pte) {
    auto start = high_resolution_clock::now();

    // 计算虚拟页号 VPN（用于反向映射）
    uint32_t vpn = AddressTranslator::getVPN(pde_idx, pte_idx);

    // 分配物理页框
    uint32_t frame = allocateFrame();

    /*
     * 页面调入
     * - 如果该页曾被换出到磁盘，则从交换区读取
     * - 否则初始化为全 0（首次访问的匿名页）
     */
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

    // 更新物理页框信息（反向映射）
    physical_memory[frame].occupied = true;
    physical_memory[frame].vpn = vpn;

    // 记录缺页处理时间
    auto end = high_resolution_clock::now();
    duration<double, std::milli> elapsed = end - start;
    stats.recordFault(elapsed.count());
}

/*
 * 物理页框分配函数
 * 优先级：
 * 1. 有空闲页框：直接使用
 * 2. 无空闲页框：触发页面置换（改进型 Clock）
 */
uint32_t VirtualMemoryManager::allocateFrame() {
    // 直接使用空闲页框
    if (!free_frames.empty()) {
        uint32_t frame = free_frames.back();
        free_frames.pop_back();
        return frame;
    }

    /*
     * 页面置换流程
     * 使用改进型 Clock（二次机会 + 脏位）
     */
    uint32_t victim = page_replacement.selectVictim();

    // 通过反向映射找到被换出页的页表项
    uint32_t old_vpn = physical_memory[victim].vpn;
    uint32_t old_pde = old_vpn >> PAGE_TABLE_BITS;
    uint32_t old_pte = old_vpn & 0x3FF;

    PageTableEntry& old_entry =
        page_directory[old_pde].page_table[old_pte];

    /*
     * 若被换出页面为脏页，需要写回磁盘
     */
    if (old_entry.dirty) {
        if (!old_entry.on_disk) {
            old_entry.disk_addr = swap_manager.allocateSpace();
            old_entry.on_disk = true;
        }
        swap_manager.savePage(memory_data[victim], old_entry.disk_addr);
    }

    // 置换完成，原页表项失效
    old_entry.valid = false;

    return victim;
}

/*
 * 读操作接口
 * 先进行地址转换，再访问物理内存
 */
uint8_t VirtualMemoryManager::readByte(uint32_t vaddr) {
    uint32_t paddr = translateAddress(vaddr, false);
    uint32_t frame = paddr >> PAGE_OFFSET_BITS;
    uint32_t offset = paddr & 0xFFF;
    return memory_data[frame][offset];
}

/*
 * 写操作接口
 * 写访问会触发 dirty 位更新
 */
void VirtualMemoryManager::writeByte(uint32_t vaddr, uint8_t value) {
    uint32_t paddr = translateAddress(vaddr, true);
    uint32_t frame = paddr >> PAGE_OFFSET_BITS;
    uint32_t offset = paddr & 0xFFF;
    memory_data[frame][offset] = value;
}

/*
 * 输出性能统计信息
 */
void VirtualMemoryManager::printStatistics() {
    stats.print(
        swap_manager.getDiskReads(),
        swap_manager.getDiskWrites(),
        PHYSICAL_FRAMES - free_frames.size()
    );
}

/*
 * 重置统计信息（用于多组测试）
 */
void VirtualMemoryManager::resetStatistics() {
    stats.reset();
    swap_manager.resetStats();
}
