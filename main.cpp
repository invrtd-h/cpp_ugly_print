#include "lib/ugly_print_20.h"
#include <unordered_map>

template<typename T>
struct TD;

int main() {
    dout << 1 << 2 << "str" << std::pair<int, char>(3, 'k');
    
    std::unordered_map<int, int> map{{1, 1}, {3, 3}, {6, 7}};
    std::pair<const int, int> y{1, 3};
    for (auto &x : map) {
        dout << x;
    }
    dout << map;
}
