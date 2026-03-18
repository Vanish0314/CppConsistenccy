#include <iostream>
#include <atomic>
#include <thread>
#include <string>

std::atomic<bool> ready{false};
std::string data;

void producer() {
    data = "Payload delivered safely!"; // (1) 写普通数据
    // release: 严禁 (1) 被重排到 store 之后
    // 保证之前的写入对 acquire 它的线程可见
    ready.store(true, std::memory_order_release);
    std::cout << "[Producer] Flag set to ready." << std::endl;
}

void consumer() {
    // acquire: 严禁下面的 (2) 被重排到 load 之前
    while (!ready.load(std::memory_order_acquire));
    
    // (2) 读普通数据
    std::cout << "[Consumer] Data observed: " << data << std::endl;
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    return 0;
}
