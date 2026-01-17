#include "page_table.h"

PageTableEntry::PageTableEntry() 
    : valid(false), dirty(false), referenced(false), on_disk(false),
      pfn(0), disk_addr(0) {}

PageDirEntry::PageDirEntry() 
    : valid(false), page_table(nullptr) {}

// === BUG修复: 不要在析构函数中delete page_table! ===
// 原因: page_directory是数组,析构时会多次调用导致double-free
PageDirEntry::~PageDirEntry() {
    // 注释掉原来的代码
    // if (page_table) delete[] page_table;
    
    // 页表内存应该在VMM析构时统一清理
}

PhysicalFrame::PhysicalFrame() 
    : occupied(false), vpn(0), last_access(0) {}