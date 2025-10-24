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

void find_primes_block_immediate(int thread_id, long long start, long long end) {
    for (long long i = start; i <= end; ++i) {
        if (is_prime(i)) {
            print_immediate(thread_id, i);
        }
    }
}

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::cout << "--- Program Start (Variant 1: Immediate + Block): " << get_timestamp() << " ---" << std::endl;

    auto config = parse_config("config.txt");
    int num_threads = config["NUM_THREADS"];
    long long max_number = config["MAX_NUMBER"];

    std::cout << "Config: Using " << num_threads << " threads to find primes up to " << max_number << "." << std::endl;

    std::vector<std::thread> threads;
    long long chunk_size = max_number / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        long long start = i * chunk_size + 1;
        long long end = (i == num_threads - 1) ? max_number : (i + 1) * chunk_size;
        threads.push_back(std::thread(find_primes_block_immediate, i, start, end));
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