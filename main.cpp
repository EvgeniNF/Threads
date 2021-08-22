#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <vector>
#include <chrono>
// home work //------------------------------------------------------------------------------------------------------
void in_str(int &i){
    std::string a;
    while(a.empty()){
        std::getline(std::cin, a);
        if (a.empty()){
            i++;
        }
        else {
            i = 100;
            break;
        }
        if (i > 3){
            i = 0;
        }
    }
}
void out_str(int &i){
    while (true){
        if (i == 0) { system("color F"); }
        else if (i == 1) { system("color 4"); }
        else if (i == 2) { system("color 6"); }
        else if (i == 3) { system("color 9"); }
        else { break; }
        system("cls");
        std::cout << "Hello, threads!" << std::endl;
    }
}
void run_home_work(){
    int a = 0;
    std::thread th_1(in_str, std::ref(a));
    std::thread th_2(out_str, std::ref(a));
    th_1.join();
    th_2.join();
}
// end //-------------------------------------------------------------------------------------------------------------

/* Доступ к вектору из двух потоков
 * Первый поток читает данные и удаляет ячейки
 * Второй поток пишет данные
 */
void show(std::vector<int> vec){
    int size = static_cast<int>(vec.size());
    std::cout << "-- Vector size: " << size << " vector data: ";
    for(int i = 0; i < size; i++){
        std::cout << vec.at(i) << " | ";
    }
    std::cout << std::endl;
}

void  show_and_del(std::vector<int> &vec,
                  std::mutex &mtx){
    std::lock_guard<std::mutex> lock(mtx);
    if(!(vec.empty())){
            show(vec);
            vec.erase(vec.begin(), vec.end());
    }
}

void add_ell(std::vector<int> &vec,
             int n, std::mutex &mtx){
    std::lock_guard<std::mutex> lock(mtx);
    vec.push_back(n);
}

void run_vector(){
    std::vector<int> vector;
    std::mutex mtx;
    for (int i = 0; i <= 1000; i++){
        std::thread th_1(add_ell, std::ref(vector), i, std::ref(mtx));
        std::thread th_2(show_and_del, std::ref(vector), std::ref(mtx));
        th_1.join();
        th_2.join();
    }
}
/* Доступ к вектору из двух потоков и выставление приоритетности операций
 * 1. добавление элемента
 * 2. если добавлен элемент и вектор не занят вывод и удаление данных
 */
void add_ell_(std::vector<int>* vec, int i, std::promise<bool> &pr){
    vec->resize(vec->size() + 1);
    vec->at(vec->size() - 1) = i;
    pr.set_value(true);
}
void show_and_del_(std::vector<int>* vec, std::promise<bool> &pr){
    std::future<bool> ft = pr.get_future();
    if (ft.get()){
        show(*vec);
        vec->erase(vec->begin(), vec->end());
    }
    else {
        std::cout << "-- Error!" << std::endl;
    }
}
void run_tasks(){
    auto* vector = new std::vector<int>;
    for (int i = 0; i <= 10000; i++){
        std::promise<bool> pr;
        std::thread th_1(add_ell_, vector, i, std::ref(pr));
        std::thread th_2(show_and_del_, vector, std::ref(pr));
        th_1.join();
        th_2.join();
    }
    std::cout << "-- Size of vector: " << vector->size() << std::endl;
    delete vector;
}

// Работа cond_var

void do_work(std::mutex &mt, std::condition_variable &cv,
             std::string &data, bool &ready,  bool &process){
    std::unique_lock<std::mutex> lk(mt);
    cv.wait(lk, [&ready] () { return ready; } );
    std::cout << "-- Worker thread is processing data" << std::endl;
    data += " after processing";
    process = true;
    std::cout << "-- Worker thread signals data processing completed" << std::endl;
    cv.notify_one();

}
void run_not(){
    std::mutex mt;
    std::condition_variable cv;
    std::string data;
    bool ready = false;
    bool process = false;
    std::thread worker(do_work, std::ref(mt), std::ref(cv), std::ref(data),
                       std::ref(ready), std::ref(process));
    data = "Example data";
    {
        std::lock_guard<std::mutex> lk(mt);
        ready = true;
        std::cout << "-- main() signals data ready for processing" << std::endl;
    }
    cv.notify_one();
    {
        std::unique_lock<std::mutex> lk(mt);
        cv.wait(lk, [&process] { return process; });
        std::cout << "-- Back in main(), data = " << data << std::endl;
    }
    worker.join();
}

// Работа mutex

