#include "ugly_print_20.h"

using ugly::dout;

int main() {
    dout << 1 << 2 << "str" << std::pair<int, char>(3, 'k');
}
