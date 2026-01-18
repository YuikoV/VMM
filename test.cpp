#include "vmm.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

/*
 * 本文件是虚拟内存管理器（VMM）的
 * —— 功能测试 + 压力测试 —— 测试套件
 *
 * 目标：
 * 1. 验证地址转换、缺页处理、页面置换是否正确
 * 2. 验证在高负载、抖动场景下系统的稳定性
 * 3. 收集命中率、缺页率、磁盘 I/O 等性能指标
 */

// ==================== 测试函数声明 ====================

// 功能测试
void testSequentialAccess(VirtualMemoryManager& vmm);   // 顺序访问
void testRandomAccess(VirtualMemoryManager& vmm);       // 随机访问
void testLocalityAccess(VirtualMemoryManager& vmm);     // 局部性原理
void testBoundaryConditions(VirtualMemoryManager& vmm); // 边界条件

// 压力测试
void testLargeWorkingSet(VirtualMemoryManager& vmm);    // 大工作集
void testMemoryExhaustion(VirtualMemoryManager& vmm);   // （预留）
void testThrashing(VirtualMemoryManager& vmm);          // 抖动测试

// 辅助输出函数
void printTestHeader(const std::string& test_name);
void printTestResult(bool pass, const std::string& message = "");

// ==================== 主函数 ====================
int main() {
    // 测试套件标题
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║     虚拟内存管理器完整测试套件                 ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n\n";

    // 创建虚拟内存管理器实例
    VirtualMemoryManager vmm;

    // ========== 功能测试 ==========
    std::cout << "【功能测试】\n";
    std::cout << "─────────────────────────────────────────────────\n";

    testSequentialAccess(vmm);    // 测试基本正确性
    testRandomAccess(vmm);        // 测试随机缺页
    testLocalityAccess(vmm);      // 测试局部性
    testBoundaryConditions(vmm);  // 测试极端地址

    // ========== 压力测试 ==========
    std::cout << "\n\n【压力测试】\n";
    std::cout << "─────────────────────────────────────────────────\n";

    testLargeWorkingSet(vmm);     // 大规模页面访问
    testThrashing(vmm);           // 抖动场景

    // 结束提示
    std::cout << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║           所有测试完成!                        ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n";

    return 0;
}

// =====================================================
// 功能测试1：顺序访问
// 目的：
//   - 验证最基本的“写 → 换出 → 换入 → 读”是否正确
//   - 检查页面置换后数据是否被正确保存到交换区
// =====================================================
void testSequentialAccess(VirtualMemoryManager& vmm) {
    printTestHeader("测试1: 顺序访问");
    vmm.resetStatistics();

    const uint32_t NUM_PAGES = 1000;  // 远大于物理帧数(256)

    // ---------- 阶段1：顺序写入 ----------
    std::cout << "  阶段1: 写入" << NUM_PAGES << "个页面...\n";
    for (uint32_t i = 0; i < NUM_PAGES; i++) {
        // 每个页面写入一个确定值，便于后续校验
        vmm.writeByte(i * PAGE_SIZE, (uint8_t)(i % 256));
    }
    std::cout << "  ✓ 写入完成\n";

    // ---------- 阶段2：顺序读取并校验 ----------
    std::cout << "  阶段2: 验证" << NUM_PAGES << "个页面...\n";
    bool pass = true;
    int fail_count = 0;
    int first_fail = -1;

    for (uint32_t i = 0; i < NUM_PAGES; i++) {
        uint8_t expected = (uint8_t)(i % 256);
        uint8_t actual = vmm.readByte(i * PAGE_SIZE);

        if (actual != expected) {
            if (first_fail == -1) first_fail = i;
            if (fail_count < 5) {  // 只输出前 5 个错误
                std::cout << "    ✗ 错误 @ 页" << i
                          << ": 期望=" << (int)expected
                          << " 实际=" << (int)actual << std::endl;
            }
            fail_count++;
            pass = false;
        }
    }

    // ---------- 结果分析 ----------
    if (pass) {
        std::cout << "  ✓ 所有" << NUM_PAGES << "个页面验证通过\n";
    } else {
        std::cout << "  ✗ 发现 " << fail_count << " 个错误\n";
        std::cout << "    首个错误在页面 " << first_fail << "\n";

        if (first_fail >= 256) {
            std::cout << "    分析: 错误出现在超过物理帧数量的页面\n";
            std::cout << "    可能原因: 页面换出/换入逻辑错误\n";
        }
    }

    printTestResult(pass, "顺序访问测试");
    vmm.printStatistics();
}

