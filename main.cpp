#include "lib/ugly_print_20.h"
#include <unordered_map>
#include <cassert>

template<typename T>
struct TD;

using ugly::dout;

void test() {
    std::vector<int> obj_vec1{9, 1, 3};
    assert(dout.std_repr(obj_vec1) == "[9, 1, 3]");
    
    int obj1[] = {9, 1, 3};
    assert(dout.std_repr(obj1) == "[9, 1, 3]");
    
    int obj2[3][3] = {
            {9, 1, 3},
            {1, 3, 9},
            {3, 9, 1},
    };
    
    assert(dout.std_repr(obj2) == "[\n"
                                  "    [9, 1, 3],\n"
                                  "    [1, 3, 9],\n"
                                  "    [3, 9, 1]\n"
                                  "]");
}

int main() {
    test();
    
    auto tup1 = std::make_tuple(1, 2, "str-val");
    dout << tup1;
    
    dout.set_null_val(1);
    dout << 1;
    dout << 2;
}
