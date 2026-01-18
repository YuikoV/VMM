// 磁盘交换区管理模块
// 使用文件模拟操作系统中的交换区（Swap Space）

#ifndef SWAP_MANAGER_H
#define SWAP_MANAGER_H

#include "config.h"
#include <fstream>
#include <string>

/**
 * @class SwapManager
 * @brief 交换区管理类
 *
 * 该类通过文件模拟磁盘交换区，负责页面的换入与换出。
 * 交换区按页管理，每个交换页面大小与内存页大小一致。
 *
 * 使用位图记录交换区页面的占用情况，
 * 并统计磁盘读写次数以支持性能分析。
 */
class SwapManager {
private:
    std::string swap_file_name;   // 交换区文件名
    std::fstream swap_file;       // 交换区文件流

    bool swap_bitmap[SWAP_SIZE];  // 交换区位图，true 表示已占用

    uint64_t disk_reads;          // 磁盘读取次数
    uint64_t disk_writes;         // 磁盘写入次数

public:
    /**
     * @brief 构造函数
     *
     * @param filename 交换区文件名，默认为 "swap.dat"
     */
    SwapManager(const std::string& filename = "swap.dat");

    /**
     * @brief 析构函数
     *
     * 在系统退出时关闭交换区文件。
     */
    ~SwapManager();

    /**
     * @brief 初始化交换区
     *
     * 创建并预分配交换区文件空间，
     * 模拟固定大小的磁盘交换区。
     */
    void initialize();

    /**
     * @brief 分配一个交换区页面
     *
     * @return 交换区页号，若交换区已满则返回 0
     */
    uint32_t allocateSpace();

    /**
     * @brief 释放交换区页面
     *
     * @param disk_addr 交换区页号
     */
    void freeSpace(uint32_t disk_addr);

    /**
     * @brief 从交换区加载页面到内存
     *
     * @param buffer    内存缓冲区
     * @param disk_addr 交换区页号
     */
    void loadPage(uint8_t* buffer, uint32_t disk_addr);

    /**
     * @brief 将页面保存到交换区
     *
     * @param buffer    内存页面数据
     * @param disk_addr 交换区页号
     */
    void savePage(const uint8_t* buffer, uint32_t disk_addr);

    /**
     * @brief 获取磁盘读取次数
     */
    uint64_t getDiskReads() const { return disk_reads; }

    /**
     * @brief 获取磁盘写入次数
     */
    uint64_t getDiskWrites() const { return disk_writes; }

    /**
     * @brief 重置磁盘 I/O 统计信息
     */
    void resetStats();
};

#endif // SWAP_MANAGER_H
