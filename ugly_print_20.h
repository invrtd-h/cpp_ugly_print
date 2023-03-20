#ifndef UGLY_PRINT_UGLY_PRINT_20_H
#define UGLY_PRINT_UGLY_PRINT_20_H

#include <concepts>
#include <iostream>
#include <queue>
#include <sstream>
#include <stack>
#include <utility>
#include <regex>


/**
 * Definitions for some concepts.
 */
namespace ugly::detail {
    template<typename T>
    concept printable = requires (T t) {
        std::cout << t;
    };
    
    template<typename T>
    concept pair = requires (T t) {
        t.first;
        t.second;
    };
    
    template<typename T>
    concept container = requires (T t) {
        t.begin();
        t.end();
    };
    
    template<typename T>
    concept array = std::is_array_v<T>;
    
    template<typename T>
    concept named_tuple = requires {
        {T::mem_names} -> std::same_as<const char* []>;
    };
    
    template<typename T>
    concept unpackable = requires {
        {T::mem_num} -> std::same_as<const int>;
    };
}

/**
 * Definitions for the to_str function.
 */
namespace ugly::detail {
    
    /**
     * Universal to_str function.
     * @return std::string object that represents that object.
     */
    template<typename T>
    auto to_str(const T&) -> std::string;
    
    auto to_str_printable(const printable auto& t) -> std::string {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }
    
    auto to_str_pair(const pair auto& t) -> std::string {
        std::stringstream ss;
        ss << '<' << to_str(t.first) << ", " << to_str(t.second) << '>';
        return ss.str();
    }
    
    auto to_str_container(const container auto& t) -> std::string {
        if (t.begin() == t.end()) {
            return std::string{"[]"};
        }
    
        std::stringstream ss;
        ss << "[";
        for (auto it = t.begin(); it != t.end(); ++it) {
            ss << to_str(*it) << ", ";
        }
    
        auto ret = ss.str();
        ret.pop_back();
        ret.back() = ']';
    
        return ret;
    }
    
    template<array T>
    auto to_str_array(const T& t) -> std::string {
        constexpr std::size_t SIZE = std::extent_v<T>;
        
        if (SIZE == 0) {
            return "[]";
        }
        
        std::stringstream ss;
        ss << "[";
        
        for (int i = 0; i < SIZE; ++i) {
            ss << to_str(t[i]) << ", ";
        }
    
        auto ret = ss.str();
        ret.pop_back();
        ret.back() = ']';
    
        return ret;
    }
    
