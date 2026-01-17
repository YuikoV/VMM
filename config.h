// 系统配置和常量定义
#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>

const uint32_t PAGE_SIZE = 4096;           // 4KB页面
const uint32_t PAGE_OFFSET_BITS = 12;      // 页内偏移12位
const uint32_t PAGE_TABLE_BITS = 10;       // 页表索引10位
const uint32_t PAGE_DIR_BITS = 10;         // 页目录索引10位
const uint32_t PHYSICAL_FRAMES = 256;      // 物理内存帧数(1MB)
const uint32_t SWAP_SIZE = 1024;           // 交换区大小(4MB)

#endif // CONFIG_H
