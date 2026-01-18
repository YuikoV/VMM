// 地址转换模块（Address Translator）
// 用于模拟硬件 MMU 中的虚拟地址到物理地址转换过程

#ifndef ADDRESS_TRANSLATOR_H
#define ADDRESS_TRANSLATOR_H

#include "config.h"

/**
 * @class AddressTranslator
 * @brief 虚拟地址解析与物理地址构造工具类
 *
 * 本类用于完成以下功能：
 * 1. 按照二级页表结构解析 32 位虚拟地址；
 * 2. 将物理页帧号与页内偏移拼接为物理地址；
 * 3. 生成虚拟页号（VPN），用于页面置换与物理帧管理。
 *
 * 该类仅包含静态方法，不维护任何状态，
 * 用于模拟硬件 MMU 的地址转换逻辑。
 */
class AddressTranslator {
public:
    /**
     * @brief 解析虚拟地址
     *
     * 将 32 位虚拟地址拆分为：
     * - 页目录索引（PDE index）：高 10 位
     * - 页表索引（PTE index）：中间 10 位
     * - 页内偏移（Offset）：低 12 位
     *
     * 虚拟地址格式：
     * | 31........22 | 21........12 | 11........0 |
     * |   PDE(10)    |   PTE(10)    | Offset(12)  |
     *
     * @param vaddr   32 位虚拟地址
     * @param pde_idx 输出参数，页目录索引
     * @param pte_idx 输出参数，页表索引
     * @param offset  输出参数，页内偏移
     */
    static void parseVirtualAddress(uint32_t vaddr,
                                    uint32_t& pde_idx,
                                    uint32_t& pte_idx,
                                    uint32_t& offset);

    /**
     * @brief 构造物理地址
     *
     * 将物理页帧号（PFN）与页内偏移拼接，生成最终物理地址。
     *
     * 物理地址格式：
     * | PFN | Offset |
     *
     * @param pfn     物理页帧号
     * @param offset 页内偏移
     * @return        物理地址
     */
    static uint32_t constructPhysicalAddress(uint32_t pfn,
                                             uint32_t offset);

    /**
     * @brief 计算虚拟页号（VPN）
     *
     * 虚拟页号由页目录索引和页表索引组合而成，
     * 用于唯一标识一个虚拟页面。
     *
     * VPN = PDE_index * 1024 + PTE_index
     *
     * @param pde_idx 页目录索引
     * @param pte_idx 页表索引
     * @return        虚拟页号 VPN
     */
    static uint32_t getVPN(uint32_t pde_idx, uint32_t pte_idx);
};

#endif // ADDRESS_TRANSLATOR_H