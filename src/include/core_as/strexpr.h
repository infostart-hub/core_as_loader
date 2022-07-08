/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* База для строковых конкатенаций через выражения времени компиляции
*/
#pragma once
namespace coreas_str {
using u8symbol = char;
using u32symbol = char32_t;

template<size_t N>
struct wchar_type {
    using type = char16_t;
};
template<>
struct wchar_type<2> {
    using type = wchar_t;
};

using u16symbol = typename wchar_type<sizeof(wchar_t)>::type;

using uint = unsigned int;
using uu8symbol = std::make_unsigned<u8symbol>::type;

/*
* Шаблонные классы для создания строковых выражений из нескольких источников
* Благодаря компиляторно-шаблонной "магии" позволяют максимально эффективно
* получать результирующую строку - сначала вычисляется длина результирующей строки,
* потом один раз выделяется память для результата, после символы помещаются в
* выделенную память.
*/

/* Источником строковых выражений может быть любой объект, поддерживающий эти операции :
* тип symb_type, length(), place()
*/
template<typename A>
constexpr const bool is_strexpr_v = requires(const A & a) {
    { a.length() } -> std::convertible_to<uint>;
    { a.place((typename A::symb_type*)nullptr) } -> std::convertible_to<typename A::symb_type*>;
};

template<typename A, typename K>
constexpr const bool is_strexpr = requires(const A & a) {
    { a.length() } -> std::convertible_to<uint>;
    { a.place((typename A::symb_type*)nullptr) } -> std::convertible_to<K*>;
};

// Для конкатенация двух объектов строковых выражений в один
template<typename A, typename B, typename = std::enable_if_t<is_strexpr_v<A> && is_strexpr_v<B> && std::is_same_v<typename A::symb_type, typename B::symb_type>>>
struct strexprjoin {
    using symb_type = typename A::symb_type;
    const A& a;
    const B& b;
    strexprjoin(const A& a_, const B& b_) : a(a_), b(b_){}
    constexpr uint length() const noexcept { return a.length() + b.length(); }
    constexpr symb_type* place(symb_type* p) const noexcept { return b.place(a.place(p)); }
    constexpr symb_type* len_and_place(symb_type* p) const noexcept { a.length(); b.length(); return place(p); }
};

template<typename A, typename B, typename = std::enable_if_t<is_strexpr_v<A>&& is_strexpr_v<B>&& std::is_same_v<typename A::symb_type, typename B::symb_type>>>
static auto operator & (const A& a, const B& b) {
    return strexprjoin<A, B>{a, b};
}

// Для возможности конкатенации ссылок на строковое выражение и создаваемого временного объекта, путём его копии
template<typename A, typename B, typename = std::enable_if_t<is_strexpr_v<A> && is_strexpr_v<B> && std::is_same_v<typename A::symb_type, typename B::symb_type>>>
struct strexprjoin_c {
    using symb_type = typename A::symb_type;
    const A& a;
    B b;
    strexprjoin_c(const A& a_, B&& b_) : a(a_), b(std::move(b_)) {}
    constexpr uint length() const noexcept { return a.length() + b.length(); }
    constexpr symb_type* place(symb_type* p) const noexcept { return b.place(a.place(p)); }
    constexpr symb_type* len_and_place(symb_type* p) const noexcept { a.length(); b.length(); return place(p); }
};

template<typename T, typename K = void, typename ...Types>
struct is_one_of_type {
    static constexpr bool value = std::is_same_v<T, K> ? true : is_one_of_type<T, Types...>::value;
};
template<typename T> struct is_one_of_type<T, void> : std::false_type {};

template<typename K>
struct empty_expr {
    using symb_type = K;
    constexpr uint length() const noexcept { return 0; }
    constexpr symb_type* place(symb_type* p) const noexcept { return p; }
};

#define eea empty_expr<u8symbol>{}
#define eew empty_expr<u16symbol>{}
#define eeu empty_expr<u32symbol>{}

template<typename K>
struct expr_char {
    using symb_type = K;
    K value;
    constexpr uint length() const noexcept { return 1; }
    constexpr symb_type* place(symb_type* p) const noexcept {
        *p++ = value;
        return p;
    }
};

template<typename A, typename K, typename std::enable_if_t<is_strexpr<A, K>, int> = 0>
constexpr static auto operator & (const A& a, K s) {
    return strexprjoin_c<A, expr_char<K>>{ a, {s} };
}

template<typename K, uint N>
struct expr_literal {
    using symb_type = K;
    const K(&str)[N + 1];
    constexpr uint length() const noexcept { return N; }
    constexpr symb_type* place(symb_type* p) const noexcept {
        if constexpr (N != 0)
            std::char_traits<K>::copy(p, str, N);
        return p + N;
    }
};

template<typename K, size_t N>
constexpr static auto e_t(const K(&s)[N]) {
    return expr_literal<K, static_cast<uint>(N - 1)>{ s };
}

template<bool first, typename K, uint N, typename A>
struct expr_literal_join {
    using symb_type = K;
    const K(&str)[N + 1];
    const A& a;
    constexpr uint length() const noexcept { return N + a.length(); }
    constexpr symb_type* place(symb_type* p) const noexcept {
        if constexpr (N != 0) {
            if constexpr (first) {
                std::char_traits<K>::copy(p, str, N);
                return a.place(p + N);
            } else {
                p = a.place(p);
                std::char_traits<K>::copy(p, str, N);
                return p + N;
            }
        } else {
            return a.place(p);
        }
    }
    constexpr symb_type* len_and_place(symb_type* p) const noexcept {
        a.length(); return place(p);
    }
};

template<typename A, typename K, size_t N, typename = std::enable_if_t<is_strexpr<A, K>>>
constexpr static auto operator & (const A& a, const K(&s)[N]) {
    return expr_literal_join<false, K, static_cast<uint>(N - 1), A>{ s, a };
}

template<typename A, typename K, size_t N, typename = std::enable_if_t<is_strexpr<A, K>>>
constexpr static auto operator & (const K(&s)[N], const A& a) {
    return expr_literal_join<true, K, static_cast<uint>(N - 1), A>{ s, a };
}

template<typename K, uint N, uint S = ' '>
struct expr_spaces {
    using symb_type = K;
    constexpr uint length() const noexcept { return N; }
    constexpr symb_type* place(symb_type* p) const noexcept {
        if constexpr (N != 0)
            std::char_traits<K>::assign(p, N, static_cast<K>(S));
        return p + N;
    }
};

template<uint N>
constexpr static auto e_spca() {
    return expr_spaces<u8symbol, N>();
}

template<uint N>
constexpr static auto e_spcw() {
    return expr_spaces<u16symbol, N>();
}

template<typename K>
struct expr_pad {
    using symb_type = K;
    K s;
    uint len;
    constexpr uint length() const noexcept { return len; }
    constexpr symb_type* place(symb_type* p) const noexcept {
        if (len)
            std::char_traits<K>::assign(p, len, s);
        return p + len;
    }
};

template<typename K>
constexpr static auto e_c(K s, uint l) {
    return expr_pad<K>{ s, l };
}

}// namespace coreas_str {