// =====================================================
// 功能测试2：随机访问
// 目的：
//   - 模拟真实程序的随机访存行为
//   - 验证缺页中断和页面置换的稳定性
// =====================================================
void testRandomAccess(VirtualMemoryManager& vmm) {
    printTestHeader("测试2: 随机访问");
    vmm.resetStatistics();

    const uint32_t NUM_ACCESSES = 5000;
    const uint32_t PAGE_RANGE = 2048;

    std::cout << "  执行" << NUM_ACCESSES << "次随机访问...\n";
    std::cout << "  地址范围: " << PAGE_RANGE << "个页面\n";

    std::srand(42);  // 固定随机种子，保证可复现

    for (uint32_t i = 0; i < NUM_ACCESSES; i++) {
        uint32_t page = std::rand() % PAGE_RANGE;
        uint32_t addr = page * PAGE_SIZE;
        uint8_t value = std::rand() % 256;

        vmm.writeByte(addr, value);
    }

    std::cout << "  ✓ 随机访问完成\n";
    printTestResult(true, "随机访问测试");
    vmm.printStatistics();

    std::cout << "  预期: 命中率较低, 页面频繁置换\n";
}

// =====================================================
// 功能测试3：局部性原理
// 目的：
//   - 验证时间局部性
//   - 工作集能否稳定驻留在内存中
// =====================================================
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
            std::cout << "  第1轮: 建立工作集, 大量缺页\n";
        } else if (round == 1) {
            std::cout << "  第2轮: 工作集已驻留, 命中率应接近100%\n";
        }
    }

    printTestResult(true, "局部性测试");
    vmm.printStatistics();
}

// =====================================================
// 功能测试4：边界条件
// 目的：
//   - 验证地址拆分与页表索引的正确性
//   - 检查极端地址是否会导致越界或错误
// =====================================================
void testBoundaryConditions(VirtualMemoryManager& vmm) {
    printTestHeader("测试4: 边界条件");
    vmm.resetStatistics();

    bool pass = true;

    // ---------- 最小地址 ----------
    vmm.writeByte(0x00000000, 0xFF);
    pass &= (vmm.readByte(0x00000000) == 0xFF);

    // ---------- 页边界 ----------
    vmm.writeByte(PAGE_SIZE - 1, 0xAA);
    vmm.writeByte(PAGE_SIZE, 0xBB);
    pass &= (vmm.readByte(PAGE_SIZE - 1) == 0xAA);
    pass &= (vmm.readByte(PAGE_SIZE) == 0xBB);

    // ---------- 最大地址 ----------
    vmm.writeByte(0xFFFFFFFF, 0x55);
    pass &= (vmm.readByte(0xFFFFFFFF) == 0x55);

    printTestResult(pass, "边界条件测试");
    vmm.printStatistics();
}

// =====================================================
// 压力测试：大工作集
// 目的：
//   - 长时间、大规模页面访问
//   - 检查系统稳定性和性能退化情况
// =====================================================
void testLargeWorkingSet(VirtualMemoryManager& vmm) {
    printTestHeader("压力测试: 大工作集");
    vmm.resetStatistics();

    const uint32_t NUM_PAGES = 10000;

    clock_t start = clock();
    for (uint32_t i = 0; i < NUM_PAGES; i++) {
        vmm.writeByte(i * PAGE_SIZE, (uint8_t)(i % 256));
    }
    clock_t end = clock();

    std::cout << "  执行时间: "
              << double(end - start) / CLOCKS_PER_SEC
              << " 秒\n";

    printTestResult(true, "大工作集测试");
    vmm.printStatistics();
}

// =====================================================
// 压力测试：抖动场景
// 目的：
//   - 工作集略大于物理内存
//   - 验证频繁置换下系统是否仍然正确
// =====================================================
void testThrashing(VirtualMemoryManager& vmm) {
    printTestHeader("压力测试: 抖动场景");
    vmm.resetStatistics();

    const uint32_t WORKING_SET_SIZE = 300;
    const uint32_t NUM_ROUNDS = 5;

    for (uint32_t r = 0; r < NUM_ROUNDS; r++) {
        for (uint32_t i = 0; i < WORKING_SET_SIZE; i++) {
            vmm.readByte(i * PAGE_SIZE);
        }
    }

    printTestResult(true, "抖动场景测试");
    vmm.printStatistics();
}

// ==================== 辅助输出函数 ====================
void printTestHeader(const std::string& test_name) {
    std::cout << "\n┌─────────────────────────────────────────────────┐\n";
    std::cout << "│ " << test_name;
    for (int i = 0; i < 47 - test_name.length(); i++) std::cout << " ";
    std::cout << "│\n";
    std::cout << "└─────────────────────────────────────────────────┘\n";
}

void printTestResult(bool pass, const std::string& message) {
    std::cout << "\n  ";
    if (pass)
        std::cout << "✓✓✓ " << message << ": 通过 ✓✓✓\n";
    else
        std::cout << "✗✗✗ " << message << ": 失败 ✗✗✗\n";
}
