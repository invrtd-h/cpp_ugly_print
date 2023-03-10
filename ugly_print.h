#ifndef UGLY_PRINT_UGLY_PRINT_H
#define UGLY_PRINT_UGLY_PRINT_H

#include <iostream>

namespace de {
    namespace test {
        struct unprintable {};
    }
    
    struct yes {
        constexpr static bool value = true;
    };
    struct no {
        constexpr static bool value = false;
    };
    
    template<typename... Ts>
    constexpr bool true_v = true;
    
    
}

namespace de::detail {
    template<typename... Preds>
    struct priority;
    
}

namespace de::detail {
    template<typename T>
    std::enable_if_t<
            true_v<typename std::void_t<decltype(
            std::cout << std::declval<T>()
            )>>,
            yes> is_printable(T t) {
        return yes{};
    }
    
    no is_printable(...) {
        return no{};
    }
}

namespace de {
    template<typename T>
    constexpr bool is_printable_v =
            decltype(detail::is_printable(std::declval<T>()))::value;
    
    static_assert(is_printable_v<int>);
    static_assert(is_printable_v<int ******>);
    static_assert(!is_printable_v<test::unprintable>);
}

#endif
