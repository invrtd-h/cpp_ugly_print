#include "lib/ugly_print_20.h"
#include <unordered_map>
#include <cassert>
#include <set>
#include <format>
#include <regex>

template<typename T>
struct TD;

using ugly::dout;

struct TestClass1 {
    int x, y;
    constexpr static int mem_num = 2;
};

struct NoRule {
    int x = 0;
    std::bitset<913> bitset;
};

void test() {
    //tuple test
    {
        auto tup1 = std::make_tuple(1, 2, "str-val");
        assert(dout.std_repr(tup1) == "(1, 2, str-val)");
    }
    
    //std::vector test
    {
        std::vector<int> obj_vec1{9, 1, 3};
        assert(dout.std_repr(obj_vec1) == "[9, 1, 3]");
    }
    
    //c-array test
    {
        int obj1[] = {9, 1, 3};
        assert(dout.std_repr(obj1) == "[9, 1, 3]");
    }
    
    //c-array(2D) test
    {
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
    
    //raw pointer test
    {
        int n = 4;
        int *p = &n;
        std::regex regex(R"(&: 0x[0-9a-f]+, \*: 4)");
        assert(std::regex_match(dout.std_repr(p), regex));
    }
    
    //custom class test
    {
        auto test_class_obj_1 = TestClass1{6, 9};
        assert(dout.std_repr(test_class_obj_1) == "(6, 9)");
    }
    
    //std::set test
    {
        std::set<int> obj3{77, 1, 8, 99, 4};
        assert(dout.std_repr(obj3) == "[1, 4, 8, 77, 99]");
    }
    
    //custom class test
    {
        NoRule no_rule;
        std::regex no_rule_regex(R"(<obj 6NoRule @0x[0-9a-f]+>)");
        assert(std::regex_match(dout.std_repr(no_rule), no_rule_regex));
    }
}

int main() {
    test();
    
    std::bitset<17> bi = 161;
    dout << bi;
    
    auto up = std::make_unique<double>(3.14);
    dout << up;
    
    auto op = std::make_optional<double>(3.14);
    dout << op;
    
    std::optional<int> null_op = std::nullopt;
    dout << null_op;
}

static_assert(ugly::detail::pointer_like<std::optional<int>>);