    template<named_tuple T>
    auto to_str_named_tuple(const T& t) -> std::string {
        constexpr int SIZE = std::extent_v<decltype(T::mem_names)>;
        const auto& names = T::mem_names;
    
        std::stringstream ss;
        if constexpr (SIZE == 0) {
            return "()";
        } else if constexpr (SIZE == 1) {
            const auto& [a] = t;
            ss << '(' << names[0] << ": " << to_str(a) << ')';
            return ss.str();
        } else if constexpr (SIZE == 2) {
            const auto& [a, b] = t;
            ss << '('
               << names[0] << ": " << to_str(a) << ", "
               << names[1] << ": " << to_str(b) << ')';
            return ss.str();
        } else if constexpr (SIZE == 3) {
            const auto& [a, b, c] = t;
            ss << '('
               << names[0] << ": " << to_str(a) << ", "
               << names[1] << ": " << to_str(b) << ", "
               << names[2] << ": " << to_str(c)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 4) {
            const auto& [a, b, c, d] = t;
            ss << '('
               << names[0] << ": " << to_str(a) << ", "
               << names[1] << ": " << to_str(b) << ", "
               << names[2] << ": " << to_str(c) << ", "
               << names[3] << ": " << to_str(d)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 5) {
            const auto& [a, b, c, d, e] = t;
            ss << '('
               << names[0] << ": " << to_str(a) << ", "
               << names[1] << ": " << to_str(b) << ", "
               << names[2] << ": " << to_str(c) << ", "
               << names[3] << ": " << to_str(d) << ", "
               << names[4] << ": " << to_str(e)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 6) {
            const auto& [a, b, c, d, e, f] = t;
            ss << '('
               << names[0] << ": " << to_str(a) << ", "
               << names[1] << ": " << to_str(b) << ", "
               << names[2] << ": " << to_str(c) << ", "
               << names[3] << ": " << to_str(d) << ", "
               << names[4] << ": " << to_str(e) << ", "
               << names[5] << ": " << to_str(f)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 7) {
            const auto& [a, b, c, d, e, f, g] = t;
            ss << '('
               << names[0] << ": " << to_str(a) << ", "
               << names[1] << ": " << to_str(b) << ", "
               << names[2] << ": " << to_str(c) << ", "
               << names[3] << ": " << to_str(d) << ", "
               << names[4] << ": " << to_str(e) << ", "
               << names[5] << ": " << to_str(f) << ", "
               << names[6] << ": " << to_str(g)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 8) {
            const auto& [a, b, c, d, e, f, g, h] = t;
            ss << '('
               << names[0] << ": " << to_str(a) << ", "
               << names[1] << ": " << to_str(b) << ", "
               << names[2] << ": " << to_str(c) << ", "
               << names[3] << ": " << to_str(d) << ", "
               << names[4] << ": " << to_str(e) << ", "
               << names[5] << ": " << to_str(f) << ", "
               << names[6] << ": " << to_str(g) << ", "
               << names[7] << ": " << to_str(h)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 9) {
            const auto& [a, b, c, d, e, f, g, h, i] = t;
            ss << '('
               << names[0] << ": " << to_str(a) << ", "
               << names[1] << ": " << to_str(b) << ", "
               << names[2] << ": " << to_str(c) << ", "
               << names[3] << ": " << to_str(d) << ", "
               << names[4] << ": " << to_str(e) << ", "
               << names[5] << ": " << to_str(f) << ", "
               << names[6] << ": " << to_str(g) << ", "
               << names[7] << ": " << to_str(h) << ", "
               << names[8] << ": " << to_str(i)
               << ')';
            return ss.str();
        } else if constexpr (SIZE == 10) {
            const auto& [a, b, c, d, e, f, g, h, i, j] = t;
            ss << '('
               << names[0] << ": " << to_str(a) << ", "
               << names[1] << ": " << to_str(b) << ", "
               << names[2] << ": " << to_str(c) << ", "
               << names[3] << ": " << to_str(d) << ", "
               << names[4] << ": " << to_str(e) << ", "
               << names[5] << ": " << to_str(f) << ", "
               << names[6] << ": " << to_str(g) << ", "
               << names[7] << ": " << to_str(h) << ", "
               << names[8] << ": " << to_str(i) << ", "
               << names[9] << ": " << to_str(j) << ", "
               << ')';
            return ss.str();
        }
    
        else {
            ss << "<object at " << &t << " with >10 members>";
            return ss.str();
        }
    }
    
