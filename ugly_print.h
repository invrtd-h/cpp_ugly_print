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
    
    struct my_named_tuple {
        [[maybe_unused]] int n;
        [[maybe_unused]] char c;
        
        constexpr static const char* mem_names[] = {"integer", "character"};
    
        static_assert(std::is_same_v<std::decay_t<decltype(mem_names)>, const char *const *>);
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
    template<typename>
    struct TD;
    
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
        
        static auto test() {
            if constexpr (PredPriority::template now<T>::value) {
                return typename PredPriority::template now<T>{};
            } else {
                return priority_test<T, typename PredPriority::next>::test();
            }
        }
        
        using match_type = decltype(test());
    };
    
    template<typename T, typename PredPriority>
    inline constexpr int priority_test_v =
            priority_test<T, PredPriority>::priority;
    
    template<typename T, typename PredPriority>
    using priority_test_t =
            typename priority_test<T, PredPriority>::match_type;
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
}

namespace ugly::tmf::detail {
    template<typename T>
    inline std::enable_if_t<std::is_same_v<
            std::decay_t<decltype(T::mem_names)>, const char *const *>,
            yes>
    is_named_tuple(T) {
        return yes{};
    }
    
    [[maybe_unused]]
    inline no is_named_tuple(...) {
        return no{};
    }
}

namespace ugly::tmf {
    template<typename T>
    struct is_named_tuple {
        constexpr static bool value =
                decltype(detail::is_named_tuple(std::declval<T>()))::value;
    };
    
    template<typename T>
    inline constexpr bool is_named_tuple_v = is_named_tuple<T>::value;
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
}

namespace ugly::tmf::detail {
    template<typename T>
    inline std::enable_if_t<true_v<
            decltype(std::declval<T>().begin() == std::declval<T>().end()),
            decltype(++std::declval<T>().begin()),
            decltype(*std::declval<T>().begin())
            >, yes>
    is_container(T) {
        return yes{};
    }
    
    [[maybe_unused]]
    inline no is_container(...) {
        return no{};
    }
}

namespace ugly::tmf {
    template<typename T>
    struct is_container {
        constexpr static bool value =
                decltype(detail::is_container(std::declval<T>()))::value;
    };
    
    template<typename T>
    inline constexpr bool is_container_v = is_container<T>::value;
}

namespace ugly::tmf {
    using priority_set = unary_predicates_priorities<
            is_printable,
            is_well_pair,
            is_named_tuple,
            is_unpackable,
            is_container
            >;
}

namespace ugly {
    using priority_set = tmf::priority_set;
    
    template<typename T>
    struct total_priority_check {
        constexpr static int value = tmf::priority_test_v<T, priority_set>;
        using type = tmf::priority_test_t<T, priority_set>;
    };
    
    template<typename T>
    inline constexpr int total_priority_check_v =
            total_priority_check<T>::value;
    
    template<typename T>
    using total_priority_check_t =
            typename total_priority_check<T>::type;
    
    static_assert(total_priority_check_v<int> == 0);
    static_assert(total_priority_check_v<std::pair<int, int>> == 1);
    
    static_assert(std::is_same_v<
            total_priority_check_t<int>, tmf::is_printable<int>>);
    
    template<typename T, template<class> class P>
    struct match_with {
        constexpr static bool value =
                std::is_same_v<total_priority_check_t<T>, P<T>>;
    };
    
    template<typename T, template<class> class P>
    inline constexpr bool match_with_v = match_with<T, P>::value;
}

