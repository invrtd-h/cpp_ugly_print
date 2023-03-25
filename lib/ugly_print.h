#ifndef UGLY_PRINT_UGLY_PRINT_H
#define UGLY_PRINT_UGLY_PRINT_H

#include <iostream>
#include <regex>
#include <string>
#include <sstream>
#include <type_traits>
#include <utility>

namespace ugly::test {
    struct unprintable {};
    
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
    struct [[maybe_unused]] TD;
    
    struct yes {
        constexpr static bool value = true;
    };
    struct no {
        constexpr static bool value = false;
    };
    
    template<typename...>
    inline constexpr bool true_v = true;
    
    template<typename>
    struct true_t {
        constexpr static bool value = true;
    };
}

namespace ugly::cmp::detail {
    
    template<typename T, typename U>
    constexpr auto is_eq_universal(T &&t, U &&u) noexcept
    -> std::enable_if_t<std::is_same_v<bool, decltype(t == u)>, tmf::yes>
    {
        return tmf::yes{};
    }
    
    [[maybe_unused]]
    constexpr auto is_eq_universal(...) noexcept -> tmf::no {
        return tmf::no{};
    }
}

namespace ugly::cmp {
    /**
     * Universal equality compare function.
     * @return t == u, if expr "t == u" is well-defined boolean.
     * false otherwise.
     */
    template<typename T, typename U>
    constexpr auto is_eq_universal(T &&t, U &&u) noexcept -> bool {
        if constexpr (decltype(detail::is_eq_universal(
                std::forward<T>(t), std::forward<U>(u)))::value)
        {
            return t == u;
        }
        
        return false;
    }
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
}

namespace ugly::tmf::detail {
    template<typename T>
    inline std::enable_if_t<true_v<
            decltype(std::declval<T>().pop()),
            decltype(std::declval<T>().top()),
            decltype(std::declval<T>().empty())
    >, yes>
    is_stack_like(T) {
        return yes{};
    }
    
    [[maybe_unused]]
    inline no is_stack_like(...) {
        return no{};
    }
    
    template<typename T>
    inline std::enable_if_t<true_v<
            decltype(std::declval<T>().pop()),
            decltype(std::declval<T>().front()),
            decltype(std::declval<T>().empty())
    >, yes>
    is_queue_like(T) {
        return yes{};
    }
    
    [[maybe_unused]]
    inline no is_queue_like(...) {
        return no{};
    }
}

namespace ugly::tmf {
    template<typename T>
    struct is_stack_like {
        constexpr static bool value =
                decltype(detail::is_stack_like(std::declval<T>()))::value;
    };
    
    template<typename T>
    struct is_queue_like {
        constexpr static bool value =
                decltype(detail::is_queue_like(std::declval<T>()))::value;
    };
}

namespace ugly::tmf {
    using priority_set = unary_predicates_priorities<
            is_printable,
            is_well_pair,
            is_named_tuple,
            is_unpackable,
            is_container,
            is_stack_like,
            is_queue_like
    >;
}

namespace ugly::tmf {
    
    template<typename T>
    struct total_priority_check {
        constexpr static int value = tmf::priority_test_v<T, priority_set>;
        using type = tmf::priority_test_t<T, priority_set>;
    };
    
    template<typename T>
    [[maybe_unused]]
    inline constexpr int total_priority_check_v =
            total_priority_check<T>::value;
    
    template<typename T>
    using total_priority_check_t =
            typename total_priority_check<T>::type;
    
    static_assert(std::is_same_v<
            total_priority_check_t<int>, tmf::is_printable<int>>);
}

namespace ugly {
    
    template<typename T, template<class> class P>
    struct match_with {
        constexpr static bool value =
                std::is_same_v<tmf::total_priority_check_t<T>, P<T>>;
    };
    
    template<typename T, template<class> class P>
    inline constexpr bool match_with_v = match_with<T, P>::value;
}

/**
 * Implementation of ugly::decay_to_str.
 */