void do_work_mutex(std::mutex &mt){
    for (int i = 0; i <= 10; i++){
        if(mt.try_lock()){
            std::cout << "Hello, I'm thread: " << std::this_thread::get_id() << " ";
            std::cout << "Num of iteration: " << i << std::endl;
            mt.unlock();
        }
        else {
            i--;
        }
    }
}
void run_mutex(){
    std::mutex mt;
    std::thread th_1(do_work_mutex, std::ref(mt));
    do_work_mutex(mt);
    th_1.join();
}

// Работа timed_mutex

void do_work_timed_mutex(std::timed_mutex &t_mt){
    t_mt.try_lock_for(std::chrono::milliseconds(3000));
    std::cout << "Hello, I'm thread: " << std::this_thread::get_id() << std::endl;
}
void run_timed_mutex(){
    std::timed_mutex t_mt;
    std::thread th_1(do_work_timed_mutex, std::ref(t_mt));
    do_work_timed_mutex(t_mt);
    th_1.join();
}

// Работа с recursive_mutex

void foo1(std::string &data, std::recursive_mutex &r_mt){
    std::lock_guard<std::recursive_mutex> lg(r_mt);
    data = "foo1";
    std::cout << data << std::endl;
}
void foo2(std::string &data, std::recursive_mutex &r_mt){
    std::lock_guard<std::recursive_mutex> lg(r_mt);
    data = "foo2";
    std::cout << data << std::endl;
    foo1(data, r_mt);
    std::cout << data << std::endl;
}
void run_recursive_mutex(){
    std::recursive_mutex r_mt;
    std::string data;
    std::thread th_1(foo1, std::ref(data), std::ref(r_mt));
    std::thread th_2(foo2, std::ref(data), std::ref(r_mt));
    th_1.join();
    th_2.join();
}

