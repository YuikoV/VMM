#include "address_translator.h"

void AddressTranslator::parseVirtualAddress(uint32_t vaddr,
                                           uint32_t& pde_idx,
                                           uint32_t& pte_idx,
                                           uint32_t& offset) {
    offset = vaddr & 0xFFF;                          // 低12位
    pte_idx = (vaddr >> PAGE_OFFSET_BITS) & 0x3FF;  // 中间10位
    pde_idx = (vaddr >> (PAGE_OFFSET_BITS + PAGE_TABLE_BITS)) & 0x3FF; // 高10位
}

uint32_t AddressTranslator::constructPhysicalAddress(uint32_t pfn, 
                                                     uint32_t offset) {
    return (pfn << PAGE_OFFSET_BITS) | offset;
}

uint32_t AddressTranslator::getVPN(uint32_t pde_idx, uint32_t pte_idx) {
    return (pde_idx << PAGE_TABLE_BITS) | pte_idx;
}
