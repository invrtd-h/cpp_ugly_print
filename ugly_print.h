#ifndef UGLY_PRINT_UGLY_PRINT_H
#define UGLY_PRINT_UGLY_PRINT_H

#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>

namespace ugly::test {
    struct unprintable {};
    
    struct has_three_members {
        [[maybe_unused]] int n;
        [[maybe_unused]] char c;
        [[maybe_unused]] void *p;
        
        constexpr static int mem_num = 3;
    };
    
    struct has_four_members {
        [[maybe_unused]] int a, b, c, d;
        constexpr static int mem_num = 4;
    };
    
    struct my_pair {
        int first;
        char second;
    };
    
    template<typename>
    struct my_unary_pred {
        constexpr static bool value = true;
    };
    
    template<typename>
    struct my_unary_non_pred_1 {
        constexpr static int value = 5;
    };
    
    template<typename>
    struct my_unary_non_pred_2 {
        [[maybe_unused]]
        constexpr static bool val = true;
    };
}

namespace ugly::tmf {
    struct yes {
        constexpr static bool value = true;
    };
    struct no {
        constexpr static bool value = false;
    };
    
    template<typename...>
    inline constexpr bool true_v = true;
    
    template<typename...>
    struct true_t {
        constexpr static bool value = true;
    };
}

// is_unary_pred template meta function
namespace ugly::tmf::detail {
    
    template<template<class> class TTP, typename T>
    inline std::enable_if_t<std::is_same_v<bool,
            std::decay_t<decltype(TTP<T>::value)>>,
            yes> is_unary_pred_impl(TTP<T>) {
        return yes{};
    }
    
    [[maybe_unused]]
    inline no is_unary_pred_impl(...) {
        return no{};
    }
}

namespace ugly::tmf {
    
    template<template<class> class TTP>
    struct is_unary_pred {
        constexpr static bool value =
        decltype(detail::is_unary_pred_impl(std::declval<TTP<int>>()))::value;
    };
    
    template<template<class> class TTP>
    inline constexpr bool is_unary_pred_v = is_unary_pred<TTP>::value;
    
    static_assert(!is_unary_pred_v<test::my_unary_non_pred_1>);
    static_assert(!is_unary_pred_v<test::my_unary_non_pred_2>);
    static_assert(is_unary_pred_v<test::my_unary_pred>);
    
    template<template<class> class TTP, template<class> class... TTPs>
    struct is_all_unary_pred {
        constexpr static bool value =
        is_unary_pred_v<TTP> and is_all_unary_pred<TTPs...>::value;
    };
    
    template<template<class> class TTP>
    struct is_all_unary_pred<TTP> {
        constexpr static bool value = is_unary_pred_v<TTP>;
    };
    
    template<template<class> class TTP, template<class> class... TTPs>
    inline constexpr bool is_all_unary_pred_v =
            is_all_unary_pred<TTP, TTPs...>::value;
    
    static_assert(is_all_unary_pred_v<test::my_unary_pred>);
    static_assert(!is_all_unary_pred_v<test::my_unary_pred,
            test::my_unary_non_pred_1>);
}
// is_unary_pred template meta function end

namespace ugly::tmf::detail {

    struct unary_predicates_enum_base {};
    
    template<template<class> class FirstUnaryPred,
            template<class> class... UnaryPreds>
    struct unary_predicates_enum;
    
    template<template<class> class FirstUnaryPred>
    struct unary_predicates_enum<FirstUnaryPred> : unary_predicates_enum_base {
        using next [[maybe_unused]] = unary_predicates_enum<true_t>;
        
        template<typename T>
        using now [[maybe_unused]] = FirstUnaryPred<T>;
    
        static_assert(is_unary_pred_v<FirstUnaryPred>,
                      "The template parameters should take one type param "
                      "and return a boolean value by ::value");
    };
    
    template<template<class> class FirstUnaryPred,
            template<class> class... UnaryPreds>
    struct unary_predicates_enum : unary_predicates_enum_base {
        using next [[maybe_unused]] = unary_predicates_enum<UnaryPreds...>;
        
        template<typename T>
        using now [[maybe_unused]] = FirstUnaryPred<T>;
    
        static_assert(is_all_unary_pred_v<FirstUnaryPred, UnaryPreds...>,
                      "The template parameters should take one type param "
                      "and return a boolean value by ::value");
    };
}

namespace ugly::tmf {
    
    template<template<class> class FirstUnaryPred,
            template<class> class... UnaryPreds>
    using unary_predicates_priorities =
            detail::unary_predicates_enum<FirstUnaryPred, UnaryPreds...>;
    
    using unary_predicates_priorities_base =
            detail::unary_predicates_enum_base;
    
    template<typename T, typename PredPriority>
    struct priority_test {
        static_assert(std::is_base_of<
                unary_predicates_priorities_base, PredPriority
                >::value,
                "The 2nd template argument should be the instance type of "
                "the template ugly::tmf::unary_predicates_priorities");
        
