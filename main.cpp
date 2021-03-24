#include <iostream>
#include <map>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>
#include <chrono>


//// RW LOCK
#pragma region
std::map<std::string,int> shoppingList{{"chleb",2},{"papryka",4},{"piwo",3}};

std::shared_timed_mutex shoppingListMutex;

void addProduct(const std::string& name, int quantity){
    std::lock_guard<std::shared_timed_mutex> writerLock(shoppingListMutex);
    std::cout << "\nAdding " << name;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    shoppingList[name]= quantity;
    std::cout << " -> Added " << name << std::endl;
}

void getQuantity(const std::string& name){
    std::shared_lock<std::shared_timed_mutex> readerLock(shoppingListMutex);
    std::cout << name << ": " << shoppingList[name] << " ";
}
#pragma endregion

//// ATOMIC SP
#pragma region
[[deprecated]]void smart() {
    std::shared_ptr<int> ptr = std::make_shared<int>(2011);
    for (auto i = 0;i < 10; i++)
    {
        std::thread([ptr] {
                        std::shared_ptr<int> localPtr(ptr);
                        localPtr = std::make_shared<int>(2014);
                    }
        ).detach();
    }

    std::shared_ptr<int> ptr1 = std::make_shared<int>(2011);

    for (auto i= 0; i<10; i++){
        std::thread([&ptr1]{
            ptr1= std::make_shared<int>(2014);
        }).detach();
    }

    std::shared_ptr<int> ptr2 = std::make_shared<int>(2011);

    for (auto i =0;i<10;i++){
        std::thread([&ptr2]{
            auto localPtr= std::make_shared<int>(2014);
            std::atomic_store(&ptr2, localPtr);
        }).detach();
    }


}
#pragma endregion

//// LATCHES AND BARRIERS
#pragma region
#if 0
// Latch
void DoWork(threadpool* pool) {
    std::latch completion_latch(NTASKS);
    for (int i = 0; i < NTASKS; ++i) {
        pool->add_task([&] {
            // perform work
            ...
            completion_latch.count_down();
        }));
    }
    // Block until work is done
    completion_latch.wait();
}

