#include "lib/ugly_print_20.h"
#include <unordered_map>

template<typename T>
struct TD;

int main() {
    auto vec1 = std::vector{1, 2, 3};
    auto vec2 = std::vector{
        std::vector{1, 2, 3},
        std::vector{4, 5, 6},
        std::vector{7, 8}
    };
    auto vec3 = std::vector{
        std::vector{
            std::vector{1, 2},
            std::vector{3, 4},
        }, std::vector{
            std::vector{5, 6},
            std::vector{7},
        }
    };
    
    dout << vec1;
    dout << vec2;
    dout << vec3;
}
