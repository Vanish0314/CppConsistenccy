/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2025 Vanishing Games. All Rights Reserved.
 * @Author: VanishXiao
 * @Date: 2026-03-18 11:11:19
 * @LastEditTime: 2026-03-18 11:20:23
 * --------------------------------------------------------------------------------
 */
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

std::atomic<int> turn{0};
int shared_data = 0;

void worker(int id) {
    // 线程通过 acq_rel 操作参与接力
    // 逻辑：读 turn (获取同步)，加 1，写 turn (发布同步)
    while (turn.fetch_add(0, std::memory_order_acquire) != id);

    // 此时已经获得“同步”
    shared_data += id;
    std::cout << "[Worker " << id << "] Accessing shared_data. Current: " << shared_data << std::endl;

    // 移交接力棒给下一个 ID
    turn.store(id + 1, std::memory_order_release);
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(worker, i);
    }
    for (auto& t : threads) t.join();

    std::cout << "Final shared_data: " << shared_data << std::endl;
    return 0;
}
