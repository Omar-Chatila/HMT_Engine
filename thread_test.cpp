#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

// Declare an atomic flag to signal when the computation is done
std::atomic<bool> is_done{false};

// Function to be run in a separate thread
void longComputation() {
    // Simulate a long computation
    std::this_thread::sleep_for(std::chrono::seconds(5));
    is_done = true;
}

int main() {
    // Start the computation in a separate thread
    std::thread computationThread(longComputation);

    // Simulate loading animation in the main thread
    while (!is_done) {
        std::cout << ".";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Ensure the computation thread has finished
    if (computationThread.joinable()) {
        computationThread.join();
    }

    std::cout << "\nComputation done!" << std::endl;

    return 0;
}
