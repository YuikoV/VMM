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
    
    // 阶段1: 写入数据
    std::cout << "阶段1: 写入1000个页面...\n";
    for (uint32_t i = 0; i < 1000; i++) {
        vmm.writeByte(i * PAGE_SIZE, (uint8_t)(i % 256));
    }
    std::cout << "写入完成\n";
    
    // 阶段2: 验证数据
    std::cout << "阶段2: 验证1000个页面...\n";
    bool pass = true;
    int fail_count = 0;
    int first_fail = -1;
    
    for (uint32_t i = 0; i < 1000; i++) {
        uint8_t expected = (uint8_t)(i % 256);
        uint8_t actual = vmm.readByte(i * PAGE_SIZE);
        if (actual != expected) {
            if (first_fail == -1) first_fail = i;
            if (fail_count < 10) {  // 打印前10个错误
                std::cout << "  错误 @ 页" << i << ": 期望=" << (int)expected 
                         << " 实际=" << (int)actual << std::endl;
            }
            fail_count++;
            pass = false;
        }
    }
    
    if (!pass) {
        std::cout << "\n发现 " << fail_count << " 个错误\n";
        std::cout << "第一个错误在页面 " << first_fail << "\n";
        
        // 检查是否是连续的错误
        if (first_fail >= 256) {
            std::cout << "分析: 错误出现在第" << first_fail 
                     << "个页面,已超过256个物理帧\n";
            std::cout << "可能原因: 页面置换后数据未正确保存/加载\n";
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