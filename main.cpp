#include "ugly_print.h"

#include <deque>
#include <stack>

using ugly::dout;

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

using int3 = int[3];
void test(int3 &tt) {
    static_assert(std::is_same_v<std::remove_reference_t<decltype(tt)>, int[3]>);
}

int main() {
    auto my_instance = MyStruct{1, 2, 'U', 'K'};
    dout << ugly::sep_set("\n") << my_instance;
    
    auto my_instance2 = MyStruct2{6, 9, "instance 2"};
    dout << my_instance2 << '\n';
    
    std::stack<int> s;
    s.push(1);
    s.push(3);
    s.push(9);
    
    dout << ugly::wrap_off << s;
}
