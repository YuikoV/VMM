#include "page_replacement.h"

PageReplacement::PageReplacement(PhysicalFrame* frames, 
                                PageDirEntry* page_dir)
    : clock_hand(0), physical_memory(frames), 
      page_directory(page_dir) {}

uint32_t PageReplacement::selectVictim() {
    uint32_t victim_frame = PHYSICAL_FRAMES;
    int pass = 0;
    uint32_t candidate1 = PHYSICAL_FRAMES;  // 未引用,脏
    uint32_t candidate2 = PHYSICAL_FRAMES;  // 引用,干净
    uint32_t candidate3 = PHYSICAL_FRAMES;  // 引用,脏
    
    // 改进Clock: 四级优先策略
    while (victim_frame == PHYSICAL_FRAMES) {
        uint32_t vpn = physical_memory[clock_hand].vpn;
        uint32_t pde_idx = vpn >> PAGE_TABLE_BITS;
        uint32_t pte_idx = vpn & 0x3FF;
        
        PageTableEntry& pte = page_directory[pde_idx].page_table[pte_idx];
        
        // 类别0: 未引用且干净 - 最佳选择
        if (!pte.referenced && !pte.dirty) {
            victim_frame = clock_hand;
            break;
        }
        
        // 类别1: 未引用但脏
        if (!pte.referenced && pte.dirty && candidate1 == PHYSICAL_FRAMES) {
            candidate1 = clock_hand;
        }
        
        // 类别2: 引用但干净
        if (pte.referenced && !pte.dirty && candidate2 == PHYSICAL_FRAMES) {
            candidate2 = clock_hand;
        }
        
        // 类别3: 引用且脏
        if (pte.referenced && pte.dirty && candidate3 == PHYSICAL_FRAMES) {
            candidate3 = clock_hand;
        }
        
        // 给第二次机会
        pte.referenced = false;
        
        clock_hand = (clock_hand + 1) % PHYSICAL_FRAMES;
        
        // 完成一轮扫描
        if (clock_hand == 0) {
            pass++;
            if (pass >= 2) {
                // 按优先级选择
                if (candidate1 != PHYSICAL_FRAMES) {
                    victim_frame = candidate1;
                } else if (candidate2 != PHYSICAL_FRAMES) {
                    victim_frame = candidate2;
                } else if (candidate3 != PHYSICAL_FRAMES) {
                    victim_frame = candidate3;
                } else {
                    victim_frame = 0;
                }
                break;
            }
        }
    }
    
    return victim_frame;
}

