#include <iostream>
#include <thread>
#include <chrono>

int flag = 0;

void Producer() {
    // Wait for the consumer to start polling
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // In Release mode, the compiler likely sees flag = 1 as redundant
    // because it is immediately followed by flag = 2.
    // Result: flag = 1 might never exist in memory.
    flag = 1; 
    flag = 2; 

    std::cout << "[Producer] State updated: 0 -> 1 -> 2 (in code)" << std::endl;
}

void Consumer() {
    std::cout << "[Consumer] Started polling, waiting for flag == 1..." << std::endl;
    
    // Timeout counter to prevent infinite loop
    int timeout = 0;
    while (flag != 1) {
        // If flag = 1 was optimized away, we might see flag = 2 directly
        if (flag == 2) {
            std::cout << "[Consumer] ERROR: Missed state 1, directly saw state 2!" << std::endl;
            return;
        }
        
        // Timeout mechanism
        if (++timeout > 500000000) {
            std::cout << "[Consumer] TIMEOUT: Never saw 1 or 2 (likely register cached)" << std::endl;
            return;
        }
    }

    std::cout << "[Consumer] SUCCESS: Caught state 1!" << std::endl;
}

int main() {
    std::thread t1(Producer);
    std::thread t2(Consumer);

    t1.join();
    t2.join();

    return 0;
}