namespace ugly::impl {
    
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
               << names[0] << ": " << decay_to_str(a) << ", "
               << names[1] << ": " << decay_to_str(b) << ", "
               << names[2] << ": " << decay_to_str(c)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 4) {
            const auto& [a, b, c, d] = t;
            ss << '('
               << names[0] << ": " << decay_to_str(a) << ", "
               << names[1] << ": " << decay_to_str(b) << ", "
               << names[2] << ": " << decay_to_str(c) << ", "
               << names[3] << ": " << decay_to_str(d)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 5) {
            const auto& [a, b, c, d, e] = t;
            ss << '('
               << names[0] << ": " << decay_to_str(a) << ", "
               << names[1] << ": " << decay_to_str(b) << ", "
               << names[2] << ": " << decay_to_str(c) << ", "
               << names[3] << ": " << decay_to_str(d) << ", "
               << names[4] << ": " << decay_to_str(e)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 6) {
            const auto& [a, b, c, d, e, f] = t;
            ss << '('
               << names[0] << ": " << decay_to_str(a) << ", "
               << names[1] << ": " << decay_to_str(b) << ", "
               << names[2] << ": " << decay_to_str(c) << ", "
               << names[3] << ": " << decay_to_str(d) << ", "
               << names[4] << ": " << decay_to_str(e) << ", "
               << names[5] << ": " << decay_to_str(f)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 7) {
            const auto& [a, b, c, d, e, f, g] = t;
            ss << '('
               << names[0] << ": " << decay_to_str(a) << ", "
               << names[1] << ": " << decay_to_str(b) << ", "
               << names[2] << ": " << decay_to_str(c) << ", "
               << names[3] << ": " << decay_to_str(d) << ", "
               << names[4] << ": " << decay_to_str(e) << ", "
               << names[5] << ": " << decay_to_str(f) << ", "
               << names[6] << ": " << decay_to_str(g)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 8) {
            const auto& [a, b, c, d, e, f, g, h] = t;
            ss << '('
               << names[0] << ": " << decay_to_str(a) << ", "
               << names[1] << ": " << decay_to_str(b) << ", "
               << names[2] << ": " << decay_to_str(c) << ", "
               << names[3] << ": " << decay_to_str(d) << ", "
               << names[4] << ": " << decay_to_str(e) << ", "
               << names[5] << ": " << decay_to_str(f) << ", "
               << names[6] << ": " << decay_to_str(g) << ", "
               << names[7] << ": " << decay_to_str(h)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 9) {
            const auto& [a, b, c, d, e, f, g, h, i] = t;
            ss << '('
               << names[0] << ": " << decay_to_str(a) << ", "
               << names[1] << ": " << decay_to_str(b) << ", "
               << names[2] << ": " << decay_to_str(c) << ", "
               << names[3] << ": " << decay_to_str(d) << ", "
               << names[4] << ": " << decay_to_str(e) << ", "
               << names[5] << ": " << decay_to_str(f) << ", "
               << names[6] << ": " << decay_to_str(g) << ", "
               << names[7] << ": " << decay_to_str(h) << ", "
               << names[8] << ": " << decay_to_str(i)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 10) {
            const auto& [a, b, c, d, e, f, g, h, i, j] = t;
            ss << '('
               << names[0] << ": " << decay_to_str(a) << ", "
               << names[1] << ": " << decay_to_str(b) << ", "
               << names[2] << ": " << decay_to_str(c) << ", "
               << names[3] << ": " << decay_to_str(d) << ", "
               << names[4] << ": " << decay_to_str(e) << ", "
               << names[5] << ": " << decay_to_str(f) << ", "
               << names[6] << ": " << decay_to_str(g) << ", "
               << names[7] << ": " << decay_to_str(h) << ", "
               << names[8] << ": " << decay_to_str(i) << ", "
               << names[9] << ": " << decay_to_str(j) << ", "
               << ')';
            return ss.str();
        }
        
        else {
            ss << "<object at " << &t << " with >10 members>";
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
            return ss.str();
        } else if constexpr (RT::mem_num == 3) {
            const auto& [a, b, c] = t;
            ss << '(' << decay_to_str(a) << ", " << decay_to_str(b) << ", "
               << decay_to_str(c) << ')';
            return ss.str();
        } else if constexpr (RT::mem_num == 4) {
            const auto& [a, b, c, d] = t;
            ss << '(' << decay_to_str(a) << ", " << decay_to_str(b) << ", "
               << decay_to_str(c) << ", " << decay_to_str(d) << ')';
            return ss.str();
        } else if constexpr (RT::mem_num == 5) {
            const auto& [a, b, c, d, e] = t;
            ss << '('
               << decay_to_str(a) << ", " << decay_to_str(b) << ", "
               << decay_to_str(c) << ", " << decay_to_str(d) << ", "
               << decay_to_str(e) << ")";
            return ss.str();
        } else if constexpr (RT::mem_num == 6) {
            const auto& [a, b, c, d, e, f] = t;
            ss << '('
               << decay_to_str(a) << ", " << decay_to_str(b) << ", "
               << decay_to_str(c) << ", " << decay_to_str(d) << ", "
               << decay_to_str(e) << ", " << decay_to_str(f) << ')';
            return ss.str();
        } else if constexpr (RT::mem_num == 7) {
            const auto& [a, b, c, d, e, f, g] = t;
            ss << '('
               << decay_to_str(a) << ", " << decay_to_str(b) << ", "
               << decay_to_str(c) << ", " << decay_to_str(d) << ", "
               << decay_to_str(e) << ", " << decay_to_str(f) << ", "
               << decay_to_str(g) << ')';
            return ss.str();
        } else if constexpr (RT::mem_num == 8) {
            const auto& [a, b, c, d, e, f, g, h] = t;
            ss << '('
               << decay_to_str(a) << ", " << decay_to_str(b) << ", "
               << decay_to_str(c) << ", " << decay_to_str(d) << ", "
               << decay_to_str(e) << ", " << decay_to_str(f) << ", "
               << decay_to_str(g) << ", " << decay_to_str(h) << ')';
            return ss.str();
        } else if constexpr (RT::mem_num == 9) {
            const auto& [a, b, c, d, e, f, g, h, i] = t;
            ss << '('
               << decay_to_str(a) << ", " << decay_to_str(b) << ", "
               << decay_to_str(c) << ", " << decay_to_str(d) << ", "
               << decay_to_str(e) << ", " << decay_to_str(f) << ", "
               << decay_to_str(g) << ", " << decay_to_str(h) << ", "
               << decay_to_str(i) << ')';
            return ss.str();
        } else if constexpr (RT::mem_num == 10) {
            const auto& [a, b, c, d, e, f, g, h, i, j] = t;
            ss << '('
               << decay_to_str(a) << ", " << decay_to_str(b) << ", "
               << decay_to_str(c) << ", " << decay_to_str(d) << ", "
               << decay_to_str(e) << ", " << decay_to_str(f) << ", "
               << decay_to_str(g) << ", " << decay_to_str(h) << ", "
               << decay_to_str(i) << ", " << decay_to_str(j) << ')';
            return ss.str();
        }
        
        else {
            ss << "<object at " << &t << " with >10 members>";
            return ss.str();
        }
    }
    
    template<typename T>
    inline auto decay_to_str(T &&t)
    -> std::enable_if_t<match_with_v<T, tmf::is_container>, std::string>
    {
        if (t.begin() == t.end()) {
            return std::string{"[]"};
        }
        
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
    inline auto decay_to_str(T &&t, std::size_t head_size)
    -> std::enable_if_t<match_with_v<T, tmf::is_container>, std::string>
    {
        std::stringstream ss;
        ss << "[";
        std::size_t i = 0;
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
    [[maybe_unused]]
    inline auto decay_to_str(T t)
    -> std::enable_if_t<match_with_v<T, tmf::is_stack_like>, std::string>
    {
        std::stringstream ss;
        ss << "[";
        
        while (!t.empty()) {
            ss << t.top() << " > ";
            t.pop();
        }
        
        auto ret = ss.str();
        ret.pop_back();
        ret.pop_back();
        ret.back() = ']';
        
        return ret;
    }
    
    template<typename T>
    [[maybe_unused]]
    inline auto decay_to_str(T t)
    -> std::enable_if_t<match_with_v<T, tmf::is_queue_like>, std::string>
    {
        std::stringstream ss;
        ss << "[";
        
        while (!t.empty()) {
            ss << t.front() << " > ";
            t.pop();
        }
        
        auto ret = ss.str();
        ret.pop_back();
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
        return ss.str();
    }
}

namespace ugly {
    template<typename T>
    [[maybe_unused]]
    inline auto decay_to_str(T&& t) -> std::string {
        return impl::decay_to_str(std::forward<T>(t));
    }
    
    template<typename T>
    [[maybe_unused]]
    inline auto decay_to_str(T&& t, std::size_t head_size) -> std::string {
        return impl::decay_to_str(std::forward<T>(t), head_size);
    }
}
/**
 * Implementation of ugly::decay_to_str end.
 */

/**
 * Implementation of ugly::fmt.
 */
namespace ugly::impl {
    template<typename T>
    [[maybe_unused]]
    void fmt_helper(std::string &string_template, T &&val) {
        auto pat = std::regex(R"(\{[^\}]*\})");
        auto match = std::smatch();
        if (not std::regex_search(string_template, match, pat)) {
            return;
        }
        auto policy = match.str();
        if (policy == "{}") {
            string_template =
                    match.prefix().str() + decay_to_str(std::forward<T>(val))
                    + match.suffix().str();
        }
    }
}

namespace ugly {
    template<typename... Args>
    [[maybe_unused]]
    std::string fmt(std::string string_template, Args&&... args) {
        (impl::fmt_helper(string_template, std::forward<Args>(args)), ...);
        return string_template;
    }
}

namespace ugly::cmd {
    class SepSetter {
        std::string sep_;
    public:
        explicit SepSetter(std::string sep) : sep_(std::move(sep)) {}
    
    public:
        [[nodiscard]]
        const std::string& sep() const {
            return sep_;
        }
    };
    
    template<int>
    class Switch {
        bool b;
    
    public:
        [[maybe_unused]]
        constexpr explicit Switch(bool b) noexcept: b(b) {}
        
        [[nodiscard]]
        bool get_b() const {
            return b;
        }
    };
    
    using FlushRuleSetter = Switch<0>;
    using WrapMessageRuleSetter = Switch<1>;
}

namespace ugly {
    class Printer {
        Printer() noexcept = default;
        
        std::string sep{"\n"};
        bool flush_rule = true;
        bool wrap_message_rule = true;
    
    public:
        template<typename T, typename = std::enable_if_t<
                !std::is_same_v<std::decay_t<T>, cmd::SepSetter> &&
                !std::is_same_v<std::decay_t<T>, cmd::FlushRuleSetter> &&
                !std::is_same_v<std::decay_t<T>, cmd::WrapMessageRuleSetter>, void>>
        Printer &operator<<(T &&t) {
            auto str = decay_to_str(std::forward<T>(t));
            if (wrap_message_rule) {
                str = "debug![" + str + "]";
            }
            str += sep;
            
            if (cmp::is_eq_universal(t, std::string("\n")) or
                cmp::is_eq_universal(t, '\n')) {
                str = '\n';
            }
            
            std::cout << str;
            if (flush_rule) {
                std::cout << std::flush;
            }
            return *this;
        }
        
        Printer &operator<<(const cmd::SepSetter& sep_setter) {
            sep = sep_setter.sep();
            return *this;
        }
        
        Printer &operator<<(cmd::FlushRuleSetter f) {
            flush_rule = f.get_b();
            return *this;
        }
        
        Printer &operator<<(cmd::WrapMessageRuleSetter w) {
            wrap_message_rule = w.get_b();
            return *this;
        }
        
        static auto get() noexcept -> Printer & {
            static Printer dout;
            return dout;
        }
        
    };
    
    [[maybe_unused]]
    inline Printer& dout = Printer::get();
}

namespace ugly {
    
    [[maybe_unused]]
    auto sep_set(std::string sep) -> cmd::SepSetter {
        return cmd::SepSetter(std::move(sep));
    }
    
    [[maybe_unused]]
    inline constexpr auto flush_on = cmd::FlushRuleSetter(true);
    
    [[maybe_unused]]
    inline constexpr auto flush_off = cmd::FlushRuleSetter(false);
    
    [[maybe_unused]]
    inline constexpr auto wrap_on = cmd::WrapMessageRuleSetter(true);
    
    [[maybe_unused]]
    inline constexpr auto wrap_off = cmd::WrapMessageRuleSetter(false);
}

#endif
