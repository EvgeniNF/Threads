#include <iostream>
#include <thread>

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

int main() {
    run_home_work();
    return 0;
}
