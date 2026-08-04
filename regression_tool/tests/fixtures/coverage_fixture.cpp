#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

static int positive_path(int value) {
    return value * 2 + 1;
}

static int negative_path(int value) {
    return value * value - 1;
}

int main(int argc, char** argv) {
    const std::string mode = argc > 1 ? argv[1] : "positive";
    if (mode == "timeout") {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        return 0;
    }
    if (mode == "fail") {
        std::cerr << "requested failure\n";
        return 3;
    }
    if (mode == "positive") {
        std::cout << "result=" << positive_path(4) << "\n";
    } else {
        std::cout << "result=" << negative_path(-4) << "\n";
    }
    std::ofstream output("fixture-output.json");
    output << "{\"mode\":\"" << mode << "\"}\n";
    return 0;
}

