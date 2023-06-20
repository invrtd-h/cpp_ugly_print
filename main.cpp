#include "lib/ugly_print_20.h"
#include <unordered_map>
#include <cassert>
#include <set>
#include <format>

template<typename T>
struct TD;

using ugly::dout;

struct TestClass1 {
    int x, y;
    constexpr static int mem_num = 2;
};

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
    
    auto test_class_obj_1 = TestClass1{6, 9};
    assert(dout.std_repr(test_class_obj_1) == "(6, 9)");
    
    std::set<int> obj3{77, 1, 8, 99, 4};
    assert(dout.std_repr(obj3) == "[1, 4, 8, 77, 99]");
    
    std::stack<int> obj4;
    obj4.push(1);
    obj4.push(7);
    obj4.push(3);
    dout << obj4;
}

int main() {
    test();
    
    auto tup1 = std::make_tuple(1, 2, "str-val");
    dout << tup1;
    
    int n = 4;
    int *p = &n;
    dout << p;
    
    auto up = std::make_unique<double>(3.14);
    dout << up;
    
    auto op = std::make_optional<double>(3.14);
    dout << op;
}

static_assert(ugly::detail::pointer_like<std::optional<int>>);