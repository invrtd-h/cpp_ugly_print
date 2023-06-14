#ifndef UGLY_PRINT_UGLY_PRINT_21_H
#define UGLY_PRINT_UGLY_PRINT_21_H

#include <any>
#include <iomanip>
#include <iostream>
#include <unordered_set>

namespace ugly::v21 {
    template<typename T>
    concept printable = requires (T t) {
        std::cout << t;
    };
    
    template<typename T>
    concept to_string_defined = requires (T t) {
        {t.to_string()} -> std::same_as<std::string>;
    };
    
    template<typename T>
    concept range = std::ranges::range<T>;
    
    template<typename T>
    concept pointer_like = requires (T t) {
        *t;
    };
    
    template<typename T>
    concept named_tuple = requires {
        T::mem_names;
    };
    
    template<typename T>
    concept unpackable = requires {
        T::mem_num;
    };
}

namespace ugly::v21 {
    template<typename>
    class [[maybe_unused]] TD;
    
    [[maybe_unused]]
    inline auto str_len(const std::string& str) noexcept -> std::size_t {
        return str.size();
    }
    
    [[maybe_unused]]
    inline constexpr auto str_len(const char* str) noexcept {
        std::size_t ret = 0;
        while (str[ret]) {
            ret++;
        }
        return ret;
    }
    
    static_assert(str_len("what") == 4);
    static_assert(str_len("sequence") == 8);
    
    template<std::convertible_to<std::string>... T>
    [[maybe_unused]]
    inline auto str_cat(const T&... strs) {
        std::size_t size = (str_len(std::string(strs)) + ...);
        std::string ret;
        ret.reserve(size);
        
        (ret.append(strs), ...);
        
        return ret;
    }
    
    [[maybe_unused]]
    inline constexpr auto str_rep(const std::string& s, std::size_t time) -> std::string {
        std::string ret;
        ret.reserve(s.size() * time);
        
        for (std::size_t i = 0; i < time; ++i) {
            ret += s;
        }
        
        return ret;
    }
}

namespace ugly::v21 {
    template<typename Derived>
    class Strategy {
        constexpr auto to_str(const auto& t) -> std::string {
            return static_cast<Derived*>(this)->to_str_impl(t);
        }
    };
    
    class BasicStrategy : Strategy<BasicStrategy> {
    public:
        template<typename T1, typename T2>
        constexpr auto to_str_impl(const std::pair<T1, T2>& t) -> std::string {
            return str_cat("<", to_str(t.first), ", ", to_str(t.second), ">");
        }
    };
    
    class PrintableStrategy {
        std::stringstream ss;
        
    public:
        PrintableStrategy() : ss() {
            ss << std::boolalpha;
            ss << std::setprecision(8);
        }
    };
    
    class RangeStrategy {
    public:
    
    };
    
    class PointerLikeStrategy {
    public:
    
    };
    
    class NamedTupleStrategy {
    public:
    
    };
    
    class UnpackableStrategy {
    public:
    
    };
    
    class Stringifier {
    
    };
}

namespace ugly::v21 {
    class Printer {
        std::unordered_set<std::size_t> w;
        std::string null_proxy;
    public:
        Printer() = default;
        
    public:
        auto std_repr(const auto& t) const -> std::string {

        }
        
    };
    
    inline auto dout = Printer();
}

#endif
