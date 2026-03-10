// Timely_TestSuite.cpp
// 
// Version 0.0.1 built 20250706
//
// Test Suite of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include <random>

#include "timely"

// Throw Exception
#ifndef test_throwexception
#define test_throwexception    TIMELY_TRY \
                               throw std::runtime_error("Intentional Exception"); \
                               TIMELY_CATCH
#endif 

// test
static unsigned long long gen_rand() {
    TIMELIES_;
    return std::random_device()();
}

static void sleep_for(int time) {
    TIMELIES_;
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

static auto a_tiny_function(auto x) {
    TIMELIES_;
    return x + 1;
}

static double calc_something() {
    TIMELIES_;

    double cals = 0;
    for (size_t i = 0; i < 124724012ULL; ++i) {
        if (i % 255 == 0) {
            cals += gen_rand();
        }
        else if(i % 1024 == 0) {
            sleep_for(0);
        }
        else {
            cals += i;
        }
    }

    return cals;
}

int main() {

    TIMELIES_;

    Timelyc x("int main(void) coverage");

    // Set project name
    TIMELY_PRJNAME("Test Suite Project");

    // Take an initial value
    std::atomic<double> cals = static_cast<double>(gen_rand());

    // Define some threads
    std::vector<std::thread> thds;
    constexpr size_t n_threads = 1;

    // 注意，线程安全可能有问题，
    // 有线程访问了错误的线程下标，目前的bug可能是这个原因
    // 我不敢确定原因，我感觉是多线程下标问题，或者是多线程线程映射问题

    // Define the generic task
    auto task = [&]() {
        TIMELIES("auto lambda task(void)");
        cals.fetch_add(a_tiny_function(calc_something()));
        return;
    };
    
    // Initialize some threads doing the tasks
    auto init = [&]() {
        TIMELIES("auto lambda init(void)");
        for (size_t i = 0; i < n_threads; ++i) {
            thds.push_back(std::thread(task));
        }
        return;
    };
    init();

    // Do something in Main Thread
    task();
    
    // Stop threads
    auto wait = [&]() {
        TIMELIES("auto lambda wait(void)");
        for (size_t i = 0; i < n_threads; ++i) {
            thds[i].join();
        }
        return;
    };
    wait();

    // Output Result
    std::cout << "Cals: " << cals << std::endl;

    return 0;
}