        constexpr static int priority =
                PredPriority::template now<T>::value ? 0 :
                priority_test<T, typename PredPriority::next>::priority + 1;
    };
    
    template<typename T, typename PredPriority>
    inline constexpr int priority_test_v =
            priority_test<T, PredPriority>::priority;
}

namespace ugly::tmf::detail {
    template<typename T>
    inline std::enable_if_t<
            true_v<typename std::void_t<decltype(
            std::cout << std::declval<T>()
            )>>,
            yes>
    is_printable(T) {
        return yes{};
    }
    
    [[maybe_unused]]
    inline no is_printable(...) {
        return no{};
    }
}

namespace ugly::tmf {
    template<typename T>
    struct is_printable {
        constexpr static bool value =
                decltype(detail::is_printable(std::declval<T>()))::value;
    };
    
    template<typename T>
    inline constexpr bool is_printable_v =
            decltype(detail::is_printable(std::declval<T>()))::value;
    
    static_assert(is_printable_v<int>);
    static_assert(is_printable_v<int ******>);
    static_assert(!is_printable_v<test::unprintable>);
    
    template<typename T>
    inline constexpr bool priority_00 = is_printable_v<T>;
}

namespace ugly::tmf::detail {
    template<typename T>
    inline std::enable_if_t<
            priority_00<decltype(std::declval<T>().first)> and
            priority_00<decltype(std::declval<T>().second)>,
            yes>
    is_pair(T) {
        return yes{};
    }
    
    [[maybe_unused]]
    inline no is_pair(...) {
        return no{};
    }
}

namespace ugly::tmf {
    template<typename T>
    struct is_well_pair {
        constexpr static bool value =
                decltype(detail::is_pair(std::declval<T>()))::value;
    };
    
    template<typename T>
    inline constexpr bool is_well_pair_v =
            decltype(detail::is_pair(std::declval<T>()))::value;
    
    static_assert(!is_well_pair_v<int>);
    static_assert(is_well_pair_v<test::my_pair>);
    static_assert(is_well_pair_v<std::pair<int, int>>);
    static_assert(!is_well_pair_v<std::pair<int, test::unprintable>>);
    
    template<typename T>
    inline constexpr bool priority_01 = !priority_00<T> and is_well_pair_v<T>;
}

namespace ugly::tmf::detail {
    template<typename T>
    inline std::enable_if_t<true_v<decltype(T::mem_num)>,
            yes>
    is_unpackable(T) {
        return yes{};
    }
    
    [[maybe_unused]]
    inline no is_unpackable(...) {
        return no{};
    }
}

namespace ugly::tmf {
    template<typename T>
    struct is_unpackable {
        constexpr static bool value =
                decltype(detail::is_unpackable(std::declval<T>()))::value;
    };
    
    template<typename T>
    inline constexpr bool is_unpackable_t =
            decltype(detail::is_unpackable(std::declval<T>()))::value;
    
    static_assert(is_unpackable_t<test::has_three_members>);
    static_assert(!is_unpackable_t<int>);
    
    template<typename T>
    inline constexpr bool priority_02 = !priority_01<T> and is_unpackable_t<T>;
}

namespace ugly::tmf {
    using priority_set = unary_predicates_priorities<
            is_printable,
            is_well_pair,
            is_unpackable
            >;
}

namespace ugly {
    using priority_set = tmf::priority_set;
    
    template<typename T>
    struct total_priority_check {
        constexpr static int value = tmf::priority_test_v<T, priority_set>;
    };
    
    template<typename T>
    inline constexpr int total_priority_check_v =
            total_priority_check<T>::value;
    
    static_assert(total_priority_check_v<int> == 0);
    static_assert(total_priority_check_v<std::pair<int, int>> == 1);
}

namespace ugly {
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<tmf::priority_00<std::decay_t<T>>, std::string>
    {
        std::stringstream ss;
        ss << std::forward<T>(t);
        
        std::string ret;
        ss >> ret;
        
        return ret;
    }
    
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<tmf::priority_01<std::decay_t<T>>, std::string>
    {
        std::stringstream ss;
        ss << '<' << t.first << ", " << t.second << '>';
        
        std::string ret(ss.str());
        return ret;
    }
    
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<tmf::priority_02<std::decay_t<T>>, std::string>
    {
        std::stringstream ss;
        if constexpr (T::mem_num == 0) {
            return std::string("()");
        } else if constexpr (T::mem_num == 3) {
            const auto& [a, b, c] = t;
            ss << '(' << a << ", " << b << ", " << c << ')';
            return std::string{ss.str()};
        } else {
            ss << "<object at " << &t << ">";
            return std::string{ss.str()};
        }
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
    
        template<typename T>
        auto operator<<(T&& t)
        -> std::enable_if_t<tmf::priority_01<std::decay_t<T>>, Printer&>
        {
            std::cout << '<' << t.first << ", " << t.second << '>';
            return *this;
        }
    };
    
    inline auto printer = Printer{};
}

#endif
