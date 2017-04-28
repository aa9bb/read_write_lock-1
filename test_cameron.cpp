#include "cameron/cpp11-on-multicore/common/rwlock.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
 
NonRecursiveRWLock m_rwLock;
struct Counter {
    int value;

    Counter() : value(0){}

    void increment(){
        WriteLockGuard<NonRecursiveRWLock> guard(m_rwLock);
        ++value;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    int get_value()
    {
        ReadLockGuard<NonRecursiveRWLock> guard(m_rwLock);
    	int val = value;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return val;
    }
};

int main(){
    std::vector<double> output;
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
        double d = std::chrono::duration <double, std::milli> (diff).count();
        std::cout << d << " ms" << std::endl;
        output.push_back(d);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    double total;
    for(auto a: output)
    {
        total += a;
    }
    std::cout << "*************** test_cameron ********************" << std::endl;
    std::cout << "Average time for " << output.size() << " test cases are : " << total/output.size() << std::endl;
    return 0;
}

	
