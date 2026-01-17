// 地址转换模块
#ifndef ADDRESS_TRANSLATOR_H
#define ADDRESS_TRANSLATOR_H

#include "config.h"

class AddressTranslator {
public:
    // 解析虚拟地址
    static void parseVirtualAddress(uint32_t vaddr, 
                                   uint32_t& pde_idx,
                                   uint32_t& pte_idx, 
                                   uint32_t& offset);
    
    // 构造物理地址
    static uint32_t constructPhysicalAddress(uint32_t pfn, 
                                             uint32_t offset);
    
    // 计算虚拟页号
    static uint32_t getVPN(uint32_t pde_idx, uint32_t pte_idx);
};

#endif // ADDRESS_TRANSLATOR_H