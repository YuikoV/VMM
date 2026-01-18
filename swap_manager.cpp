#include "swap_manager.h"
#include <cstring>
#include <iostream>

/**
 * @brief 构造函数
 *
 * @param filename 用于模拟交换区的文件名
 *
 * 初始化交换区文件名及磁盘 I/O 统计信息，
 * 并清空交换区位图。
 */
SwapManager::SwapManager(const std::string& filename)
    : swap_file_name(filename),
      disk_reads(0),
      disk_writes(0) {

    // 初始化交换区位图，false 表示未占用
    std::memset(swap_bitmap, 0, sizeof(swap_bitmap));
}

/**
 * @brief 析构函数
 *
 * 在系统退出时关闭交换区文件。
 */
SwapManager::~SwapManager() {
    if (swap_file.is_open()) {
        swap_file.close();
    }
}

/**
 * @brief 初始化交换区
 *
 * 创建（或重新创建）交换区文件，
 * 并预分配固定大小的磁盘空间，
 * 用于模拟操作系统中的交换区。
 */
void SwapManager::initialize() {
    swap_file.open(swap_file_name,
                   std::ios::in | std::ios::out |
                   std::ios::binary | std::ios::trunc);

    // 若文件打开失败，尝试重新创建
    if (!swap_file.is_open()) {
        swap_file.open(swap_file_name, std::ios::out | std::ios::binary);
        swap_file.close();
        swap_file.open(swap_file_name,
                       std::ios::in | std::ios::out | std::ios::binary);
    }

    // 预分配交换区空间（按页写入零数据）
    uint8_t zero_page[PAGE_SIZE] = {0};
    for (uint32_t i = 0; i < SWAP_SIZE; i++) {
        swap_file.write(reinterpret_cast<char*>(zero_page), PAGE_SIZE);
    }
    swap_file.flush();
}

/**
 * @brief 在交换区中分配一个页面空间
 *
 * @return 分配到的交换区页号
 *
 * 通过位图顺序扫描的方式查找空闲交换页，
 * 若交换区已满则返回 0。
 */
uint32_t SwapManager::allocateSpace() {
    for (uint32_t i = 0; i < SWAP_SIZE; i++) {
        if (!swap_bitmap[i]) {
            swap_bitmap[i] = true;
            return i;
        }
    }
    return 0;  // 交换区已满
}

/**
 * @brief 释放交换区空间
 *
 * @param disk_addr 交换区页号
 */
void SwapManager::freeSpace(uint32_t disk_addr) {
    if (disk_addr < SWAP_SIZE) {
        swap_bitmap[disk_addr] = false;
    }
}

/**
 * @brief 从交换区读取页面到内存
 *
 * @param buffer    内存缓冲区
 * @param disk_addr 交换区页号
 */
void SwapManager::loadPage(uint8_t* buffer, uint32_t disk_addr) {
    swap_file.seekg(disk_addr * PAGE_SIZE, std::ios::beg);
    swap_file.read(reinterpret_cast<char*>(buffer), PAGE_SIZE);
    disk_reads++;
}

/**
 * @brief 将内存页面写回交换区
 *
 * @param buffer    内存页面数据
 * @param disk_addr 交换区页号
 */
void SwapManager::savePage(const uint8_t* buffer, uint32_t disk_addr) {
    swap_file.seekp(disk_addr * PAGE_SIZE, std::ios::beg);
    swap_file.write(reinterpret_cast<const char*>(buffer), PAGE_SIZE);
    swap_file.flush();
    disk_writes++;
}

/**
 * @brief 重置磁盘 I/O 统计信息
 *
 * 用于多轮测试场景。
 */
void SwapManager::resetStats() {
    disk_reads = 0;
    disk_writes = 0;
}
