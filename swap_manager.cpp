#include "swap_manager.h"
#include <cstring>
#include <iostream>

SwapManager::SwapManager(const std::string& filename)
    : swap_file_name(filename), disk_reads(0), disk_writes(0) {
    std::memset(swap_bitmap, 0, sizeof(swap_bitmap));
}

SwapManager::~SwapManager() {
    if (swap_file.is_open()) {
        swap_file.close();
    }
}

void SwapManager::initialize() {
    swap_file.open(swap_file_name, 
                  std::ios::in | std::ios::out | 
                  std::ios::binary | std::ios::trunc);
    
    if (!swap_file.is_open()) {
        swap_file.open(swap_file_name, std::ios::out | std::ios::binary);
        swap_file.close();
        swap_file.open(swap_file_name, 
                      std::ios::in | std::ios::out | std::ios::binary);
    }
    
    // 预分配交换区空间
    uint8_t zero_page[PAGE_SIZE] = {0};
    for (uint32_t i = 0; i < SWAP_SIZE; i++) {
        swap_file.write((char*)zero_page, PAGE_SIZE);
    }
    swap_file.flush();
}

uint32_t SwapManager::allocateSpace() {
    for (uint32_t i = 0; i < SWAP_SIZE; i++) {
        if (!swap_bitmap[i]) {
            swap_bitmap[i] = true;
            return i;
        }
    }
    return 0;  // 交换区满
}

void SwapManager::freeSpace(uint32_t disk_addr) {
    if (disk_addr < SWAP_SIZE) {
        swap_bitmap[disk_addr] = false;
    }
}

void SwapManager::loadPage(uint8_t* buffer, uint32_t disk_addr) {
    swap_file.seekg(disk_addr * PAGE_SIZE, std::ios::beg);
    swap_file.read((char*)buffer, PAGE_SIZE);
    disk_reads++;
}

void SwapManager::savePage(const uint8_t* buffer, uint32_t disk_addr) {
    swap_file.seekp(disk_addr * PAGE_SIZE, std::ios::beg);
    swap_file.write((char*)buffer, PAGE_SIZE);
    swap_file.flush();
    disk_writes++;
}

void SwapManager::resetStats() {
    disk_reads = 0;
    disk_writes = 0;
}
