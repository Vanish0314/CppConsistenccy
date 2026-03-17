#include <iostream>
#include <thread>
#include <atomic>

int x = 0;
bool cond = false; 
std::atomic<bool> done{false};

void source_producer() {
    for (int i = 0; i < 10000000; ++i) {
        if (cond) {
            x = 42;
        }
    }
}

void consumer() {
    int detected_glitch = 0;
    while (!done) {
        // cond 始终为 false，理论上 x 永远为 0
        if (!cond && x == 42) {
            detected_glitch++;
        }
    }
    if (detected_glitch > 0) {
        std::cout << "[Consumer] DETECTED: Saw x == 42 while cond was FALSE! Count: " << detected_glitch << std::endl;
    } else {
        std::cout << "[Consumer] Safe: No glitch detected with simple 'if (cond) x = 42;'" << std::endl;
    }
}

int main() {
    std::cout << "[Main] Starting experiment with simple 'if (cond) x = 42;'..." << std::endl;
    
    cond = false; 
    x = 0;

    std::thread t2(consumer);
    std::thread t1(source_producer);

    t1.join();
    done = true;
    t2.join();

    return 0;
}
