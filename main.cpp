#include "ugly_print.h"

int main() {
    std::cout << ugly::decay_to_str(1) << '\n';
    void *p = nullptr;
    std::cout << ugly::decay_to_str(p) << '\n';
    
    std::cout << ugly::decay_to_str(std::make_pair(1, "what")) << '\n';
}
