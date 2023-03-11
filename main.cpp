#include "ugly_print.h"

int main() {
    std::cout << ugly::decay_to_str(1) << '\n';
    void *p = nullptr;
    std::cout << ugly::decay_to_str(p) << '\n';
    
    std::cout << ugly::decay_to_str(std::make_pair(1, "what")) << '\n';
    
    std::cout << ugly::decay_to_str(ugly::test::has_three_members{6, 'a', &std::cout}) << '\n';
    
    std::cout << ugly::decay_to_str(ugly::test::has_four_members{1, 2, 3, 4}) << '\n';
}
