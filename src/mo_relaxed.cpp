/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2025 Vanishing Games. All Rights Reserved.
 * @Author: VanishXiao
 * @Date: 2026-03-18 10:51:05
 * @LastEditTime: 2026-03-18 10:56:27
 * --------------------------------------------------------------------------------
 */
#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> data{0};
std::atomic<bool> ready{false};
std::atomic<int> counter{0};

void producer() {
    // 1. 验证原子性：每个线程加 1000 次
    for (int i = 0; i < 1000; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }

    // 2. 验证可见性顺序：
    // 在 relaxed 模式下，下面两行可能会被编译器或 CPU 重排
    data.store(42, std::memory_order_relaxed); 
    ready.store(true, std::memory_order_relaxed); 
}

void consumer() {
    // 等待标志位
    while (!ready.load(std::memory_order_relaxed));

    // 虽然 ready 已经是 true 了，但由于不保证可见性顺序，
    // 在某些硬件架构（如 ARM）或极其激进的编译器优化下，
    // 这里读到的 data 可能是 0 而不是 42。
    if (data.load(std::memory_order_relaxed) == 0) {
        std::cout << "[Relaxed] GLITCH: Saw ready=true but data=0! (Visibility Order Violation)" << std::endl;
    }
}

int main() {
    std::cout << "Starting Relaxed test..." << std::endl;

    // 运行多次实验
    for (int i = 0; i < 100; ++i) {
        data = 0;
        ready = false;
        counter = 0;

        std::thread t1(producer);
        std::thread t2(consumer);
        t1.join();
        t2.join();

        // 验证原子性
        if (counter.load() != 1000) {
            std::cout << "[Relaxed] Atomicity Error! Counter: " << counter.load() << std::endl;
        }
    }

    std::cout << "Test finished. (On x86, glitches are rare due to TSO hardware, but allowed by C++ standard)" << std::endl;
    
    return 0;
}
