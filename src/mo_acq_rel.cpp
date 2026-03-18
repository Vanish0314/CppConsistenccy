#include <iostream>
#include <atomic>
#include <thread>
#include <assert.h>

/*
 * --------------------------------------------------------------------------------
 * 强力演示：Acquire-Release 的传递性 (Transitivity)
 * 线程 1 (生产者) -> 线程 2 (接力者) -> 线程 3 (消费者)
 * --------------------------------------------------------------------------------
 */

int data = 0;
std::atomic<bool> sync1{false};
std::atomic<bool> sync2{false};

// 线程 1：生产数据并发出第一个信号
void thread1() {
    data = 42; // (A)
    // release: 确保 (A) 不会重排到 store 之后
    sync1.store(true, std::memory_order_release); 
    std::cout << "[Thread 1] Data set to 42 and signaled sync1." << std::endl;
}

// 线程 2：接收第一个信号，发出第二个信号（接力）
void thread2() {
    // acquire: 确保 (B) 之后的指令不重排到 load 之前
    while (!sync1.load(std::memory_order_acquire)); // (B)
    
    // 此时，线程 2 已经观察到了 data = 42。
    // 它通过 acq_rel 语义将这个可见性继续传递下去。
    sync2.store(true, std::memory_order_release); // (C)
    std::cout << "[Thread 2] Received sync1, signaled sync2." << std::endl;
}

// 线程 3：接收第二个信号，验证数据
void thread3() {
    // acquire: 与线程 2 的 release 匹配
    while (!sync2.load(std::memory_order_acquire)); // (D)

    // 强力证明：虽然线程 3 从未直接与线程 1 同步，
    // 但由于线程 2 充当了“接力者”，同步链条为：
    // Thread 1 (A) --release--> Thread 2 (B) --release--> Thread 3 (D)
    // 这种传递性保证了线程 3 看到的 data 一定是 42。
    if (data == 42) {
        std::cout << "[Thread 3] SUCCESS: Data 42 observed via transitive synchronization!" << std::endl;
    } else {
        std::cout << "[Thread 3] ERROR: Data visibility failed!" << std::endl;
    }
}

int main() {
    std::thread t3(thread3);
    std::thread t2(thread2);
    std::thread t1(thread1);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
