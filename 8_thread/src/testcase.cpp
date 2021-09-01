#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>

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

    namespace test_lock_guard {
        int g_i = 0;
        int g_j = 100;
        std::mutex g_i_mutex;  // protects g_i
        // 当我们锁定mutex的时候 仅仅是mutex被锁定 而不是资源本身
        // 只是我们在写代码的时候 会约定g_i_mutex表示着g_i的使用状态
        // 每次多线程使用g_i这个资源的时候 会先访问g_i_mutex 看是否能使用这个资源
        // 因为是约定而不是强制绑定 所有就算我们lock住了g_i_mutex 其他线程任然可以访问和修改g_i 只是这个时候可能有问题

        void safe_increment()
        {
            const std::lock_guard<std::mutex> lock(g_i_mutex);
            ++g_i;
            if (g_i == 1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }

            std::cout << "g_i: " << g_i << "; in thread #"
                      << std::this_thread::get_id() << '\n';

            // g_i_mutex is automatically released when lock
            // goes out of scope
        }

        void test_lock_guard()
        {
            std::cout << "g_i: " << g_i << "; in main()\n";

            std::thread t1(safe_increment);
            std::thread t2(safe_increment);
            std::thread t3([](){
                std::cout << g_j << std::endl;
            });

            t1.join();
            t2.join();
            t3.join();

            std::cout << "g_i: " << g_i << "; in main()\n";
        }
    }

    namespace test_unique_lock {
        struct Box {
            explicit Box(int num) : num_things{num} {}

            int num_things;
            std::mutex m;
        };

        void transfer(Box &from, Box &to, int num)
        {
            // don't actually take the locks yet
            std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
            std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);

            // lock both unique_locks without deadlock
            std::lock(lock1, lock2);

            from.num_things -= num;
            to.num_things += num;

            // 'from.m' and 'to.m' mutexes unlocked in 'unique_lock' dtors
        }

        void test_unique_lock()
        {
            Box acc1(100);
            Box acc2(50);

            std::thread t1(transfer, std::ref(acc1), std::ref(acc2), 10);
            std::thread t2(transfer, std::ref(acc2), std::ref(acc1), 5);

            t1.join();
            t2.join();
        }
    } // namespace test_unique_lock
    
    /**********shared_mutex为c++17新特性 需要编译器支持17才能运行**********/
    /**********shared_lock为c++17新特性 需要编译器支持17才能运行**********/
    namespace tesst_shared_lock {
        /*
        class ThreadSafeCounter {
        public:
        ThreadSafeCounter() = default;

        // Multiple threads/readers can read the counter's value at the same time.
        unsigned int get() const {
            std::shared_lock lock(mutex_);
            return value_;
        }

        // Only one thread/writer can increment/write the counter's value.
        void increment() {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            value_++;
        }

        // Only one thread/writer can reset/write the counter's value.
        void reset() {
            std::unique_lock lock(mutex_);
            value_ = 0;
        }

        private:
            mutable std::shared_mutex mutex_;
            unsigned int value_ = 0;
        };

        void tesst_shared_lock() {
            ThreadSafeCounter counter;

            auto increment_and_print = [&counter]() {
                for (int i = 0; i < 3; i++) {
                    // std::cout << std::this_thread::get_id() << ' ' << counter.increment() << '\n';
                    counter.increment();
                    std::cout << std::this_thread::get_id() << ' ' << counter.get() << '\n';

                    // Note: Writing to std::cout actually needs to be synchronized as well
                    // by another std::mutex. This has been omitted to keep the example small.
                }
            };

            std::thread thread1(increment_and_print);
            std::thread thread2(increment_and_print);

            thread1.join();
            thread2.join();
        }
        */
        // Explanation: The output below was generated on a single-core machine. When
        // thread1 starts, it enters the loop for the first time and calls increment()
        // followed by get(). However, before it can print the returned value to
        // std::cout, the scheduler puts thread1 to sleep and wakes up thread2, which
        // obviously has time enough to run all three loop iterations at once. Back to
        // thread1, still in the first loop iteration, it finally prints its local copy
        // of the counter's value, which is 1, to std::cout and then runs the remaining
        // two loop iterations. On a multi-core machine, none of the threads is put to
        // sleep and the output is more likely to be in ascending order.
    }

    namespace test_lock_flag {
        struct bank_account {
            explicit bank_account(int balance) : balance(balance) {}
            int balance;
            std::mutex m;
        };

        void transfer(bank_account &from, bank_account &to, int amount)
        {
            // 锁定两个互斥而不死锁
            std::lock(from.m, to.m);
            // 保证二个已锁定互斥在作用域结尾解锁
            std::lock_guard<std::mutex> lock1(from.m, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(to.m, std::adopt_lock);

            // 等价方法：
            // 先给mutex打上defer_lock标记 表示自动解锁的lock
            // std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
            // std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);
            // 锁定前面打上标记的锁 如果没有被锁定 就调用锁定函数 如果被锁定就在此等待
            // std::lock(lock1, lock2);

            from.balance -= amount;
            to.balance += amount;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }

        void test_lock_flag()
        {
            bank_account my_account(100);
            bank_account your_account(50);

            std::thread t1(transfer, std::ref(my_account), std::ref(your_account), 10);
            std::thread t2(transfer, std::ref(your_account), std::ref(my_account), 5);
            std::thread t3(
                [](bank_account &from, bank_account &to, int amount) {
                    // 尝试锁定互斥量但不阻塞 不管成功与否
                    std::unique_lock<std::mutex> tryLock(from.m, std::try_to_lock);
                    if (tryLock.owns_lock()) {
                        std::cout << "try to lock success!\n";
                    } else {
                        std::cout << "try to lock failed!\n";
                    }

                },
                std::ref(my_account), std::ref(your_account), 10);

            t1.join();
            t2.join();
            t3.join();
        }
    }
}

