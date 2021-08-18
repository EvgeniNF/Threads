#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
// Файлы тест потоков
//======================= использование мьютексов ====================//
void dosomth_1(std::mutex &mtx){
    mtx.lock();
    std::cout << "-- ID thread:" << std::this_thread::get_id();
    std::cout << "      -- Started func: dosomth_1" << std::endl;
    mtx.unlock();
}
void dosomth_2(std::mutex &mtx){
    mtx.lock();
    std::cout << "-- ID thread:" << std::this_thread::get_id();
    std::cout << "      -- Started func: dosomth_2" << std::endl;
    mtx.unlock();
}
void run_mutex(){
    std::mutex mtx;
    std::thread th_1(dosomth_1, std::ref(mtx));
    std::thread th_2(dosomth_2, std::ref(mtx));
    th_1.join();
    th_2.join();
}
//=================== mutex end ====================//

// Использование лямбда выражения для получения значения из потока //
int incr(int prev){
    prev += 1;
    return prev;
}
void print_func(int data){
    if (data < 10){
        std::cout << "-- data: " << data << " < 10" << std::endl;
    }
    else {
        std::cout << "-- data: " << data << " >= 10" << std::endl;
    }
}
void loop(){
    int a = 0;
    while (a < 20){
        std::thread th_1(print_func, a);
        std::thread th_2([&a]() { a = incr(a); } );
        th_1.join();
        th_2.join();
    }
}
//=================== lambda end ====================//

// Использование функций потока //
void print_points(int a){
    std::cout << "-- ID thread: " << std::this_thread::get_id() << " work!  ->   #";
    for (int i = 0; i < a; i++){
        std::cout << ".";
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "#" << std::endl;
    std::cout << "-- Work is done!" << std::endl;
}
void dosomth_3(){
    int s = 10;
    std::cout << "-- ID thread: " << std::this_thread::get_id() << std::endl;
    std::cout << "-- This thread will stopped" << std::endl;
    std::this_thread::yield();
    std::thread th_2(print_points, s);
    std::this_thread::sleep_for(std::chrono::seconds(s));
    th_2.join();
    std::cout << "-- ID thread: " << std::this_thread::get_id() << std::endl;
}
void run_func(){
    std::thread th_1(dosomth_3);
    th_1.join();
}
// Связывание потоков через future //

//================= future end ======================//

int main() {
    run_mutex();
    system("pause");
    loop();
    system("pause");
    run_func();
    system("pause");
    return 0;
}