    template<unpackable T>
    auto to_str_unpackable(const T& t) -> std::string {
        constexpr int n = T::mem_num;
    
        std::stringstream ss;
        if constexpr (n == 0) {
            return "()";
        } else if constexpr (n == 1) {
            const auto& [a] = t;
            ss << '(' << to_str(a) << ')';
            return ss.str();
        } else if constexpr (n == 2) {
            const auto& [a, b] = t;
            ss << '(' << to_str(a) << ", " << to_str(b) << ')';
            return ss.str();
        } else if constexpr (n == 3) {
            const auto& [a, b, c] = t;
            ss << '(' << to_str(a) << ", " << to_str(b) << ", "
               << to_str(c) << ')';
            return ss.str();
        } else if constexpr (n == 4) {
            const auto& [a, b, c, d] = t;
            ss << '(' << to_str(a) << ", " << to_str(b) << ", "
               << to_str(c) << ", " << to_str(d) << ')';
            return ss.str();
        } else if constexpr (n == 5) {
            const auto& [a, b, c, d, e] = t;
            ss << '('
               << to_str(a) << ", " << to_str(b) << ", "
               << to_str(c) << ", " << to_str(d) << ", "
               << to_str(e) << ")";
            return ss.str();
        } else if constexpr (n == 6) {
            const auto& [a, b, c, d, e, f] = t;
            ss << '('
               << to_str(a) << ", " << to_str(b) << ", "
               << to_str(c) << ", " << to_str(d) << ", "
               << to_str(e) << ", " << to_str(f) << ')';
            return ss.str();
        } else if constexpr (n == 7) {
            const auto& [a, b, c, d, e, f, g] = t;
            ss << '('
               << to_str(a) << ", " << to_str(b) << ", "
               << to_str(c) << ", " << to_str(d) << ", "
               << to_str(e) << ", " << to_str(f) << ", "
               << to_str(g) << ')';
            return ss.str();
        } else if constexpr (n == 8) {
            const auto& [a, b, c, d, e, f, g, h] = t;
            ss << '('
               << to_str(a) << ", " << to_str(b) << ", "
               << to_str(c) << ", " << to_str(d) << ", "
               << to_str(e) << ", " << to_str(f) << ", "
               << to_str(g) << ", " << to_str(h) << ')';
            return ss.str();
        } else if constexpr (n == 9) {
            const auto& [a, b, c, d, e, f, g, h, i] = t;
            ss << '('
               << to_str(a) << ", " << to_str(b) << ", "
               << to_str(c) << ", " << to_str(d) << ", "
               << to_str(e) << ", " << to_str(f) << ", "
               << to_str(g) << ", " << to_str(h) << ", "
               << to_str(i) << ')';
            return ss.str();
        } else if constexpr (n == 10) {
            const auto& [a, b, c, d, e, f, g, h, i, j] = t;
            ss << '('
               << to_str(a) << ", " << to_str(b) << ", "
               << to_str(c) << ", " << to_str(d) << ", "
               << to_str(e) << ", " << to_str(f) << ", "
               << to_str(g) << ", " << to_str(h) << ", "
               << to_str(i) << ", " << to_str(j) << ')';
            return ss.str();
        }
    
        else {
            ss << "<object at " << &t << " with >10 members>";
            return ss.str();
        }
    }
    
    auto to_str_rule_undefined(const auto& t) -> std::string {
        std::stringstream ss;
        ss << "<object at " << &t << ">";
        return ss.str();
    }
    
    template<typename T>
    auto to_str(const T& t) -> std::string {
        if constexpr (printable<T>) {
            return to_str_printable(t);
        } else if constexpr (pair<T>) {
            return to_str_pair(t);
        } else if constexpr (container<T>) {
            return to_str_container(t);
        } else if constexpr (array<T>) {
            return to_str_array(t);
        } else if constexpr (named_tuple<T>) {
            return to_str_named_tuple(t);
        } else if constexpr (unpackable<T>) {
            return to_str_unpackable(t);
        }
        
        else {
            return to_str_rule_undefined(t);
        }
    }
}

namespace ugly::detail {
    template<typename T>
    void fmt_helper(std::string &string_template, const T& val) {
        auto pat = std::regex(R"(\{[^\}]*\})");
        auto match = std::smatch();
        if (not std::regex_search(string_template, match, pat)) {
            return;
        }
        auto policy = match.str();
        if (policy == "{}") {
            string_template =
                    match.prefix().str() + to_str(val)
                    + match.suffix().str();
        }
    }
    
    template<typename... Args>
    [[maybe_unused]]
    std::string fmt(std::string string_template, const Args&... args) {
        (fmt_helper(string_template, args), ...);
        return string_template;
    }
}

namespace ugly::detail {
    class Printer {
    
    public:
        Printer& operator<<(const auto& t) {
            std::cout << fmt("debug!/{}/\n", t);
            return *this;
        }
    };
    
    Printer dout;
}

namespace ugly {
    using detail::to_str;
    using detail::dout;
}

#endif