#include <condition_variable>
namespace test_condition_variable {
    namespace test_notify_one {
        using namespace std::chrono_literals;
 
        std::condition_variable cv;
        std::mutex cv_m;
        int i = 0;
        bool done = false;

        void waits()
        {
            std::unique_lock<std::mutex> lk(cv_m);
            std::cout << "Waiting... \n";
            cv.wait(lk, []{return i == 1;});
            std::cout << "...finished waiting; i == " << i << '\n';
            done = true;
        }

        void signals()
        {
            std::this_thread::sleep_for(200ms);
            std::cout << "Notifying falsely...\n";
            cv.notify_one(); // waiting thread is notified with i == 0.
                             // cv.wait wakes up, checks i, and goes back to waiting

            std::unique_lock<std::mutex> lk(cv_m);
            i = 1;
            while (!done) 
            {
                std::cout << "Notifying true change...\n";
                lk.unlock();
                cv.notify_one(); // waiting thread is notified with i == 1, cv.wait returns
                std::this_thread::sleep_for(300ms);
                lk.lock();
            }
        }

        void test_notify_one()
        {
            std::thread t1(waits), t2(signals);
            t1.join(); 
            t2.join();
        }
    }
    
    namespace test_notify_all {
        std::condition_variable cv;
        std::mutex cv_m; // This mutex is used for three purposes:
                         // 1) to synchronize accesses to i
                         // 2) to synchronize accesses to std::cerr
                         // 3) for the condition variable cv
        int i = 0;

        void waits()
        {
            std::unique_lock<std::mutex> lk(cv_m);
            std::cerr << "Waiting... \n";
            cv.wait(lk, []{return i == 1;});
            std::cerr << "...finished waiting. i == 1\n";
        }

        void signals()
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            {
                std::lock_guard<std::mutex> lk(cv_m);
                std::cerr << "Notifying...\n";
            }
            cv.notify_all();

            std::this_thread::sleep_for(std::chrono::seconds(1));

            {
                std::lock_guard<std::mutex> lk(cv_m);
                i = 1;
                std::cerr << "Notifying again...\n";
            }
            cv.notify_all();
        }

        void test_notify_all()
        {
            std::thread t1(waits), t2(waits), t3(waits), t4(signals);
            t1.join(); 
            t2.join(); 
            t3.join();
            t4.join();
        }
    }
}

// #include <semaphore>
namespace test_semaphone {
    /*
    // global binary semaphore instances
    // object counts are set to zero
    // objects are in non-signaled state
    std::binary_semaphore
        smphSignalMainToThread(0),
        smphSignalThreadToMain(0);

    void ThreadProc()
    {
        // wait for a signal from the main proc
        // by attempting to decrement the semaphore
        smphSignalMainToThread.acquire();

        // this call blocks until the semaphore's count
        // is increased from the main proc

        std::cout << "[thread] Got the signal\n"; // response message

        // wait for 3 seconds to imitate some work
        // being done by the thread
        using namespace std::literals;
        std::this_thread::sleep_for(3s);

        std::cout << "[thread] Send the signal\n"; // message

        // signal the main proc back
        smphSignalThreadToMain.release();
    }

    void test_semaphone()
    {
        // create some worker thread
        std::thread thrWorker(ThreadProc);

        std::cout << "[main] Send the signal\n"; // message

        // signal the worker thread to start working
        // by increasing the semaphore's count
        smphSignalMainToThread.release();

        // wait until the worker thread is done doing the work
        // by attempting to decrement the semaphore's count
        smphSignalThreadToMain.acquire();

        std::cout << "[main] Got the signal\n"; // response message
        thrWorker.join();
    }
    */
}

int main() {
    // test_condition_variable::test_notify_all::test_notify_all();
    static auto phase = "... done\n" "Cleaning up...\n";
    std::cout <<  phase;
}