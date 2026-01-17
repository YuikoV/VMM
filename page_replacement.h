// 页面置换算法
#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "config.h"
#include "page_table.h"

class PageReplacement {
private:
    uint32_t clock_hand;  // Clock算法指针
    PhysicalFrame* physical_memory;
    PageDirEntry* page_directory;
    
public:
    PageReplacement(PhysicalFrame* frames, PageDirEntry* page_dir);
    
    // 改进型Clock置换算法
    uint32_t selectVictim();
    
    // 重置Clock指针
    void resetClock() { clock_hand = 0; }
};

#endif // PAGE_REPLACEMENT_H
