#include "rwlock.h"
#include "../../read_write_lock.cpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
ipvcore::conn::third_party::rwlock::NonRecursiveRWLock m_rwLock;
std::mutex mx;
RWLock lock;
struct Counter {
    int value;
    int read_count;
    int write_count;

    Counter() : read_count(0), write_count(0), value(0){}

    void write(){
        ipvcore::conn::third_party::rwlock::WriteLockGuard<ipvcore::conn::third_party::rwlock::NonRecursiveRWLock> guard(m_rwLock);
        //std::lock_guard<std::mutex> lock(mx);
        //WriteLockGuard<RWLock> lc(lock);

        write_count++;
        value++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    void read()
    {
        ipvcore::conn::third_party::rwlock::ReadLockGuard<ipvcore::conn::third_party::rwlock::NonRecursiveRWLock> guard(m_rwLock);
        //std::lock_guard<std::mutex> lock(mx);
        //ReadLockGuard<RWLock> lc(lock);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
};

int main(){
    std::vector<double> output;
    for(int a = 0; a < 2 ; a++)
    {
    	auto start = std::chrono::high_resolution_clock::now();
        Counter counter;
        std::vector<std::thread> threads;
        for(int i = 0; i < 10; ++i){
            threads.push_back(std::thread([&counter](){
                int wrcount = 30, rdcount = 70;
                for(; wrcount > 0 && rdcount > 0 ; ){
                    int xRan;
                    srand( time(0)); // This will ensure a really randomized number by help of time.
	
                    xRan=rand()%50+1;
                    xRan = xRan % 2;
                    if(xRan == 0)
                    {
                        rdcount--;
                        counter.read();
                    }
                    else 
                    {
                        wrcount--;
                        counter.write();
                    }
                }
                if(wrcount == 0)
                {
                    for( ; rdcount > 0; )
                    {
                        rdcount--;
                        counter.read();
                    }
                }
                if(rdcount == 0)
                {
                    for( ; wrcount > 0; )
                    {
                        wrcount--;
                        counter.write();
                    }
                }
            }));
        }

        for(auto& thread : threads){
            thread.join();
        }
        std::cout << "test case: " << a << std::endl;
        std::cout << "write count: " << counter.write_count << std::endl;
        std::cout << "value: " << counter.value << std::endl;
        auto end = std::chrono::high_resolution_clock::now();
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
    std::cout << "Average time for " << output.size() << " test cases are : " << total/output.size() << "ms" << std::endl;
    return 0;
}

	
