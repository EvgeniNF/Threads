#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
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
// Работа promise


using namespace std;
int main() {
    run_tasks();
    return 0;
}
