#ifndef UGLY_PRINT_UGLY_PRINT_H
#define UGLY_PRINT_UGLY_PRINT_H

#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>

namespace ugly::test {
    struct unprintable {};
    
    struct has_three_members {
        int n;
        char c;
        void *p;
        
        constexpr static int mem_num = 3;
    };
    
    struct my_pair {
        int first;
        char second;
    };
}

namespace ugly::tmf {
    struct yes {
        constexpr static bool value = true;
    };
    struct no {
        constexpr static bool value = false;
    };
    
    template<typename... Ts>
    constexpr bool true_v = true;
}

namespace ugly::tmf::detail {
    template<typename T>
    std::enable_if_t<
            true_v<typename std::void_t<decltype(
            std::cout << std::declval<T>()
            )>>,
            yes>
    is_printable(T t) {
        return yes{};
    }
    
    no is_printable(...) {
        return no{};
    }
}

namespace ugly::tmf {
    template<typename T>
    constexpr bool is_printable_v =
            decltype(detail::is_printable(std::declval<T>()))::value;
    
    static_assert(is_printable_v<int>);
    static_assert(is_printable_v<int ******>);
    static_assert(!is_printable_v<test::unprintable>);
    
    template<typename T>
    constexpr bool priority_00 = is_printable_v<T>;
}

namespace ugly::tmf::detail {
    template<typename T>
    std::enable_if_t<
            priority_00<decltype(std::declval<T>().first)> and
            priority_00<decltype(std::declval<T>().second)>,
            yes>
    is_pair(T &&t) {
        return yes{};
    }
    
    no is_pair(...) {
        return no{};
    }
}

namespace ugly::tmf {
    template<typename T>
    constexpr bool is_well_pair_v =
            decltype(detail::is_pair(std::declval<T>()))::value;
    
    static_assert(!is_well_pair_v<int>);
    static_assert(is_well_pair_v<test::my_pair>);
    static_assert(is_well_pair_v<std::pair<int, int>>);
    static_assert(!is_well_pair_v<std::pair<int, test::unprintable>>);
    
    template<typename T>
    constexpr bool priority_01 = !priority_00<T> and is_well_pair_v<T>;
}

namespace ugly {
    template<typename T>
    auto decay_to_str(T &&t)
    -> std::enable_if_t<tmf::priority_00<std::decay_t<T>>, std::string>
    {
        std::stringstream ss;
        ss << std::forward<T>(t);
        
        std::string ret;
        ss >> ret;
        
        return ret;
    }
    
    template<typename T>
    auto decay_to_str(T &&t)
    -> std::enable_if_t<tmf::priority_01<std::decay_t<T>>, std::string>
    {
        std::stringstream ss;
        ss << '<' << t.first << ", " << t.second << '>';
        
        std::string ret(ss.str());
        return ret;
    }
}

namespace ugly {
    struct Printer {
        template<typename T>
        auto operator<<(T&& t)
        -> std::enable_if_t<tmf::priority_00<std::decay_t<T>>, Printer&>
        {
            std::cout << std::forward<T>(t);
            return *this;
        }
    
    };
    
    inline auto printer = Printer{};
}

#endif
