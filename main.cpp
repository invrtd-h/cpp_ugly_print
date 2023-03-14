#include "ugly_print.h"

#include <deque>
#include <stack>

struct MyStruct {
    int x, y;
    char c, d;
    
    constexpr static const char* mem_names[] = {"x_val", "y_val", "c_val", "d_val"};
};

struct MyStruct2 {
    int x, y;
    const char *name;
    
    constexpr static int mem_num = 3;
};

int main() {
    auto my_instance = MyStruct{1, 2, 'U', 'K'};
    std::cout << ugly::decay_to_str(my_instance) << '\n';
    
    auto my_instance2 = MyStruct2{6, 9, "instance 2"};
    std::cout << ugly::decay_to_str(my_instance2) << '\n';
    
    std::stack<int> s;
    s.push(1);
    s.push(3);
    s.push(9);
    
    std::cout << ugly::decay_to_str(s) << '\n';
}
