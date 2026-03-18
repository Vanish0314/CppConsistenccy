/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2025 Vanishing Games. All Rights Reserved.
 * @Author: VanishXiao
 * @Date: 2026-03-18 11:11:07
 * @LastEditTime: 2026-03-18 11:19:08
 * --------------------------------------------------------------------------------
 */
#include <iostream>
#include <atomic>
#include <thread>
#include <string>


struct Config {
    int update_id;
    std::string name;
};

std::atomic<Config*> g_config_ptr{nullptr};
int g_unrelated_global = 0;

void producer() {
    // 准备数据
    Config* new_cfg = new Config{1, "RCU_Config_V1"};
    
    // (A) 修改一个完全不相关的变量
    g_unrelated_global = 100; 

    // (B) 使用 release 发布指针
    // release 确保 (A) 和 new_cfg 的初始化在 store 之前完成
    g_config_ptr.store(new_cfg, std::memory_order_release);
    
    std::cout << "[Producer] Config published." << std::endl;
}

void consumer() {
    Config* cfg;
    // (C) 使用 consume 加载指针
    while (!(cfg = g_config_ptr.load(std::memory_order_consume)));

    // 下面两行代码通过 cfg 指针访问成员，与 cfg 有“数据依赖”。
    // C++ 标准保证：通过 consume 同步后的指针访问其成员，一定能看到最新值。
    std::cout << "[Consumer] Dependent Data: ID=" << cfg->update_id 
              << ", Name=" << cfg->name << std::endl;

    // g_unrelated_global 与 cfg 指针没有任何数据依赖。
    // 虽然生产者先写了它，但由于这里使用的是 consume 而非 acquire，
    // 在底层语义上，并不保证这里一定能看到 100。
    // 在 ARM 架构上，这里的读取可能被重排到 (C) 之前，读到旧值 0。
    std::cout << "[Consumer] Unrelated Data (No Dependency): " << g_unrelated_global << std::endl;

    // 注意：在 x86-64 (TSO) 或 MSVC/GCC 下，consume 通常会被自动提升为 acquire，
    // 且硬件本身保证了 Load-Load 顺序，因此你很难在 X86 PC 上观察到 g_unrelated_global 为 0。
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    return 0;
}
