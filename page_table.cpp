#include "page_table.h"

PageTableEntry::PageTableEntry() 
    : valid(false), dirty(false), referenced(false), 
      pfn(0), disk_addr(0) {}

PageDirEntry::PageDirEntry() 
    : valid(false), page_table(nullptr) {}

PageDirEntry::~PageDirEntry() {
    if (page_table) delete[] page_table;
}

PhysicalFrame::PhysicalFrame() 
    : occupied(false), vpn(0), last_access(0) {}
