#include "page_replacement.h"

/**
 * @brief 页面置换模块构造函数
 *
 * @param frames   物理帧数组指针，用于获取每个物理帧当前映射的虚拟页
 * @param page_dir 页目录指针，用于根据 VPN 定位页表项
 */
PageReplacement::PageReplacement(PhysicalFrame* frames,
                                 PageDirEntry* page_dir)
    : clock_hand(0),              // Clock 指针初始化为 0
      physical_memory(frames),
      page_directory(page_dir) {}

/**
 * @brief 选择一个待置换的物理页帧
 *
 * 本函数实现改进型 Clock（Enhanced Clock）页面置换算法。
 * 算法综合考虑页面的引用位（referenced）与脏位（dirty），
 * 并通过循环指针避免每次从头扫描所有页面。
 *
 * 页面优先级（从高到低）：
 *  1. 未引用 + 干净
 *  2. 未引用 + 脏
 *  3. 已引用 + 干净
 *  4. 已引用 + 脏
 *
 * @return 被选中的物理页帧号
 */
uint32_t PageReplacement::selectVictim() {
    // 初始化为非法帧号，表示尚未选中
    uint32_t victim_frame = PHYSICAL_FRAMES;

    // 扫描轮数计数，用于限制最多扫描两轮
    int pass = 0;

    // 各类别候选页帧（只记录第一次出现的位置）
    uint32_t candidate1 = PHYSICAL_FRAMES;  // 未引用 + 脏
    uint32_t candidate2 = PHYSICAL_FRAMES;  // 已引用 + 干净
    uint32_t candidate3 = PHYSICAL_FRAMES;  // 已引用 + 脏

    // 循环扫描物理帧（Clock 指针方式）
    while (victim_frame == PHYSICAL_FRAMES) {

        // 通过物理帧记录的 VPN 找到对应页表项
        uint32_t vpn = physical_memory[clock_hand].vpn;
        uint32_t pde_idx = vpn >> PAGE_TABLE_BITS;
        uint32_t pte_idx = vpn & 0x3FF;

        PageTableEntry& pte =
            page_directory[pde_idx].page_table[pte_idx];

        // 类别 0：未引用且干净 —— 最优选择，立即返回
        if (!pte.referenced && !pte.dirty) {
            victim_frame = clock_hand;
            break;
        }

        // 类别 1：未引用但脏（记录首次出现）
        if (!pte.referenced && pte.dirty &&
            candidate1 == PHYSICAL_FRAMES) {
            candidate1 = clock_hand;
        }

        // 类别 2：已引用但干净
        if (pte.referenced && !pte.dirty &&
            candidate2 == PHYSICAL_FRAMES) {
            candidate2 = clock_hand;
        }

        // 类别 3：已引用且脏
        if (pte.referenced && pte.dirty &&
            candidate3 == PHYSICAL_FRAMES) {
            candidate3 = clock_hand;
        }

        // 清除引用位，给予页面“第二次机会”
        pte.referenced = false;

        // Clock 指针向前推进
        clock_hand = (clock_hand + 1) % PHYSICAL_FRAMES;

        // 若完成一整轮扫描
        if (clock_hand == 0) {
            pass++;

            // 最多扫描两轮，防止无限循环
            if (pass >= 2) {
                // 按优先级选择候选页
                if (candidate1 != PHYSICAL_FRAMES) {
                    victim_frame = candidate1;
                } else if (candidate2 != PHYSICAL_FRAMES) {
                    victim_frame = candidate2;
                } else if (candidate3 != PHYSICAL_FRAMES) {
                    victim_frame = candidate3;
                } else {
                    // 理论兜底情况
                    victim_frame = 0;
                }
                break;
            }
        }
    }

    return victim_frame;
}
