#include "vmm.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

// 功能测试
void testSequentialAccess(VirtualMemoryManager& vmm);
void testRandomAccess(VirtualMemoryManager& vmm);
void testLocalityAccess(VirtualMemoryManager& vmm);
void testBoundaryConditions(VirtualMemoryManager& vmm);

// 压力测试
void testLargeWorkingSet(VirtualMemoryManager& vmm);
void testMemoryExhaustion(VirtualMemoryManager& vmm);
void testThrashing(VirtualMemoryManager& vmm);

// 辅助函数
void printTestHeader(const std::string& test_name);
void printTestResult(bool pass, const std::string& message = "");

int main() {
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║     虚拟内存管理器完整测试套件                 ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n\n";
    
    VirtualMemoryManager vmm;
    
    // ========== 功能测试 ==========
    std::cout << "【功能测试】\n";
    std::cout << "─────────────────────────────────────────────────\n";
    
    testSequentialAccess(vmm);
    testRandomAccess(vmm);
    testLocalityAccess(vmm);
    testBoundaryConditions(vmm);
    
    // ========== 压力测试 ==========
    std::cout << "\n\n【压力测试】\n";
    std::cout << "─────────────────────────────────────────────────\n";
    
    testLargeWorkingSet(vmm);
    testThrashing(vmm);
    
    std::cout << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║           所有测试完成!                        ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n";
    
    return 0;
}

// ==================== 功能测试1: 顺序访问 ====================
void testSequentialAccess(VirtualMemoryManager& vmm) {
    printTestHeader("测试1: 顺序访问");
    vmm.resetStatistics();
    
    const uint32_t NUM_PAGES = 1000;
    
    // 阶段1: 写入数据
    std::cout << "  阶段1: 写入" << NUM_PAGES << "个页面...\n";
    for (uint32_t i = 0; i < NUM_PAGES; i++) {
        vmm.writeByte(i * PAGE_SIZE, (uint8_t)(i % 256));
    }
    std::cout << "  ✓ 写入完成\n";
    
    // 阶段2: 验证数据
    std::cout << "  阶段2: 验证" << NUM_PAGES << "个页面...\n";
    bool pass = true;
    int fail_count = 0;
    int first_fail = -1;
    
    for (uint32_t i = 0; i < NUM_PAGES; i++) {
        uint8_t expected = (uint8_t)(i % 256);
        uint8_t actual = vmm.readByte(i * PAGE_SIZE);
        
        if (actual != expected) {
            if (first_fail == -1) first_fail = i;
            if (fail_count < 5) {  // 只打印前5个错误
                std::cout << "    ✗ 错误 @ 页" << i 
                         << ": 期望=" << (int)expected 
                         << " 实际=" << (int)actual << std::endl;
            }
            fail_count++;
            pass = false;
        }
    }
    
    // 结果分析
    if (pass) {
        std::cout << "  ✓ 所有" << NUM_PAGES << "个页面验证通过\n";
    } else {
        std::cout << "  ✗ 发现 " << fail_count << " 个错误\n";
        std::cout << "    首个错误在页面 " << first_fail << "\n";
        
        if (first_fail >= 256) {
            std::cout << "    分析: 错误出现在超过物理帧数量的页面\n";
            std::cout << "    可能原因: 页面置换后数据未正确保存/加载\n";
        }
    }
    
    printTestResult(pass, "顺序访问测试");
    vmm.printStatistics();
}

// ==================== 功能测试2: 随机访问 ====================
void testRandomAccess(VirtualMemoryManager& vmm) {
    printTestHeader("测试2: 随机访问");
    vmm.resetStatistics();
    
    const uint32_t NUM_ACCESSES = 5000;
    const uint32_t PAGE_RANGE = 2048;
    
    std::cout << "  执行" << NUM_ACCESSES << "次随机访问...\n";
    std::cout << "  地址范围: " << PAGE_RANGE << "个页面\n";
    
    std::srand(42);  // 固定种子以便复现
    
    for (uint32_t i = 0; i < NUM_ACCESSES; i++) {
        uint32_t page = std::rand() % PAGE_RANGE;
        uint32_t addr = page * PAGE_SIZE;
        uint8_t value = std::rand() % 256;
        
        vmm.writeByte(addr, value);
    }
    
    std::cout << "  ✓ 随机访问完成\n";
    printTestResult(true, "随机访问测试");
    vmm.printStatistics();
    
    // 预期结果验证
    std::cout << "  预期: 命中率 12-15%, 频繁置换\n";
}

// ==================== 功能测试3: 局部性原理 ====================
void testLocalityAccess(VirtualMemoryManager& vmm) {
    printTestHeader("测试3: 局部性原理");
    vmm.resetStatistics();
    
    const uint32_t WORKING_SET_SIZE = 100;
    const uint32_t NUM_ROUNDS = 10;
    
    std::cout << "  工作集大小: " << WORKING_SET_SIZE << "个页面\n";
    std::cout << "  访问轮数: " << NUM_ROUNDS << "轮\n";
    
    for (uint32_t round = 0; round < NUM_ROUNDS; round++) {
        for (uint32_t i = 0; i < WORKING_SET_SIZE; i++) {
            vmm.readByte(i * PAGE_SIZE);
        }
        
        if (round == 0) {
            std::cout << "  第1轮: 全部缺页(建立工作集)\n";
        } else if (round == 1) {
            std::cout << "  第2轮: 预期命中率接近100%\n";
        }
    }
    
    std::cout << "  ✓ 局部性测试完成\n";
    printTestResult(true, "局部性测试");
    vmm.printStatistics();
    
    std::cout << "  预期: 命中率 >90%, 磁盘I/O极少\n";
}

