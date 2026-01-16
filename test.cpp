// 测试程序主文件
#include "vmm.h"
#include <iostream>
#include <cstdlib>

void testSequentialAccess(VirtualMemoryManager& vmm);
void testRandomAccess(VirtualMemoryManager& vmm);
void testLocalityAccess(VirtualMemoryManager& vmm);
void testBoundaryConditions(VirtualMemoryManager& vmm);

int main() {
    std::cout << "========== 虚拟内存管理器测试程序 ==========\n";
    
    VirtualMemoryManager vmm;
    
    testSequentialAccess(vmm);
    testRandomAccess(vmm);
    testLocalityAccess(vmm);
    testBoundaryConditions(vmm);
    
    std::cout << "\n所有测试完成!\n";
    return 0;
}

void testSequentialAccess(VirtualMemoryManager& vmm) {
    std::cout << "\n测试用例1: 顺序访问测试\n";
    vmm.resetStatistics();
    
    for (uint32_t i = 0; i < 1000; i++) {
        vmm.writeByte(i * PAGE_SIZE, (uint8_t)(i % 256));
    }
    
    bool pass = true;
    for (uint32_t i = 0; i < 1000; i++) {
        uint8_t val = vmm.readByte(i * PAGE_SIZE);
        if (val != (uint8_t)(i % 256)) {
            pass = false;
            break;
        }
    }
    
    std::cout << "测试结果: " << (pass ? "通过" : "失败") << std::endl;
    vmm.printStatistics();
}

void testRandomAccess(VirtualMemoryManager& vmm) {
    std::cout << "\n测试用例2: 随机访问测试\n";
    vmm.resetStatistics();
    
    std::srand(42);
    for (int i = 0; i < 5000; i++) {
        uint32_t addr = (std::rand() % 2048) * PAGE_SIZE;
        vmm.writeByte(addr, std::rand() % 256);
    }
    
    vmm.printStatistics();
}

void testLocalityAccess(VirtualMemoryManager& vmm) {
    std::cout << "\n测试用例3: 局部性原理测试\n";
    vmm.resetStatistics();
    
    for (int round = 0; round < 10; round++) {
        for (uint32_t i = 0; i < 100; i++) {
            vmm.readByte(i * PAGE_SIZE);
        }
    }
    
    vmm.printStatistics();
}

void testBoundaryConditions(VirtualMemoryManager& vmm) {
    std::cout << "\n测试用例4: 边界条件测试\n";
    vmm.resetStatistics();
    
    vmm.writeByte(0, 0xFF);
    vmm.writeByte(PAGE_SIZE - 1, 0xAA);
    vmm.writeByte(0xFFFFFFFF, 0x55);
    
    bool pass = (vmm.readByte(0) == 0xFF) && 
                (vmm.readByte(PAGE_SIZE - 1) == 0xAA) &&
                (vmm.readByte(0xFFFFFFFF) == 0x55);
    
    std::cout << "测试结果: " << (pass ? "通过" : "失败") << std::endl;
    vmm.printStatistics();
}