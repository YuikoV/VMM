// 系统配置与全局常量定义
// 用于统一管理虚拟内存系统中的关键参数设置

#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>

/* ================= 页面与地址相关配置 ================= */

// 页面大小：4KB
// 对应页内偏移 12 位（2^12 = 4096）
const uint32_t PAGE_SIZE = 4096;

// 页内偏移位数
// 虚拟地址低 12 位用于表示页内偏移
const uint32_t PAGE_OFFSET_BITS = 12;

// 页表索引位数
// 每个页表包含 2^10 = 1024 个页表项
const uint32_t PAGE_TABLE_BITS = 10;

// 页目录索引位数
// 页目录同样包含 1024 个页目录项
const uint32_t PAGE_DIR_BITS = 10;

/*
 * 32 位虚拟地址结构说明：
 * | 31........22 | 21........12 | 11........0 |
 * |   PDE(10)    |   PTE(10)    | Offset(12)  |
 */


/* ================= 物理内存与交换区配置 ================= */

// 物理内存帧数
// 256 个物理页帧 × 4KB = 1MB 物理内存
const uint32_t PHYSICAL_FRAMES = 256;

// 交换区页面数量
// 1024 个交换页 × 4KB = 4MB 模拟磁盘空间
const uint32_t SWAP_SIZE = 1024;

#endif // CONFIG_H