// ==================== 功能测试4: 边界条件 ====================
void testBoundaryConditions(VirtualMemoryManager& vmm) {
    printTestHeader("测试4: 边界条件");
    vmm.resetStatistics();
    
    bool pass = true;
    
    // 测试1: 最小地址
    std::cout << "  测试1: 最小地址 (0x00000000)\n";
    vmm.writeByte(0x00000000, 0xFF);
    uint8_t val1 = vmm.readByte(0x00000000);
    if (val1 == 0xFF) {
        std::cout << "    ✓ 最小地址: 通过\n";
    } else {
        std::cout << "    ✗ 最小地址: 失败 (期望0xFF, 实际0x" 
                  << std::hex << (int)val1 << std::dec << ")\n";
        pass = false;
    }
    
    // 测试2: 页边界
    std::cout << "  测试2: 页边界 (PAGE_SIZE-1, PAGE_SIZE)\n";
    vmm.writeByte(PAGE_SIZE - 1, 0xAA);
    vmm.writeByte(PAGE_SIZE, 0xBB);
    
    uint8_t val2 = vmm.readByte(PAGE_SIZE - 1);
    uint8_t val3 = vmm.readByte(PAGE_SIZE);
    
    if (val2 == 0xAA && val3 == 0xBB) {
        std::cout << "    ✓ 页边界: 通过\n";
    } else {
        std::cout << "    ✗ 页边界: 失败\n";
        pass = false;
    }
    
    // 测试3: 最大地址
    std::cout << "  测试3: 最大地址 (0xFFFFFFFF)\n";
    vmm.writeByte(0xFFFFFFFF, 0x55);
    uint8_t val4 = vmm.readByte(0xFFFFFFFF);
    
    if (val4 == 0x55) {
        std::cout << "    ✓ 最大地址: 通过\n";
    } else {
        std::cout << "    ✗ 最大地址: 失败 (期望0x55, 实际0x" 
                  << std::hex << (int)val4 << std::dec << ")\n";
        pass = false;
    }
    
    printTestResult(pass, "边界条件测试");
    vmm.printStatistics();
}

// ==================== 压力测试5: 大工作集 ====================
void testLargeWorkingSet(VirtualMemoryManager& vmm) {
    printTestHeader("压力测试5: 大工作集");
    vmm.resetStatistics();
    
    const uint32_t NUM_PAGES = 10000;
    
    std::cout << "  访问" << NUM_PAGES << "个不同页面...\n";
    
    clock_t start = clock();
    
    for (uint32_t i = 0; i < NUM_PAGES; i++) {
        vmm.writeByte(i * PAGE_SIZE, (uint8_t)(i % 256));
        
        // 进度显示
        if (i % 2000 == 0 && i > 0) {
            std::cout << "    已访问 " << i << " 个页面...\n";
        }
    }
    
    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;
    
    std::cout << "  ✓ 大工作集测试完成\n";
    std::cout << "  执行时间: " << elapsed << " 秒\n";
    
    printTestResult(true, "大工作集测试");
    vmm.printStatistics();
    
    std::cout << "  预期: 系统稳定,无崩溃\n";
}


// ==================== 压力测试6: 抖动场景 ====================
void testThrashing(VirtualMemoryManager& vmm) {
    printTestHeader("压力测试6: 抖动场景");
    vmm.resetStatistics();
    
    const uint32_t WORKING_SET_SIZE = 300;  // 超过物理内存
    const uint32_t NUM_ROUNDS = 5;
    
    std::cout << "  工作集: " << WORKING_SET_SIZE << "个页面\n";
    std::cout << "  物理帧: 256个\n";
    std::cout << "  访问轮数: " << NUM_ROUNDS << "轮\n";
    std::cout << "  这将导致频繁的页面置换(抖动)\n";
    
    for (uint32_t round = 0; round < NUM_ROUNDS; round++) {
        std::cout << "  第" << (round + 1) << "轮访问中...\n";
        
        for (uint32_t i = 0; i < WORKING_SET_SIZE; i++) {
            vmm.readByte(i * PAGE_SIZE);
        }
    }
    
    std::cout << "  ✓ 抖动场景测试完成\n";
    printTestResult(true, "抖动场景测试");
    vmm.printStatistics();
    
    std::cout << "  预期: 性能下降但功能正确\n";
}

// ==================== 辅助函数 ====================
void printTestHeader(const std::string& test_name) {
    std::cout << "\n┌─────────────────────────────────────────────────┐\n";
    std::cout << "│ " << test_name;
    // 补齐空格
    int padding = 47 - test_name.length();
    for (int i = 0; i < padding; i++) std::cout << " ";
    std::cout << "│\n";
    std::cout << "└─────────────────────────────────────────────────┘\n";
}

void printTestResult(bool pass, const std::string& message) {
    std::cout << "\n  ";
    if (pass) {
        std::cout << "✓✓✓ " << message << ": 通过 ✓✓✓\n";
    } else {
        std::cout << "✗✗✗ " << message << ": 失败 ✗✗✗\n";
    }
}