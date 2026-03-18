#include <iostream>
#include <thread>

int x = 0;
int y = 0;
int r1 = 0;
int r2 = 0;

void thread1() {
    x = 1;          // Store X
    // --- 编译器或 CPU 可能在这里插入重排 ---
    r1 = y;         // Load Y
}

void thread2() {
    y = 1;          // Store Y
    // --- 编译器或 CPU 可能在这里插入重排 ---
    r2 = x;         // Load X
}

int main() {
    int iterations = 0;
    int detected_count = 0;

    std::cout << "Starting Memory Consistency Experiment..." << std::endl;

    // 运行多次实验以捕捉“不可能”的瞬间
    while (true) {
        iterations++;
        x = 0; y = 0; r1 = 0; r2 = 0;

        // 使用两个线程同时运行
        std::thread t1(thread1);
        std::thread t2(thread2);

        t1.join();
        t2.join();

        // 检查是否发生了违反“顺序一致性”的情况
        // 理论上，如果 x=1 发生在 r2=x 之前，或者 y=1 发生在 r1=y 之前，
        // 那么 r1 和 r2 就不可能同时为 0。
        if (r1 == 0 && r2 == 0) {
            detected_count++;
            std::cout << "Iteration " << iterations 
                      << ": [CONSISTENCY VIOLATION] r1=0, r2=0 detected!" << std::endl;
            
            if (detected_count >= 5) break; // 抓到 5 次就停止
        }

        if (iterations > 1000000) {
            std::cout << "Executed 1,000,000 times, no violation detected (Luck or strong hardware)." << std::endl;
            break;
        }
    }

    return 0;
}