namespace ugly {
    
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<match_with_v<T, tmf::is_printable>, std::string>
    {
        std::stringstream ss;
        ss << std::forward<T>(t);
        
        return ss.str();
    }
    
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<match_with_v<T, tmf::is_well_pair>, std::string>
    {
        std::stringstream ss;
        ss << '<' << decay_to_str(t.first) << ", "
           << decay_to_str(t.second) << '>';
        
        return ss.str();
    }
    
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<match_with_v<T, tmf::is_named_tuple>, std::string>
    {
        using RT = std::remove_reference_t<T>;
        constexpr int SIZE = std::extent_v<decltype(RT::mem_names)>;
        const auto& names = RT::mem_names;
        
        std::stringstream ss;
        if constexpr (SIZE == 0) {
            return std::string("()");
        } else if constexpr (SIZE == 1) {
            const auto& [a] = t;
            ss << '(' << names[0] << ": " << decay_to_str(a) << ')';
            return ss.str();
        } else if constexpr (SIZE == 2) {
            const auto& [a, b] = t;
            ss << '('
            << names[0] << ": " << decay_to_str(a) << ", "
            << names[1] << ": " << decay_to_str(b) << ')';
            return ss.str();
        } else if constexpr (SIZE == 3) {
            const auto& [a, b, c] = t;
            ss << '('
               << names[0] << ": " << decay_to_str(a)
               << names[1] << ": " << decay_to_str(b)
               << names[2] << ": " << decay_to_str(c) << ')';
            return ss.str();
        }

        else {
            ss << "<object at " << &t << ">";
            return ss.str();
        }
    }
    
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<match_with_v<T, tmf::is_unpackable>, std::string>
    {
        using RT = std::remove_reference_t<T>;
        
        std::stringstream ss;
        if constexpr (RT::mem_num == 0) {
            return std::string("()");
        } else if constexpr (RT::mem_num == 1) {
            const auto& [a] = t;
            ss << '(' << decay_to_str(a) << ')';
            return std::string{ss.str()};
        } else if constexpr (RT::mem_num == 2) {
            const auto& [a, b] = t;
            ss << '(' << decay_to_str(a) << ", " << decay_to_str(b) << ')';
            return std::string{ss.str()};
        } else if constexpr (RT::mem_num == 3) {
            const auto& [a, b, c] = t;
            ss << '(' << decay_to_str(a) << ", " << decay_to_str(b) << ", "
                      << decay_to_str(c) << ')';
            return std::string{ss.str()};
        } else if constexpr (RT::mem_num == 4) {
            const auto& [a, b, c, d] = t;
            ss << '(' << decay_to_str(a) << ", " << decay_to_str(b) << ", "
                      << decay_to_str(c) << ", " << decay_to_str(d) << ')';
            return std::string{ss.str()};
        } else if constexpr (RT::mem_num == 5) {
            const auto& [a, b, c, d, e] = t;
            ss << '('
               << decay_to_str(a) << ", " << decay_to_str(b) << ", "
               << decay_to_str(c) << ", " << decay_to_str(d) << ", "
               << decay_to_str(e) << ")";
            return std::string{ss.str()};
        }
        
        else {
            ss << "<object at " << &t << ">";
            return std::string{ss.str()};
        }
    }
    
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<match_with_v<T, tmf::is_container>, std::string>
    {
        std::stringstream ss;
        ss << "[";
        for (auto it = t.begin(); it != t.end(); ++it) {
            ss << decay_to_str(*it) << ", ";
        }
        
        auto ret = ss.str();
        ret.pop_back();
        ret.back() = ']';
        
        return ret;
    }
    
    template<typename T>
    inline auto decay_to_str(T &&t, int head_size)
    -> std::enable_if_t<match_with_v<T, tmf::is_container>, std::string>
    {
        std::stringstream ss;
        ss << "[";
        int i = 0;
        auto it = t.begin();
        for (; it != t.end() and i < head_size; ++it) {
            ss << decay_to_str(*it) << ", ";
            ++i;
        }
        
        if (it != t.end()) {
            ss << "...]";
            return ss.str();
        }
        
        auto ret = ss.str();
        ret.pop_back();
        ret.back() = ']';
        
        return ret;
    }
    
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<match_with_v<T, tmf::true_t>, std::string>
    {
        std::stringstream ss;
        ss << "<object at " << &t << ">";
        return std::string{ss.str()};
    }
}

#endif
