#include "page_table.h"

/**
 * @brief 页表项构造函数
 *
 * 初始化一个无效页表项，表示该虚拟页尚未映射到物理内存。
 * dirty / referenced / on_disk 均初始化为 false。
 */
PageTableEntry::PageTableEntry()
    : valid(false),
      dirty(false),
      referenced(false),
      on_disk(false),
      pfn(0),
      disk_addr(0) {}

/**
 * @brief 页目录项构造函数
 *
 * 页目录项初始状态为无效，
 * page_table 指针为空，表示尚未分配二级页表。
 */
PageDirEntry::PageDirEntry()
    : valid(false),
      page_table(nullptr) {}

/**
 * @brief 页目录项析构函数
 *
 * ⚠️ 注意：
 * page_directory 通常以数组形式存在，
 * 若在析构函数中释放 page_table，
 * 将在数组析构过程中触发多次 delete，
 * 导致 double-free 错误。
 *
 * 因此，页表内存的释放应由 VirtualMemoryManager
 * 在系统销毁阶段统一管理。
 */
PageDirEntry::~PageDirEntry() {
    // 不在此处释放 page_table，避免重复释放
}

/**
 * @brief 物理页帧结构初始化
 *
 * occupied 表示当前页帧是否被占用；
 * vpn 记录映射到该帧的虚拟页号；
 * last_access 可用于扩展 LRU / Aging 等算法。
 */
PhysicalFrame::PhysicalFrame()
    : occupied(false),
      vpn(0),
      last_access(0) {}
