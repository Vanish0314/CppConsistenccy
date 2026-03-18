/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2025 Vanishing Games. All Rights Reserved.
 * @Author: VanishXiao
 * @Date: 2026-03-18 10:52:28
 * @LastEditTime: 2026-03-18 11:01:11
 * --------------------------------------------------------------------------------
 */
#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> x{0};
std::atomic<int> y{0};
int r1 = 0;
int r2 = 0;

void thread1() {
    x.store(1, std::memory_order_seq_cst); // Store X
    r1 = y.load(std::memory_order_seq_cst); // Load Y
}

void thread2() {
    y.store(1, std::memory_order_seq_cst); // Store Y
    r2 = x.load(std::memory_order_seq_cst); // Load X
}

int main() {
    int iterations = 0;
    int violation_count = 0;

    std::cout << "[Seq_Cst] Starting experiment. This will run 500,000 times..." << std::endl;

    for (int i = 0; i < 500000; ++i) {
        x = 0; y = 0; r1 = 0; r2 = 0;

        std::thread t1(thread1);
        std::thread t2(thread2);
        t1.join();
        t2.join();

        // 在 seq_cst 下，r1=0 且 r2=0 是绝对不可能发生的
        if (r1 == 0 && r2 == 0) {
            violation_count++;
        }
    }

    std::cout << "[Seq_Cst] Completed 500,000 iterations." << std::endl;
    std::cout << "[Seq_Cst] Violations (r1=0, r2=0) detected: " << violation_count << std::endl;

    return 0;
}