// Barrier
void DoWork() {
    ...
    std::barrier task_barrier(n_threads);

    for (int i = 0; i < n_threads; ++i) {
        workers.push_back(new thread([&] {
            bool active = true;
            while(active) {
                Task task = tasks.get();

                ...

                task_barrier.arrive_and_wait();
            }
        });
    }
    while (!finished()) {
        GetNextStage(tasks);
    }
}

// Flex barrier
void DoWork() {

    ...
    int initial_threads;
    atomic<int> current_threads(initial_threads);
    std::function rf = [&] { return current_threads;};
    std::flex_barrier task_barrier(n_threads, rf);

    for (int i = 0; i < n_threads; ++i) {
        workers.push_back(new thread([&] {
            bool active = true;
            while(active) {
                Task task = tasks.get();
                // perform task
                ...
                if (finished(task)) {
                    current_threads--;
                    active = false;
                }
                task_barrier.arrive_and_wait();
            }
        });
    }
    while (!finished()) {
        GetNextStage(tasks);
    }
}
#endif
#pragma endregion

//// COROUTINES
#pragma region

int func1() {
    return 1;
}
int func2(int arg) {
    return arg;
}
double func2(double arg) {
    return arg;
}
template <typename T>
T func3(T arg) {
    return arg;
}
struct FuncObject4 {
    int operator()() { // (1)
        return 4;
    }
};
auto func5 = [] { return 5; };
auto func6 = [] (auto arg){ return arg;};

void functionsShowOff() {

    func1();
    func2(2);
    func2(2.0);
    func3(3);
    func3(3.0);
    FuncObject4 func4;
    func4();
    func5();
    func6(6);
    func6(6.0);

}


std::vector<int> getNumbers(int begin, int end, int inc = 1)
{
    std::vector<int> numbers;
    for (int i = begin; i < end; i += inc) {
        numbers.push_back(i);
    }
    return numbers;
}

int generatorForNumbers(int begin, int inc = 1)
{
    return begin;
}
//generator<int> generatorForNumbers(int begin, int inc = 1) {
//
//    for (int i = begin;; i += inc) {
//        co_yield i;
//    }
//
//}


/* Awaitable
std::suspend_always
struct suspend_always {
    constexpr bool await_ready() const noexcept { return false; }
    constexpr void await_suspend(coroutine_handle<>) const noexcept {}
    constexpr void await_resume() const noexcept {}
};

 struct suspend_never {
   constexpr bool await_ready() const noexcept { return true; }
    constexpr void await_suspend(coroutine_handle<>) const noexcept {}
    constexpr void await_resume() const noexcept {}
};

*/

/* Server C++
Acceptor acceptor{443};

while (true){
    Socket socket= acceptor.accept();
    auto request= socket.read();
    auto response= handleRequest(request);
    socket.write(response);
}

Acceptor acceptor{443};

while (true){
    Socket socket= co_await acceptor.accept();
    auto request= co_await socket.read();
    auto response= handleRequest(request);
    co_await socket.write(response);
}

 */

#pragma endregion
//// TRANSACTIONAL MEMORY
#pragma region
using namespace std::chrono_literals;
int j= 0;

void increment()
{
    synchronized
    {
        std::cout << ++j << " ,";
//        std::this_thread::sleep_for(1ns);
    }
}

#pragma endregion

//// TASK BLOCKS
//TODO
int main()
{

#pragma region RW LOCK

    std::cout << std::endl;

    std::thread reader1([] { getQuantity("chleb"); });
    std::thread reader2([] { getQuantity("piwo"); });
    std::thread w1([] { addProduct("piwo", 20); });
    std::thread reader3([] { getQuantity("papryka"); });
    std::thread reader4([] { getQuantity("piwo"); });
    std::thread w2([] { addProduct("sledzik", 1); });
    std::thread reader5([] { getQuantity("chleb"); });
    std::thread reader6([] { getQuantity("sledzik"); });
    std::thread reader7([] { getQuantity("piwo"); });
    std::thread reader8([] { getQuantity("papryka"); });

    reader1.join();
    reader2.join();
    reader3.join();
    reader4.join();
    reader5.join();
    reader6.join();
    reader7.join();
    reader8.join();
    w1.join();
    w2.join();

    std::cout << std::endl;

    std::cout << "\nThe new telephone book" << std::endl;
    for (auto teleIt: shoppingList) {
        std::cout << teleIt.first << ": " << teleIt.second << std::endl;
    }

    std::cout << std::endl;

#pragma endregion

#pragma region COROUTINES
    // funkcje
    functionsShowOff();
    // Generator po staremu
    std::cout << "Po staremu "<< std::endl;
    const auto numbers = getNumbers(-10, 11);
    for (auto n: numbers) std::cout << n << " ";
    std::cout << "\n\n";
    for (auto n: getNumbers(0, 101, 5)) std::cout << n << " ";
    std::cout << "\n\n";

    // Generator po nowemu
    std::cout << std::endl;
    const auto numbersNew= generatorForNumbers(-10);
    for (int i= 1; i <= 20; ++i) std::cout << numbersNew << " ";
//    for (auto n : generatorForNumbers(0, 5)) std::cout << n << " ";
    std::cout << "\n\n";

#pragma endregion
#pragma region Transactional
    // synchronized
    std::cout << std::endl;
    std::vector<std::thread> vecSyn(10);
//    std::vector<std::thread> vecUnsyn(10);

    for(auto& thr: vecSyn)
        thr = std::thread([]{ for(int n = 0; n < 10; ++n) increment(); });
//    for(auto& thr: vecUnsyn)
//        thr = std::thread([]{ for(int n = 0; n < 10; ++n) std::cout << ++j << " ,"; });

    for(auto& thr: vecSyn) thr.join();
//    for(auto& thr: vecUnsyn) thr.join();

    std::cout << "\n\n";
#pragma endregion


}
