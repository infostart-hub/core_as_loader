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
*
* Для построения строкового выражения необходимо начать его с одного из объектов типа strexpr<A>
* Оператор "унарный плюс" объектов lstring, sstring, SimpleStr как-раз
* возвращают такие объекты. Либо можно создать пустой объект строкового выражения:
* ee<char|wide_char|char32_t>()
* Далее через оператор & присоединяются другие части выражения. Для этого они должны быть
* объектами, имеющими интерфейс, такой же как у strexpr, а также иметь глобально переопределенный
* operator & (const strexpr<A>& a, ТипОбъекта s)
* Из-коробки такими объектами могут быть SimpleStr, sstring, lstring.
* Также имеются встроенные перегрузки для числовых и символьных типов, строковых констант,
* есть возможность использовать слияние контейнеров строк с разделителем,
* выбор из строковых подвыражений, замена.
*/
template<typename A>
struct strexpr {
    using symb_type = typename A::symb_type;
    A a;
    constexpr uint length() const noexcept { return a.length(); }
    constexpr symb_type* place(symb_type* p) const noexcept { return a.place(p); }
};

template<typename A, typename B>
struct strexprjoin {
    using symb_type = typename A::symb_type;
    A a;
    B b;
    constexpr uint length() const noexcept { return a.length() + b.length(); }
    constexpr symb_type* place(symb_type* p) const noexcept { return b.place(a.place(p)); }
};

template<typename T, typename K = void, typename ...Types>
struct is_one_of_type {
    static constexpr bool value = std::is_same_v<T, K> ? true : is_one_of_type<T, Types...>::value;
};

template<typename T> struct is_one_of_type<T, void> : std::false_type {};

template<typename K>
struct expr_empty {
    using symb_type = K;
    constexpr uint length() const noexcept { return 0; }
    constexpr symb_type* place(symb_type* p) const noexcept { return p; }
};

template<typename K>
constexpr static auto ee() {
    return strexpr<expr_empty<K>>{expr_empty<K>{}};
}
#define eea ee<u8symbol>()
#define eew ee<u16symbol>()

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

template<typename A, typename K, typename std::enable_if_t<std::is_same_v<K, typename A::symb_type>, int> = 0>
constexpr static auto operator & (const strexpr<A>& a, K s) {
    using J = strexprjoin<A, expr_char<K>>;
    return strexpr<J>{J{ a.a, expr_char<K>{s} }};
}

template<typename K, uint N>
struct expr_literal {
    using symb_type = K;
    const K* str;
    constexpr uint length() const noexcept { return N; }
    constexpr symb_type* place(symb_type* p) const noexcept {
        if constexpr (N != 0)
            std::char_traits<K>::copy(p, str, N);
        return p + N;
    }
};

template<typename A, typename K, size_t N, std::enable_if_t<std::is_same_v<K, typename A::symb_type>, int> = 0>
constexpr static auto operator & (const strexpr<A>& a, const K(&s)[N]) {
    using L = expr_literal<K, static_cast<uint>(N - 1)>;
    using J = strexprjoin<A, L>;
    return strexpr<J>{J{ a.a, L { s } }};
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

template<typename A, typename K, uint N, uint S>
constexpr static auto operator & (const strexpr<A>& a, const expr_spaces<K, N, S>& s) {
    using J = strexprjoin<A, expr_spaces<K, N, S>>;
    return strexpr<J>{J{ a.a, s }};
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

template<typename A, typename K>
constexpr static auto operator & (const strexpr<A>& a, const expr_pad<K>& s) {
    using J = strexprjoin<A, expr_pad<K>>;
    return strexpr<J>{J{ a.a, s }};
}

template<typename K>
constexpr static auto e_c(K s, uint l) {
    return expr_pad<K>{ s, l };
}

}// namespace coreas_str {
