// 页面置换模块（Page Replacement）
// 负责在物理内存耗尽时选择合适的页帧进行置换

#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "config.h"
#include "page_table.h"

/**
 * @class PageReplacement
 * @brief 页面置换算法实现类
 *
 * 本类实现改进型 Clock（Enhanced Clock）页面置换算法，
 * 在传统二次机会算法基础上，引入引用位与脏位，
 * 以在页面命中率与磁盘 I/O 开销之间取得平衡。
 *
 * 该类维护一个循环指针（Clock Hand），
 * 通过顺序扫描物理帧避免每次缺页都进行全表 O(n) 遍历，
 * 在长期运行中具有接近 O(1) 的均摊时间复杂度。
 */
class PageReplacement {
private:
    // Clock 算法指针，指向当前检查的物理页帧
    uint32_t clock_hand;

    // 物理帧数组，用于获取每个帧当前映射的虚拟页号
    PhysicalFrame* physical_memory;

    // 页目录指针，用于根据虚拟页号定位页表项
    PageDirEntry* page_directory;

public:
    /**
     * @brief 构造函数
     *
     * @param frames   物理帧数组指针
     * @param page_dir 页目录指针
     */
    PageReplacement(PhysicalFrame* frames, PageDirEntry* page_dir);

    /**
     * @brief 选择一个待置换的物理页帧
     *
     * 采用改进型 Clock（Enhanced Clock）算法，根据页面的
     * 引用位和脏位信息综合判断淘汰对象。
     *
     * 页面优先级（由高到低）：
     *  1. 未引用且干净
     *  2. 未引用但脏
     *  3. 已引用但干净
     *  4. 已引用且脏
     *
     * @return 被选中的物理页帧号
     */
    uint32_t selectVictim();

    /**
     * @brief 重置 Clock 指针
     *
     * 主要用于系统重置或测试场景，
     * 将 Clock 扫描指针重新置为起始位置。
     */
    void resetClock() { clock_hand = 0; }
};

#endif // PAGE_REPLACEMENT_H