// Работа с shared_mutex
void incr (std::shared_mutex &s_mt, int &i){
    std::unique_lock<std::shared_mutex> ul(s_mt);
    i++;
}
void show_counter(std::shared_mutex &s_mt, int &i){
    //std::shared_lock<std::shared_mutex> sl(s_mt);
    std::cout << "Counter = " << i << " " << "Thread: " << std::this_thread::get_id() << std::endl;
}
void reset(std::shared_mutex &s_mt, int &i){
    std::unique_lock<std::shared_mutex> ul(s_mt);
    i = 0;
}
void run_shared_mutex(){
    int i = 0;
    std::shared_mutex s_mt;
    auto incr_work = [&s_mt, &i] (){
        while (i <= 30){
            incr(s_mt, i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };
    auto show_work = [&s_mt, &i] () {
        while (i <= 30){
            show_counter(s_mt, i);
        }
    };
    std::thread th_1(incr_work);
    std::thread th_2(show_work);
    std::thread th_3(show_work);
    th_1.join();
    th_2.join();
    th_3.join();
}

// Работа с scoped_mutex
struct Persons{
    explicit Persons(std::string human) : names(std::move(human)) {}
    std::string names;
    std::mutex mt;
    [[nodiscard]] std::string ret_humans() const {
        return names;
    }
};
void add_human(Persons &pr_1, Persons &pr_2){
    static std::mutex io;
    io.lock();
    std::cout << "Persons: " << pr_1.names << " - " << pr_2.names << " wait for lock!" << std::endl;
    io.unlock();
    {
        std::scoped_lock sl(pr_1.mt, pr_2.mt);
        io.lock();
        std::cout << "Persons: " << pr_1.names << " - " << pr_2.names << " was locked!" << std::endl;
        io.unlock();
        pr_1.names += ", " + pr_2.names;
        pr_2.names += ", " + pr_1.names;
    }
}
void run_scoped_mutex(){
    Persons josh("Josh"), adrian("Adrian"), poupe("Poupe"), elis("Elis");
    std::vector<std::thread> threads;
    threads.emplace_back(add_human, std::ref(josh), std::ref(elis));
    threads.emplace_back(add_human, std::ref(poupe), std::ref(adrian));
    threads.emplace_back(add_human, std::ref(adrian), std::ref(poupe));
    threads.emplace_back(add_human, std::ref(elis), std::ref(josh));
    for (auto &th : threads) { th.join(); }
    std::cout << "Result: " << std::endl;
    std::cout << "Josh: " << josh.names << std::endl;
    std::cout << "Adrian: " << adrian.names << std::endl;
    std::cout << "Poupe: " << poupe.names << std::endl;
    std::cout << "Elis: " << elis.names << std::endl;
}

// Блокировка структуры
struct Data{
    int value = 0;
    std::mutex mt;
};
void incr_data(Data &data){
    std::lock_guard<std::mutex> lg(data.mt);
    {
        data.value++;
        std::cout << "-- Thread: " << std::this_thread::get_id() << " It's incr" << std::endl;
    }
}
void show_data(Data &data){
    std::lock_guard<std::mutex> lg(data.mt);
    {
        std::cout << "-- Thread: " << std::this_thread::get_id() << " Data: " << data.value << std::endl;
    }
}
void run_lock_struct(){
    Data data;
    auto incr_and_show = [&data] () {
        for (int i = 0; i < 20; i++){
            incr_data(data);
            show_data(data);
        }
    };
    std::thread th_1(incr_and_show);
    std::thread th_2(incr_and_show);
    th_1.join();
    th_2.join();
}

// Работа с conditional_variable
void work(std::condition_variable &cv, std::mutex &cv_m, bool &ready){
    std::unique_lock lk(cv_m);
    cv.wait(lk, []{ return true; });
    std::cout << "-- Do some work..." << std::endl;
    for (int i = 0; i <= 10; i++){
        std::cout << "-- Waite please..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    ready = true;
    lk.unlock();
    cv.notify_one();
}
void run_conditional_variable(){
    std::condition_variable cv;
    std::mutex cv_m;
    bool ready = false;
    std::thread th_1(work, std::ref(cv), std::ref(cv_m), std::ref(ready));
    {
        std::lock_guard<std::mutex> lg(cv_m);
        std::cout << "-- Start work process" << std::endl;
    }
    cv.notify_one();
    {
        std::unique_lock lk(cv_m);
        cv.wait(lk, [&ready]{ return ready; });
        std::cout << "-- End work process!" << std::endl;
        lk.unlock();
    }
    th_1.join();
}

// Работа c Promise
void work_prom(std::mutex &mt, std::promise<bool> &pr){
    mt.lock();
    std::cout << "-- Do any..." << std::endl;
    mt.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    mt.lock();
    std::cout << "Work completed!" << std::endl;
    pr.set_value(true);
    mt.unlock();
}
void run_promise(){
    std::mutex mt;
    std::promise<bool> pr;
    std::future ft = pr.get_future();
    std::thread th_1(work_prom, std::ref(mt), std::ref(pr));
    mt.lock();
    std::cout << "Waite content!" << std::endl;
    mt.unlock();
    ft.wait();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    mt.lock();
    std::cout << "End work!" << std::endl;
    mt.unlock();
    th_1.join();
}

// Работа с packaged_task
bool work_task(std::mutex &mt){
    mt.lock();
    std::cout << "-- Work task begin!" << std::endl;
    mt.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return true;
}
void run_packaged_task(){
    std::mutex mt;
    std::packaged_task<bool(std::mutex &)> tsk_1(work_task);
    std::future ft = tsk_1.get_future();
    std::thread th_1(std::move(tsk_1), std::ref(mt));
    mt.lock();
    std::cout << "-- Waiting end work" << std::endl;
    mt.unlock();
    if (ft.get()){
        std::cout << "-- Work is done!" << std::endl;
    }
    th_1.join();
}

// Работа с async
int work_async(std::mutex &io, int a, int b){
    {
        std::lock_guard<std::mutex> lg(io);
        std::cout << "-- Start calculating | Thread id = " << std::this_thread::get_id() << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    int res = a + b;
    {
        std::lock_guard<std::mutex> lg(io);
        std::cout << "-- End calculating" << std::endl;
    }
    return res;
}

void run_async(){
    std::mutex io;
    int a = 10, b = 20;
    std::future<int> ft;
    ft = std::async(std::launch::async ,work_async, std::ref(io), a, b);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    {
        std::lock_guard<std::mutex> lg(io);
        std::cout << "-- Waiting data... | Thread id = " << std::this_thread::get_id() << std::endl;
    }
    int res = ft.get();
    {
        std::lock_guard<std::mutex> lg(io);
        std::cout << "-- Data is ready! data = " << res << std::endl;
    }
}

int main() {
    run_home_work();
    system("pause");
    run_tasks();
    system("pause");
    run_not();
    system("pause");
    run_mutex();
    system("pause");
    run_timed_mutex();
    system("pause");
    run_recursive_mutex();
    system("pause");
    run_shared_mutex();
    system("pause");
    run_scoped_mutex();
    system("pause");
    run_lock_struct();
    system("pause");
    run_conditional_variable();
    system("pause");
    run_promise();
    system("pause");
    run_packaged_task();
    system("pause");
    run_async();
    system("pause");
    return 0;
}
