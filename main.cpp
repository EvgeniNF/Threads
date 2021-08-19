#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>
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
std::shared_ptr<std::vector<int>> show(std::shared_ptr<std::vector<int>> vec){
    int size = static_cast<int>(vec->size());
    std::cout << "-- Vector size: " << size << " vector data: ";
    for(int i = 0; i < size; i++){
        std::cout << vec->at(i) << " | ";
    }
    std::cout << std::endl;
    return std::move(vec);
}

std::shared_ptr<std::vector<int>> show_and_del(std::shared_ptr<std::vector<int>> vec,
                  std::mutex &mtx){
    mtx.lock();
    if(!(vec->empty())){
        try{
            vec = show(std::move(vec));
            vec->erase(vec->begin(), vec->end());
        }
        catch (const char* exception){
            std::cout << exception << std::endl;
        }
    }
    mtx.unlock();
    return std::move(vec);
}

std::shared_ptr<std::vector<int>> add_ell(std::shared_ptr<std::vector<int>> vec,
             int n, std::mutex &mtx){
    mtx.lock();
    vec->push_back(n);
    mtx.unlock();
    return std::move(vec);
}

void run_vector(){
    auto vector = std::make_shared<std::vector<int>>();
    std::mutex mtx;
    for (int i = 0; i <= 1000; i++){
        std::thread th_1([&vector, &i, &mtx] () { vector = add_ell(std::move(vector), i, std::ref(mtx)); } );
        std::thread th_2([&vector, &mtx] () { vector = show_and_del(std::move(vector), std::ref(mtx)); } );
        th_1.join();
        th_2.join();
    }
}

int main() {
    run_vector();
    system("pause");
    return 0;
}
