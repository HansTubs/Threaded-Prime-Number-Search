#define _CRT_SECURE_NO_WARNINGS //keep this

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <fstream>
#include <chrono>
#include <cmath>
#include <map>
#include <sstream>
#include <iomanip>
#include <algorithm>

std::mutex g_data_mutex;

std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

bool is_prime(long long n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long long i = 5; i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

std::map<std::string, int> parse_config(const std::string& filename) {
    std::map<std::string, int> config;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            config[key] = std::stoi(value);
        }
    }
    return config;
}

void print_immediate(int thread_id, long long number) {
    std::lock_guard<std::mutex> lock(g_data_mutex);
    std::cout << "[" << get_timestamp() << "] [Thread " << thread_id << "]: Found prime " << number << std::endl;
}

void find_primes_interleaved_immediate(int thread_id, int num_threads, long long max_num) {
    for (long long i = thread_id + 1; i <= max_num; i += num_threads) {
        if (is_prime(i)) {
            print_immediate(thread_id, i);
        }
    }
}

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::cout << "--- Program Start (Variant 2: Immediate + Interleaved): " << get_timestamp() << " ---" << std::endl;

    auto config = parse_config("config.txt");
    int num_threads = config["NUM_THREADS"];
    long long max_number = config["MAX_NUMBER"];

    std::cout << "Config: Using " << num_threads << " threads to find primes up to " << max_number << "." << std::endl;

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(find_primes_interleaved_immediate, i, num_threads, max_number));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << "\n--- Program End: " << get_timestamp() << " ---" << std::endl;
    std::cout << "--- Total Execution Time: " << duration.count() << " seconds ---" << std::endl;

    return 0;
}