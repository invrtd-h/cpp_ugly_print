#include "ugly_print.h"

#include <vector>
#include <unordered_map>

int main() {
    std::cout << ugly::decay_to_str(1) << '\n';
    void *p = nullptr;
    std::cout << ugly::decay_to_str(p) << '\n';
    
    std::cout << ugly::decay_to_str(std::make_pair(1, "what")) << '\n';
    
    std::cout << ugly::decay_to_str(ugly::test::has_three_members{6, 'a', &std::cout}) << '\n';
    
    auto x = ugly::test::has_three_members{9, 'x', &std::cin};
    std::cout << ugly::decay_to_str(x) << '\n';
    
    std::cout << ugly::decay_to_str(ugly::test::has_four_members{1, 2, 3, 4}) << '\n';
    std::cout << ugly::decay_to_str(ugly::test::my_named_tuple{7, 'a'}) << '\n';
    
    std::cout << ugly::decay_to_str(std::vector<int>{6, 9, 7, 4}) << '\n';
    std::cout << ugly::decay_to_str(std::vector{std::vector{1, 2, 3, 4}, std::vector{5, 6, 7}}) << '\n';
    std::cout << ugly::decay_to_str(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}, 7) << '\n';
    
    std::cout << ugly::decay_to_str(std::unordered_map<int, int>{{1, 3}, {2, 4}, {3, 5}}) << '\n';
    
    std::cout << ugly::decay_to_str(std::cout) << '\n';
}
