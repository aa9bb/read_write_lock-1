#include "read_write_lock.cpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
 
RWLock lock;
std::mutex mt;
struct Counter {
    int value;

    Counter() : value(0){}

    void increment(){
    	//std::unique_lock<std::mutex> lock(mt);
        lock.wrlock();
        ++value;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        lock.unlock();
    }

    int get_value()
    {
    	//std::unique_lock<std::mutex> lock(mt);
        lock.rdlock();
    	int val = value;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        lock.unlock();
        return val;
    }
};

int main(){
    for(int a = 0; a < 10 ; a++)
    {
    	auto start = std::chrono::steady_clock::now();
        Counter counter;
        std::vector<std::thread> threads;
        for(int i = 0; i < 100; ++i){
            threads.push_back(std::thread([&counter](){
                for(int i = 0; i < 100; ++i){
                    counter.increment();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                	for(int j =0; j < 5; j++)
                		counter.get_value();
                }
            }));
        }

        for(auto& thread : threads){
            thread.join();
        }
        std::cout << "test case: " << a << std::endl;
        std::cout << counter.get_value() << std::endl;
        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}

	
