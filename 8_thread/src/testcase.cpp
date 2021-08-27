#include <mutex>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <string>
#include <thread>


namespace test_thread {
std::thread::id main_thread_id;

void hello()  
{
    std::cout << "Hello Concurrent World\n";
    if (main_thread_id == std::this_thread::get_id())
        std::cout << "This is the main thread.\n";
    else
        std::cout << "This is not the main thread.\n";
}

void pause_thread(int n) {
    std::this_thread::sleep_for(std::chrono::seconds(n));
    std::cout << "pause of " << n << " seconds ended\n";
}
void test_thread()
{
    main_thread_id = std::this_thread::get_id();
    std::thread t(hello);
    std::cout << "hardware_concurrency = " << t.hardware_concurrency() << std::endl;//可以并发执行多少个(不准确)
    std::cout << "native_handle = " << t.native_handle() << std::endl;//可以并发执行多少个(不准确)
    t.join();
    std::thread a(hello);
    a.detach();
    std::thread threads[5];                         // 默认构造线程

    std::cout << "Spawning 5 threads...\n";
    for (int i = 0; i < 5; ++i)
        threads[i] = std::thread(pause_thread, i + 1);   // move-assign threads
    std::cout << "Done spawning threads. Now waiting for them to join:\n";
    for (auto &thread : threads)
        thread.join();
    std::cout << "All threads joined!\n";
}
}

namespace test_thread_function {
// "busy sleep" while suggesting that other threads run 
// for a small amount of time
void little_sleep(std::chrono::microseconds us)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + us;
    do {
        std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
    std::cout << "little_sleep() end!" << std::endl;
}
 
void test_thread_function()
{
    auto start = std::chrono::high_resolution_clock::now();
 
    little_sleep(std::chrono::microseconds(100));
 
    std::chrono::duration<double, std::milli> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "waited for "
              << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()
              << " microseconds\n";
    
    // Returns the id of the current thread
    std::thread::id this_id = std::this_thread::get_id();
    std::cout << "this current thread id = "
              << this_id
              << std::endl;

    // stops the execution of the current thread for a specified time duration
    using namespace std::chrono_literals;
    start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(2000ms);
    auto end = std::chrono::high_resolution_clock::now();
    elapsed = end-start;
    std::cout << "sleep_for() Waited " << elapsed.count() << " ms\n";

    // stops the execution of the current thread until a specified time point
    start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_until(start + 2000ms);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "sleep_until() Waited " << elapsed.count() << " ms\n";
}
}

namespace test_jthread {
// jthread c++20的新特性 需要编译器支持
// void sleep(const int& seconds)
// {
//     std::this_thread::sleep_for(std::chrono::seconds(seconds));
// }
// 
// void test_jthread()
// {
//     std::jthread jt{ [](std::stop_token st) {
//         while(!st.requested_stop()) {
//             std::cout << "doing work\n";
//             sleep(1);
//         }
//     }};
//     sleep(5);
//     jt.request_stop();
// }
// 
// jthread::~jthread()
// {
//     if (joinable()) {
//         request_stop();
//         join();
//     }
// }
}

namespace test_mutex {
/**********scoped_lock为c++17新特性 需要编译器支持17才能运行**********/
namespace test_scoped_lock
{
// struct Employee {
//     Employee(std::string id) : id(id) {}
//     std::string id;
//     std::vector<std::string> lunch_partners;
//     std::mutex m;
//     std::string output() const
//     {
//         std::string ret = "Employee " + id + " has lunch partners: ";
//         for( const auto& partner : lunch_partners )
//             ret += partner + " ";
//         return ret;
//     }
// };
//  
// void send_mail(Employee &, Employee &)
// {
//     // 模拟耗时的发信操作
//     std::this_thread::sleep_for(std::chrono::seconds(1));
// }
//  
// void assign_lunch_partner(Employee &e1, Employee &e2)
// {
//     static std::mutex io_mutex; // 打印互斥
//     {
//         std::lock_guard<std::mutex> lk(io_mutex);
//         std::cout << e1.id << " and " << e2.id << " are waiting for locks" << std::endl;
//     }
//  
//     {
//         // 用 std::scoped_lock 取得二个锁，而无需担心
//         // 其他对 assign_lunch_partner 的调用死锁我们
//         // 而且它亦提供便利的 RAII 风格机制
//         std::scoped_lock lock(e1.m, e2.m);
//  
//         {
//             std::lock_guard<std::mutex> lk(io_mutex);
//             std::cout << e1.id << " and " << e2.id << " got locks" << std::endl;
//         }
//         // 互斥操作 使用scoped同时获取并锁定多个资源进行操作 资源之间不死锁
//         e1.lunch_partners.push_back(e2.id);
//         e2.lunch_partners.push_back(e1.id);
//     }
//     
//     // 无竞争耗时操作
//     send_mail(e1, e2);
//     send_mail(e2, e1);
// }
//  
// void test_scoped_lock()
// {
//     Employee alice("alice"), bob("bob"), christina("christina"), dave("dave");
//  
//     // 在并行线程中指派，因为就午餐指派发邮件消耗很长时间
//     std::vector<std::thread> threads;
//     threads.emplace_back(assign_lunch_partner, std::ref(alice), std::ref(bob));
//     threads.emplace_back(assign_lunch_partner, std::ref(christina), std::ref(bob));
//     threads.emplace_back(assign_lunch_partner, std::ref(christina), std::ref(alice));
//     threads.emplace_back(assign_lunch_partner, std::ref(dave), std::ref(bob));
//  
//     for (auto &thread : threads) thread.join();
//     std::cout << alice.output() << '\n'  << bob.output() << '\n'
//               << christina.output() << '\n' << dave.output() << '\n';
// }
} // namespace test_scoped_lock


}

int main() {
    test_thread_function::test_thread_function();
}