#include "address_translator.h"

/**
 * @brief 解析32位虚拟地址
 *
 * 将32位虚拟地址按照二级页表结构拆分为：
 * - 页目录索引（PDE index）：高10位
 * - 页表索引（PTE index）：中间10位
 * - 页内偏移（Offset）：低12位
 *
 * 地址格式：
 * | 31........22 | 21........12 | 11........0 |
 * |   PDE(10)    |   PTE(10)    | Offset(12)  |
 *
 * @param vaddr   32位虚拟地址
 * @param pde_idx 输出参数，页目录索引
 * @param pte_idx 输出参数，页表索引
 * @param offset  输出参数，页内偏移
 */
void AddressTranslator::parseVirtualAddress(uint32_t vaddr,
                                            uint32_t& pde_idx,
                                            uint32_t& pte_idx,
                                            uint32_t& offset) {
    // 提取低12位作为页内偏移（4KB 页面大小）
    offset = vaddr & 0xFFF;

    // 右移12位后，取低10位作为页表索引
    pte_idx = (vaddr >> PAGE_OFFSET_BITS) & 0x3FF;

    // 再右移10位，取高10位作为页目录索引
    pde_idx = (vaddr >> (PAGE_OFFSET_BITS + PAGE_TABLE_BITS)) & 0x3FF;
}

/**
 * @brief 构造物理地址
 *
 * 将物理页帧号（PFN）与页内偏移拼接，得到最终物理地址。
 *
 * 物理地址格式：
 * | PFN | Offset |
 *
 * @param pfn     物理页帧号
 * @param offset  页内偏移
 * @return        物理地址
 */
uint32_t AddressTranslator::constructPhysicalAddress(uint32_t pfn,
                                                      uint32_t offset) {
    // PFN 左移12位，低位补上页内偏移
    return (pfn << PAGE_OFFSET_BITS) | offset;
}

/**
 * @brief 计算虚拟页号（VPN）
 *
 * 虚拟页号用于唯一标识一个虚拟页面，
 * 由页目录索引和页表索引共同组成。
 *
 * VPN = PDE_index * 1024 + PTE_index
 *
 * @param pde_idx 页目录索引
 * @param pte_idx 页表索引
 * @return        虚拟页号 VPN
 */
uint32_t AddressTranslator::getVPN(uint32_t pde_idx, uint32_t pte_idx) {
    // 高10位为页目录索引，低10位为页表索引
    return (pde_idx << PAGE_TABLE_BITS) | pte_idx;
}