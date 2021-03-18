/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Классы для работы со строками
*/
#pragma once
#include "core_as_base.h"
#include "strexpr.h"

template<typename T>
struct unicode_traits {};
template<>
struct unicode_traits<u8symbol> {
    // Эти операции с utf-8 могут изменить длину строки
    // Поэтому их специализации отличаются
    // В функцию помимо текста и адреса буфера для записи передается размер буфера
    // Возвращает длину получающейся строки.
    // Если получающеюся строка не влезает в отведенный буфер, указатели устанавливаются на последние
    // обработанные символы, для повторного возобновления работы,
    // а для оставшихся символов считается нужный размер буфера.
    static COREAS_API uint upper(const u8symbol*& src, uint lenStr, u8symbol*& dest, uint lenBuf);
    static COREAS_API uint lower(const u8symbol*& src, uint len, u8symbol*& dest, uint lenBuf);
    static COREAS_API uint findFirstLower(const u8symbol* src, uint lenStr);
    static COREAS_API uint findFirstUpper(const u8symbol* src, uint lenStr);

    static COREAS_API int compareiu(const u8symbol* text1, uint len1, const u8symbol* text2, uint len2);

    static COREAS_API size_t hashia(const u8symbol* src, uint l);
    static COREAS_API size_t hashiu(const u8symbol* src, uint l);
};

template<>
struct unicode_traits<u16symbol> {
    static COREAS_API void upper(const u16symbol* src, uint len, u16symbol* dest);
    static COREAS_API void lower(const u16symbol* src, uint len, u16symbol* dest);
    static COREAS_API uint findFirstLower(const u16symbol* src, uint lenStr);
    static COREAS_API uint findFirstUpper(const u16symbol* src, uint lenStr);

    static COREAS_API int compareiu(const u16symbol* text1, uint len1, const u16symbol* text2, uint len2);
    static COREAS_API size_t hashia(const u16symbol* src, uint l);
    static COREAS_API size_t hashiu(const u16symbol* src, uint l);
};

template<>
struct unicode_traits<u32symbol> {
    static COREAS_API void upper(const u32symbol* src, uint len, u32symbol* dest);
    static COREAS_API void lower(const u32symbol* src, uint len, u32symbol* dest);
    static COREAS_API uint findFirstLower(const u32symbol* src, uint lenStr);
    static COREAS_API uint findFirstUpper(const u32symbol* src, uint lenStr);

    static COREAS_API int compareiu(const u32symbol* text1, uint len1, const u32symbol* text2, uint len2);
    static COREAS_API size_t hashia(const u32symbol* src, uint s);
    static COREAS_API size_t hashiu(const u32symbol* src, uint s);
};

namespace str_pos {
constexpr const uint badIdx = static_cast<uint>(-1);
}

template<uint N>
using is_const_pattern_t = enable_if_t<(N > 1) && N <= 17, int>;

template<typename K, uint I>
struct _ascii_mask {
    constexpr static const size_t value = size_t(K(~0x7F)) << ((I - 1) * sizeof(K) * 8) | _ascii_mask<K, I-1>::value;
};

template<typename K>
struct _ascii_mask<K, 0> {
    constexpr static const size_t value = 0;
};

template<typename K>
struct ascii_mask {
    using uns = typename make_unsigned<K>::type;
    constexpr static const uint width = sizeof(size_t) / sizeof(uns);
    constexpr static const size_t value = _ascii_mask<uns, width>::value;
};

template<bool, typename O, typename K>
constexpr bool is_strtype_obj_v = is_same_v<typename O::symb_type, K>;

template<typename O, typename K>
constexpr bool is_strtype_obj_v<false, O, K> = false;

template<typename O, typename K>
constexpr bool is_strtype_v = is_strtype_obj_v<is_class_v<O>, O, K>;

template<typename O, typename K>
using is_strtype_t = enable_if_t<is_strtype_v<O, K>, int>;


template<typename K>
constexpr static bool isAsciiUpper(K k) {
    return k >= 'A' && k <= 'Z';
}

template<typename K>
constexpr static bool isAsciiLower(K k) {
    return k >= 'a' && k <= 'z';
}

template<typename K>
constexpr static K makeAsciiLower(K k) {
    return isAsciiUpper(k) ? k | 0x20 : k;
}

// Только после проверки, что это upper
template<typename K>
constexpr static K makeAsciiUpper(K k) {
    return isAsciiLower(k) ? k & ~0x20 : k;
}

enum TrimSides { trimLeft = 1, trimRight = 2, trimAll = 3 };
template<TrimSides S, typename K, uint N, bool withSpaces = false>
struct trimOperator;

template<typename K, uint N, uint L> struct expr_replaces;

/*
* Класс с базовыми строковыми алгоритмами
* Является базой для классов, могущих выполнять константные операции со строками.
* Ничего не знает о хранении строк, ни сам, ни у класса наследника, то есть работает
* только с указателем на строку и её длиной.
* Для работы класс-наследник должен реализовать методы:
*   uint length() const noexcept     - возвращает длину строки
*   const K* c_str() const noexcept  - возвращает указатель на начало строки
*   bool isEmpty() const noexcept    - проверка, не пустая ли строка
* K       - тип символов
* StrRef  - тип хранилища куска строки
* Impl    - конечный класс наследник
* Exp     - для плейсхолдера для строковых выражений
*/

template<typename K, typename StrRef, typename Impl, typename Exp = Impl>
class str_algs {
    const Impl& d() const noexcept {
        return *static_cast<const Impl*>(this);
    }
    uint _len() const noexcept {
        return d().length();
    }
    const K* _str() const noexcept {
        return d().c_str();
    }
    bool _isEmpty() const noexcept {
        return d().isEmpty();
    }

public:
    using symb_type = K;
    using SimpleStr = StrRef;
    using traits = char_traits<K>;
    using uni = unicode_traits<K>;
    using uns_type = make_unsigned_t<K>;
    using my_type = Impl;
    // Пустой конструктор
    str_algs() = default;

    // Чтобы быть источником в строковом выражении
    constexpr strexpr<Exp> operator + () const noexcept {
        return strexpr<Exp>{ d() };
    }
    constexpr K* place(K* ptr) const noexcept {
        uint myLen = _len();
        if (myLen) {
            traits::copy(ptr, _str(), myLen);
            return ptr + myLen;
        }
        return ptr;
    }

    // Чтобы быть источником строкового объекта
    constexpr operator SimpleStr() const noexcept {
        return SimpleStr{ _str(), _len() };
    }
    SimpleStr to_str() const noexcept {
        return SimpleStr{ _str(), _len() };
    }

    constexpr SimpleStr operator () (int from, int len = 0) const noexcept {
        uint myLen = _len(), idxStart = from >= 0 ? from : myLen + from,
            idxEnd = (len > 0 ? from : myLen) + len;
        if (idxEnd > myLen)
            idxEnd = myLen;
        if (idxStart > idxEnd)
            idxStart = idxEnd;
        return SimpleStr { _str() + idxStart, idxEnd - idxStart };
    }
    constexpr SimpleStr mid(uint from, int len = -1) const noexcept {
        uint myLen = _len(), idxStart = from,
            idxEnd = len >= 0 ? from + len : myLen;
        if (idxEnd > myLen)
            idxEnd = myLen;
        if (idxStart > idxEnd)
            idxStart = idxEnd;
        return SimpleStr{ _str() + idxStart, idxEnd - idxStart };
    }
    void store(char*& ptr) const noexcept {
        uint len = (_len() + 1) * sizeof(K);
        memcpy(ptr, _str(), len);
        ptr += len;
    }

    bool operator!() const noexcept {
        return _isEmpty();
    }
    // Доступ к символу, отрицательные - с конца строки
    K at(int idx) const {
        return _str()[idx >= 0 ? idx : _len() + idx];
    }
    // Сравнение строк
    int compare(SimpleStr o) const {
        if (!o.length())
            return _isEmpty() ? 0 : 1;
        uint myLen = _len(), checkLen = min(myLen, o.length());
        int cmp = checkLen ? traits::compare(_str(), o.c_str(), checkLen) : 0;
        return cmp == 0 ? (myLen > o.length() ? 1 : myLen == o.length() ? 0 : -1) : cmp;
    }

    uint find(SimpleStr pattern, uint offset = 0) const noexcept {
        uint lenText = _len(), lenPattern = pattern.length();
        // Образец, не вмещающийся в строку и пустой образец не находим
        if (!lenPattern || offset + lenPattern > lenText)
            return str_pos::badIdx;
        lenPattern--;
        const K* text = _str(), * last = text + lenText - lenPattern;
        for (const K* fnd = text + offset; ; ++fnd) {
            fnd = traits::find(fnd, last - fnd, pattern.c_str()[0]);
            if (!fnd)
                return str_pos::badIdx;
            if (!lenPattern || traits::compare(fnd + 1, pattern.c_str() + 1, lenPattern) == 0)
                return static_cast<uint>(static_cast<size_t>(fnd - text));
        }
    }

    uint find(K s, uint offset = 0) const noexcept {
        uint len = _len();
        if (offset < len) {
            const K* str = _str(), * fnd = traits::find(str + offset, len - offset, s);
            if (fnd)
                return static_cast<uint>(static_cast<size_t>(fnd - str));
        }
        return str_pos::badIdx;
    }

    uint find_reverse(K s) const noexcept {
        uint len = _len();
        while (len > 0) {
            if (_str()[--len] == s)
                return len;
        }
        return str_pos::badIdx;
    }

    bool isEqual(SimpleStr other) const noexcept {
        uint myLen = _len();
        return other.length() == myLen && (myLen == 0 || traits::compare(_str(), other.c_str(), myLen) == 0);
    }

    bool operator == (SimpleStr other) const noexcept {
        return isEqual(other);
    }

    bool operator != (SimpleStr other) const noexcept {
        return !isEqual(other);
    }

    bool operator < (SimpleStr other) const {
        return compare(other) < 0;
    }

    // Сравнение ascii строк без учёта регистра
    int compareia(SimpleStr text) const noexcept {
        if (!text.length())
            return _isEmpty() ? 0 : 1;
        uint myLen = _len(), checkLen = min(myLen, text.length());
        const uns_type* ptr1 = reinterpret_cast<const uns_type*>(_str()), * ptr2 = reinterpret_cast<const uns_type*>(text.c_str());
        while (checkLen--) {
            uns_type s1 = *ptr1++, s2 = *ptr2++;
            if (s1 == s2)
                continue;
            s1 = makeAsciiLower(s1);
            s2 = makeAsciiLower(s2);
            if (s1 > s2)
                return 1;
            else if (s1 < s2)
                return -1;
        }
        return myLen == text.length() ? 0 : myLen > text.length() ? 1 : -1;
    }

    bool isEqualia(SimpleStr text) const noexcept {
        return text.length() == _len() && (text.length() == 0 || compareia(text) == 0);
    }

    bool isLessia(SimpleStr text) const noexcept {
        return compareia(text) < 0;
    }

    int compareiu(SimpleStr text) const noexcept {
        if (!text.length())
            return _isEmpty() ? 0 : 1;
        return uni::compareiu(_str(), _len(), text.c_str(), text.length());
    }

    bool isEqualiu(SimpleStr text) const noexcept {
        return text.length() == _len() && (text.length() == 0 || compareiu(text) == 0);
    }

    bool isLessiu(SimpleStr text) const noexcept {
        return compareiu(text) < 0;
    }

    my_type substr(int from, int len = 0) const { // индексация в code units
        return my_type(d().operator()(from, len));
    }
    my_type mid_(uint from, int len = -1) const { // индексация в code units
        return my_type(d().mid(from, len));
    }

    vector<uint> find_all(SimpleStr pattern, uint offset = 0, uint maxCount = 0) const {
        if (!maxCount)
            maxCount--;
        vector<uint> result;
        while (maxCount-- > 0) {
            uint fnd = find(pattern, offset);
            if (fnd == str_pos::badIdx)
                break;
            result.push_back(fnd);
            offset = fnd + pattern.length();
        }
        return result;
    }

    template<typename T, typename Op>
    T splitf(SimpleStr delimeter, const Op& beforeFunc, uint offset = 0) const {
        uint mylen = _len(), lenDelimeter = delimeter.length();
        T results;
        SimpleStr me{ _str(), _len() };
        for (;;) {
            uint beginOfDelim = find(delimeter, offset);
            if (beginOfDelim == str_pos::badIdx) {
                SimpleStr last = beforeFunc(SimpleStr{ me.c_str() + offset, me.length() - offset });
                if (last.isSame(me)) {
                    // Пробуем положить весь объект
                    results.emplace_back(d());
                } else
                    results.emplace_back(last);
                break;
            }
            results.emplace_back(beforeFunc(SimpleStr{ me.c_str() + offset, beginOfDelim - offset }));
            offset = beginOfDelim + lenDelimeter;
        }
        return results;
    }

    // Разбиение строки на части
    template<typename T>
    T split(SimpleStr delimeter, uint offset = 0) const {
        return splitf<T>(delimeter, [](SimpleStr o) {return o; }, offset);
    }

    // Начинается ли эта строка с указанной подстроки
    bool isPrefixed(SimpleStr prefix) const noexcept {
        if (!prefix.length() || _len() < prefix.length())
            return false;
        return 0 == prefix.compare(SimpleStr{ _str(), prefix.length() });
    }
    // Начинается ли эта строка с указанной подстроки без учета ascii регистра
    bool isPrefixedia(SimpleStr prefix) const noexcept {
        if (!prefix.length() || _len() < prefix.length())
            return false;
        return 0 == prefix.compareia(SimpleStr{ _str(), prefix.length() });
    }
    // Начинается ли эта строка с указанной подстроки без учета unicode регистра
    bool isPrefixediu(SimpleStr prefix) const noexcept {
        if (!prefix.length() || _len() < prefix.length())
            return false;
        return 0 == uni::compareiu(_str(), prefix.length(), prefix.c_str(), prefix.length());
    }

    // Является ли эта строка началом указанной строки
    bool isPrefixIn(SimpleStr text) const noexcept {
        uint myLen = _len();
        if (myLen > text.length())
            return false;
        return !myLen || 0 == traits::compare(text.c_str(), _str(), myLen);
    }

    bool isAscii() const noexcept {
        if (_isEmpty())
            return true;
        const int sl = ascii_mask<K>::width;
        const size_t mask = ascii_mask<K>::value;
        uint len = _len();
        const uns_type* ptr = const_cast<const uns_type*>(_str());
        if constexpr (sl > 1) {
            while (len >= sl) {
                if (*reinterpret_cast<const size_t*>(ptr) & mask)
                    return false;
                ptr += sl;
                len -= sl;
            }
        }
        while (len--) {
            if (*ptr++ > 127)
                return false;
        }
        return true;
    }
    // ascii версия upper
    template<typename R = my_type>
    R uppera() const {
        return R::uppera_s(d());
    }
    // ascii версия lower
    template<typename R = my_type>
    R lowera() const {
        return R::lower_s(d());
    }
    template<typename R = my_type>
    R upper() const {
        return R::upper_s(d());
    }
    template<typename R = my_type>
    R lower() const {
        return R::lower_s(d());
    }

    template<typename R = my_type>
    R replace(SimpleStr pattern, SimpleStr repl, uint offset = 0, uint maxCount = 0) const {
        return R::replace_s(d(), pattern, repl, offset, maxCount);
    }

    template<uint N, uint L>
    expr_replaces<K, N - 1, L - 1> replace_init(const K(&pattern)[N], const K(&repl)[L]) {
        return expr_replaces<K, N - 1, L - 1>{d(), pattern, repl};
    }

    template<typename R = my_type>
    R& replaceTo(R& obj, SimpleStr pattern, SimpleStr repl, uint offset = 0, uint maxCount = 0) const {
        return R::replaceTo_s(obj, d(), pattern, repl, offset, maxCount);
    }

    template<typename From, is_strtype_t<From, K> = 0, typename Op>
    static my_type make_trim_op(const From& from, const Op& opTrim) {
        SimpleStr sfrom = from, newPos = opTrim(sfrom);
        return newPos.isSame(sfrom) ? my_type{ from } : my_type{ newPos };
    }
    template<TrimSides S, typename From, is_strtype_t<From, K> = 0>
    static my_type trim_static(const From& from) {
        return make_trim_op(from, trimOperator<S, K, static_cast<uint>(-1), true>{});
    }

    template<TrimSides S, bool withSpaces, uint N, typename From, is_strtype_t<From, K> = 0, is_const_pattern_t<N> = 0>
    static my_type trim_static(const From& from, const K(&pattern)[N]) {
        return make_trim_op(from, trimOperator<S, K, N - 1, withSpaces> {pattern});
    }

    template<TrimSides S, bool withSpaces, typename From, is_strtype_t<From, K> = 0>
    static my_type trim_static(const From& from, SimpleStr pattern) {
        return make_trim_op(from, trimOperator<S, K, 0, withSpaces> { pattern});
    }
    // Триминг по пробельным символам - ' ', \t\n\v\f\r
    template<typename R = my_type>
    R trim() const {
        return R::trim_static<TrimSides::trimAll>(d());
    }
    template<typename R = my_type>
    R trim_l() const {
        return R::trim_static<TrimSides::trimLeft>(d());
    }
    template<typename R = my_type>
    R trim_r() const {
        return R::trim_static<TrimSides::trimRight>(d());
    }
    // Триминг по символам в литерале
    template<typename R = my_type, uint N, is_const_pattern_t<N> = 0>
    R trim(const K(&pattern)[N]) const {
        return R::trim_static<TrimSides::trimAll, false>(d(), pattern);
    }
    template<typename R = my_type, uint N, is_const_pattern_t<N> = 0>
    R trim_l(const K(&pattern)[N]) const {
        return R::trim_static<TrimSides::trimLeft, false>(d(), pattern);
    }
    template<typename R = my_type, uint N, is_const_pattern_t<N> = 0>
    R trim_r(const K(&pattern)[N]) const {
        return R::trim_static<TrimSides::trimRight, false>(d(), pattern);
    }
    // Триминг по символам в литерале и пробелам
    template<typename R = my_type, uint N, is_const_pattern_t<N> = 0>
    R trim_s(const K(&pattern)[N]) const {
        return R::trim_static<TrimSides::trimAll, true>(d(), pattern);
    }
    template<typename R = my_type, uint N, is_const_pattern_t<N> = 0>
    R trim_sl(const K(&pattern)[N]) const {
        return R::trim_static<TrimSides::trimLeft, true>(d(), pattern);
    }
    template<typename R = my_type, uint N, is_const_pattern_t<N> = 0>
    R trim_sr(const K(&pattern)[N]) const {
        return R::trim_static<TrimSides::trimRight, true>(d(), pattern);
    }
    // Триминг по динамическому источнику
    template<typename R = my_type>
    R trim(SimpleStr pattern) const {
        return R::trim_static<TrimSides::trimAll, false>(d(), pattern);
    }
    template<typename R = my_type>
    R trim_l(SimpleStr pattern) const {
        return R::trim_static<TrimSides::trimLeft, false>(d(), pattern);
    }
    template<typename R = my_type>
    R trim_r(SimpleStr pattern) const {
        return R::trim_static<TrimSides::trimRight, false>(d(), pattern);
    }
    // Триминг по символам в литерале и пробелам
    template<typename R = my_type>
    R trim_s(SimpleStr pattern) const {
        return R::trim_static<TrimSides::trimAll, true>(d(), pattern);
    }
    template<typename R = my_type>
    R trim_sl(SimpleStr pattern) const {
        return R::trim_static<TrimSides::trimLeft, true>(d(), pattern);
    }
    template<typename R = my_type>
    R trim_sr(SimpleStr pattern) const {
        return R::trim_static<TrimSides::trimRight, true>(d(), pattern);
    }
};


template<typename K>
struct SimpleStrNt;

#define empty_bases __declspec(empty_bases) 

/*
* Базовая структура с информацией о строке.
* Это структура для невладеющих строк.
* Так как здесь только один базовый класс, ms компилятор автоматом применяет empty base optimization,
* в результате размер класса не увеличивается
*/
template<typename K>
struct SimpleStr : str_algs<K, SimpleStr<K>, SimpleStr<K> > {
    using symb_type = K;
    using my_type = SimpleStr<K>;

    const symb_type* str;
    uint len;
    
    SimpleStr() = default;
    
    template<uint N>
    constexpr SimpleStr(const K(&v)[N]) : str(v), len(N - 1) {}
    
    constexpr SimpleStr(const K* p, uint l) : str(p), len(l) {}

    constexpr uint length() const noexcept {
        return len;
    }
    constexpr const symb_type* c_str() const noexcept {
        return str;
    }
    constexpr bool isEmpty() const noexcept {
        return len == 0;
    }

    bool isSame(const SimpleStr<K>& other) const noexcept{
        return str == other.str && len == other.len;
    }
    K operator[] (uint idx) const {
        return str[idx];
    }
};

/*
* Класс, заявлящий, что ссылается на нуль-терминированную строку.
* Служит для показателя того, что функция параметром хочет получить
* строку с нулем в конце, например, ей надо дальше передавать его в
* стороннее API. Без этого ей надо было бы либо указывать параметром
* конкретный класс строки, что лишает универсальности, либо приводить
* к постоянным накладным расходам на излишнее копирование строк во временный
* буфер. Источником нуль-терминированных строк могут быть константные строки
* при компиляции, либо классы, хранящие строки.
*/
template<typename K>
struct SimpleStrNt : SimpleStr<K> {
    using symb_type = K;
    using my_type = SimpleStrNt<K>;
    using base = SimpleStr<K>;
    using base::base;

    constexpr static const K empty_string[1] = { 0 };
    static my_type from_pointer(const K* p) {
        return my_type{ p ? p : empty_string, p ? static_cast<uint>(char_traits<K>::length(p)) : 0 };
    }
    constexpr static my_type null() { return my_type{ empty_string, 0 }; }
    operator const K* () const noexcept {
        return base::str;
    }
};

using SimpleStrA = SimpleStr<u8symbol>;
using SimpleStrW = SimpleStr<u16symbol>;
using SimpleStrU = SimpleStr<u32symbol>;
using SimpleStrNtA = SimpleStrNt<u8symbol>;
using SimpleStrNtW = SimpleStrNt<u16symbol>;
using SimpleStrNtU = SimpleStrNt<u32symbol>;
using ssa = SimpleStr<u8symbol>;
using ssw = SimpleStr<u16symbol>;
using ssu = SimpleStr<u32symbol>;

template<typename A>
constexpr static auto operator & (const strexpr<A>& a, const SimpleStr<typename A::symb_type>& s) {
    using J = strexprjoin<A, SimpleStr<typename A::symb_type>>;
    return strexpr<J>{J{ a.a, s }};
}

constexpr static auto operator "" _ss(const u8symbol* p, size_t l) {
    return SimpleStr<u8symbol>{p, static_cast<uint>(l) };
}

constexpr static auto operator "" _ss(const u16symbol* p, size_t l) {
    return SimpleStr<u16symbol>{p, static_cast<uint>(l) };
}

constexpr static auto operator "" _ss(const u32symbol* p, size_t l) {
    return SimpleStr<u32symbol>{p, static_cast<uint>(l) };
}

template<typename K>
static auto e_s(const K* ptr) {
    return SimpleStrNt<K>::from_pointer(ptr);
}

// Когда нужно вернуть из класса объект своего класса, инициализированный шаблонным параметром,
// но применять только публичные конструкторы
template<typename T, typename ...Args>
T return_public(Args&& ...other) {
    return T(forward<Args>(other)...);
}

// Чтобы через ::assign нельзя было вызвать приватные конструкторы класса
// вызываем конструктор через эту прокладку
template<typename T, typename ...Args>
void init_public(T* pThis, Args&& ...other) {
    new (pThis) T(forward<Args>(other)...);
}

template<typename K, bool withSpaces>
struct CheckSpaceTrim {
    bool isTrimSpaces(K s) const {
        return s == ' ' || (s >= 9 && s <= 13);// || isspace(s);
    }
};
template<typename K>
struct CheckSpaceTrim<K, false> {
    bool isTrimSpaces(K s) const {
        return false;
    }
};

template<typename K>
struct CheckSymbolsTrim  {
    SimpleStr<K> symbols;
    bool isTrimSybols(K s) const {
        return symbols.len != 0 && char_traits<K>::find(symbols.str, symbols.len, s) != nullptr;
    }
};

template<typename K, uint N>
struct CheckConstSymbolsTrim {
    const K symbols[N];
    
    CheckConstSymbolsTrim() = default;
    
    template <typename... Characters>
    constexpr CheckConstSymbolsTrim(Characters... characters)
        : symbols{ characters... } {
    }
    
    template <size_t... Indexes>
    constexpr CheckConstSymbolsTrim(const K(&value)[N + 1], index_sequence<Indexes...> dummy) :
        CheckConstSymbolsTrim(value[Indexes]...) {
    }

    constexpr CheckConstSymbolsTrim(const K(&s)[N + 1]) : CheckConstSymbolsTrim(s, typename std::make_index_sequence<N>{}){
    }

    template<uint Idx, enable_if_t<Idx == N, int> = 0>
    constexpr bool isInSymbols(K s) const noexcept {
        return false;
    }

    template<uint Idx, enable_if_t<Idx != N, int> = 0>
    constexpr bool isInSymbols(K s) const noexcept {
        return s == symbols[Idx] || isInSymbols<Idx + 1>(s);
    }

    bool isTrimSybols(K s) const noexcept {
        return isInSymbols<0>(s);
    }
};

template<typename K>
struct CheckConstSymbolsTrim<K, 0> {
    bool isTrimSybols(K s) const {
        return false;
    }
};

template<typename K, uint N>
struct SymbSelector {
    using type = CheckConstSymbolsTrim<K, N>;
};

template<typename K>
struct SymbSelector<K, 0> {
    using type = CheckSymbolsTrim<K>;
};

template<typename K>
struct SymbSelector<K, static_cast<uint>(-1)> {
    using type = CheckConstSymbolsTrim<K, 0>;
};

template<TrimSides S, typename K, uint N, bool withSpaces>
struct trimOperator : SymbSelector<K, N>::type, CheckSpaceTrim<K, withSpaces> {
    bool isTrim(K s) const {
        return CheckSpaceTrim<K, withSpaces>::isTrimSpaces(s) || SymbSelector<K, N>::type::isTrimSybols(s);
    }
    SimpleStr<K> operator()(SimpleStr<K> from) const {
        if constexpr ((S & TrimSides::trimLeft) != 0) {
            while (from.len) {
                if (isTrim(*from.str)) {
                    from.str++;
                    from.len--;
                } else
                    break;
            }
        }
        if constexpr ((S & TrimSides::trimRight) != 0) {
            const K* back = from.str + from.len - 1;
            while (from.len) {
                if (isTrim(*back)) {
                    back--;
                    from.len--;
                } else
                    break;
            }
        }
        return from;
    }
};

template<TrimSides S, typename K>
using SimpleTrim = trimOperator<S, K, 0, true>;
using trim_w = SimpleTrim<TrimSides::trimAll, u16symbol>;
using trim_a = SimpleTrim<TrimSides::trimAll, u8symbol>;
using triml_w = SimpleTrim<TrimSides::trimLeft, u16symbol>;
using triml_a = SimpleTrim<TrimSides::trimLeft, u8symbol>;
using trimr_w = SimpleTrim<TrimSides::trimRight, u16symbol>;
using trimr_a = SimpleTrim<TrimSides::trimRight, u8symbol>;

template<TrimSides S = TrimSides::trimAll, bool withSpaces = false, typename K, uint N, is_const_pattern_t<N> = 0>
static auto trimOp(const K(&pattern)[N]) {
    return trimOperator<S, K, N - 1, withSpaces> {pattern};
}

template<TrimSides S = TrimSides::trimAll, bool withSpaces = false, typename K>
static auto trimOp(SimpleStr<K> pattern) {
    return trimOperator<S, K, 0, withSpaces> {pattern};
}


template<typename Src, typename Dest>
struct utf_convert_selector;

template<>
struct utf_convert_selector<u8symbol, u16symbol> {
    // Максимально один code_point utf8 может преобразоваться в 1 code_unit utf16,
    // а code_point'ов в utf8 максимум может быть столько, сколько code_unit'ов.
    static uint maxSpace(uint len) { return len; }
    COREAS_API static uint convert(const u8symbol* src, uint srcLen, u16symbol* dest);
};

template<>
struct utf_convert_selector<u8symbol, u32symbol> {
    // Максимально один code_point utf8 может преобразоваться в 1 code_unit utf32,
    // а code_point'ов в utf8 максимум может быть столько, сколько code_unit'ов.
    static uint maxSpace(uint len) { return len; }
    COREAS_API static uint convert(const u8symbol* src, uint srcLen, u32symbol* dest);
};

template<>
struct utf_convert_selector<u16symbol, u8symbol> {
    // Максимально один code_unit utf16 может преобразоваться в 3 code_unit utf8,
    // либо 2 code_point utf16 в 4 code_unit utf8
    static uint maxSpace(uint len) { return len * 3; }
    COREAS_API static uint convert(const u16symbol* src, uint srcLen, u8symbol* dest);
};

template<>
struct utf_convert_selector<u16symbol, u32symbol> {
    // Максимально один code_unit utf16 может преобразоваться в 1 code_unit utf32,
    static uint maxSpace(uint len) { return len; }
    COREAS_API static uint convert(const u16symbol* src, uint srcLen, u32symbol* dest);
};

template<>
struct utf_convert_selector<u32symbol, u8symbol> {
    // Максимально один code_point utf32 может преобразоваться в 4 code_unit utf8,
    static uint maxSpace(uint len) { return len * 4; }
    COREAS_API static uint convert(const u32symbol* src, uint srcLen, u8symbol* dest);
};

template<>
struct utf_convert_selector<u32symbol, u16symbol> {
    // Максимально один code_point utf32 может преобразоваться в 2 code_unit utf16,
    static uint maxSpace(uint len) { return len * 2; }
    COREAS_API static uint convert(const u32symbol* src, uint srcLen, u16symbol* dest);
};

template<typename K>
struct utf_init {
    SimpleStr<K> src;
};

template<typename K>
auto fromUtf(SimpleStr<K> t) {
    return utf_init<K>{ t };
}


template<typename K, typename Impl>
class from_utf_convertable {
protected:
    from_utf_convertable() = default;
    using my_type = Impl;
    /*
     Эти методы должен реализовать класс-наследник.
     вызывается только при создании объекта
       init(uint size)
       setSize(uint size)
    */
public:
    template<typename O, enable_if_t<!is_same_v<O, K>, int> = 0>
    from_utf_convertable(utf_init<O> init) {
        using worker = utf_convert_selector<O, K>;
        Impl* d = static_cast<Impl*>(this);
        uint len = init.src.length();
        if (!len)
            d->createEmpty();
        else
            d->setSize(worker::convert(init.src.c_str(), len, d->init(worker::maxSpace(len))));
    }

    template<typename From, enable_if_t<!is_same_v<From, K>, int> = 0>
    static my_type fromOtherUtf(SimpleStr<From> src) {
        return my_type(utf_init{ src });
    }
};

/*
* База для объектов, владеющих строкой
* По прежнему ничего не знает о том, где наследник хранит строку и её размер
* Просто вызывает его методы для получения места, и заполняет его при необходимости.
* Работает только при создании объекта, не работает с модификацией строки после
* ее создания и гарантирует, что если вызываются эти методы, объект еще только
* создается, и какого-либо расшаривания данных еще не было.
* Эти методы должен реализовать класс-наследник, вызываются только при создании объекта
*   K* init(uint size)     - выделить место для строки указанного размера, вернуть адрес
*   void createEmpty()     - создать пустой объект
*   K* setSize(uint size)  - перевыделить место для строки, если при создании не угадали
*                            нужный размер и место нужно больше или меньше.
*                            Содержимое строки нужно оставить.
*
* Также наследник должен реализовывать метод assign, для работы присваивания из любого SimpleStr
* 
* K     - тип символов
* Impl  - тип наследника
*/
template<typename K, typename Impl>
class str_storeable {

    using my_type = Impl;
    using traits = char_traits<K>;
    using uni = unicode_traits<K>;

    Impl& d() noexcept {
        return *static_cast<Impl*>(this);
    }
    const Impl& d() const noexcept {
        return *static_cast<const Impl*>(this);
    }
    explicit constexpr str_storeable(uint size) {
        if (size)
            d().init(size);
        else
            d().createEmpty();
    }

    template<typename From, typename Op1, typename Op2>
    static my_type change_case_ascii(const From& f, const Op1& opCheckWrongCase, const Op2& opMakeNeedCase) {
        if (!f.isEmpty()) {
            const K* ptr = f.c_str();
            for (uint l = 0, len = f.length(); l < len; l++, ptr++) {
                K s = *ptr;
                if (opCheckWrongCase(s)) {
                    my_type res(len);
                    K* pWrite = const_cast<K*>(res.c_str());
                    if (l) {
                        char_traits<K>::copy(pWrite, f.c_str(), l);
                        pWrite += l;
                    }
                    *pWrite++ = opMakeNeedCase(s);
                    for (++l, ++ptr; l < len; l++)
                        *pWrite++ = opMakeNeedCase(*ptr++);
                    *pWrite = 0;
                    return res;
                }
            }
        }
        return return_public<my_type>(f);
    }

    template<typename T>
    struct ChangeCase {
        template<typename From, typename Op1, typename Op2>
        static my_type change_case(const From& f, const Op1& op1, const Op2& op2) {
            if (!f.isEmpty()) {
                const K* ptr = f.c_str();
                uint len = f.length(), first = op1(ptr, len);
                if (first != str_pos::badIdx) {
                    my_type res(len);
                    K* pWrite = const_cast<K*>(res.c_str());
                    if (first > 0)
                        traits::copy(pWrite, ptr, first);
                    op2(ptr + first, len - first, pWrite + first);
                    return res;
                }
            }
            return return_public<my_type>(f);
        }
    };
    // Для utf8 сделаем отдельную спецификацию, так как при смене регистра может изменится длина строки
    template<>
    struct ChangeCase<u8symbol> {
        template<typename From, typename Op1, typename Op2>
        static my_type change_case(const From& f, const Op1& op1, const Op2& op2) {
            if (!f.isEmpty()) {
                const K* ptr = f.c_str();
                uint len = f.length(), first = op1(ptr, len);
                if (first != str_pos::badIdx) {

                    my_type res(len);

                    K* pWrite = const_cast<K*>(res.c_str());
                    if (first > 0)
                        traits::copy(pWrite, ptr, first);
                    const u8symbol* source = ptr + first;
                    u8symbol* dest = pWrite + first;
                    uint lenRemain = len - first;
                    uint newLen = op2(source, lenRemain, dest, lenRemain);
                    if (newLen < lenRemain) {
                        // Строка просто укоротилась
                        res.setSize(newLen + first);
                    } else if (newLen > lenRemain) {
                        // Строка не влезла в буфер.
                        uint readed = static_cast<uint>(source - ptr - first);
                        uint writed = static_cast<uint>(dest - pWrite);
                        newLen += first;
                        pWrite = res.setSize(newLen);
                        dest = pWrite + writed;
                        op2(source, lenRemain - readed, dest, newLen - writed);
                    }
                    pWrite[newLen] = 0;
                    return res;
                }
            }
            return return_public<my_type>(f);
        }
    };

public:

    using my_type = Impl;
    using SimpleStr = SimpleStr<K>;
    using SimpleStrNt = SimpleStrNt<K>;
    
    constexpr str_storeable() {
        d().createEmpty();
    }
    
    // Конструктор из другого строкового объекта
    constexpr str_storeable(SimpleStr other) {
        if (other.length()) {
            K* ptr = d().init(other.length());
            char_traits<K>::copy(ptr, other.c_str(), other.length());
            ptr[other.length()] = 0;
        } else
            d().createEmpty();
    }
    
    // Конструктор из строкового литерала
    template<uint N>
    constexpr str_storeable(const K(&value)[N]) {
        if (N > 1) {
            K* ptr = d().init(N - 1);
            char_traits<K>::copy(ptr, value, N - 1);
            ptr[N - 1] = 0;
        } else
            d().createEmpty();
    }
    
    // Конструктор из строкового выражения
    template<typename A>
    constexpr str_storeable(const strexpr<A>& expr) {
        uint len = expr.length();
        if (len)
            *expr.place(d().init(len)) = 0;
        else
            d().createEmpty();
    }

    operator SimpleStrNt() const noexcept {
        return SimpleStrNt{ d().c_str(), d().length() };
    }
    SimpleStrNt to_nts(uint from = 0) const noexcept {
        uint len = d().length();
        if (from > len)
            from = len;
        return SimpleStrNt{ d().c_str() + from, len - from };
    }

    my_type& operator = (const SimpleStr& other) {
        return d().assign(other);
    }
    // Слияние контейнера строк
    template<typename T>
    static my_type join(const T& strings, SimpleStr delimeter, bool tail = false) {
        if (!strings.size())
            return my_type();
        if (strings.size() == 1 && (!delimeter.length() || !tail))
            return my_type(strings[0]);
        uint commonLen = 0;
        for (auto it = strings.begin(), e = strings.end(); it != e;) {
            commonLen += it->length();
            ++it;
            if (it != e || tail)
                commonLen += delimeter.length();
        }
        if (!commonLen)
            return my_type();

        my_type res(commonLen);	// выделяется память под все строки
        K* ptr = const_cast<K*>(res.c_str());
        for (auto it = strings.begin(), e = strings.end(); it != e; ) {
            uint copyLen = it->length();
            if (copyLen) {
                traits::copy(ptr, it->c_str(), copyLen);
                ptr += copyLen;
            }
            ++it;
            if (delimeter.length() && (it != e || tail)) {
                traits::copy(ptr, delimeter.c_str(), delimeter.length());
                ptr += delimeter.length();
            }
        }
        *ptr = 0;
        return res;
    }
    template<typename From, is_strtype_t<From, K> = 0>
    static my_type uppera_s(const From& f) {
        return change_case_ascii(f, isAsciiLower, makeAsciiUpper);
    }

    // ascii версия lower
    template<typename From, is_strtype_t<From, K> = 0>
    static my_type lowera_s(const From& f) {
        return change_case_ascii(f, isAsciiUpper, makeAsciiLower);
    }

    // Юникодная версия
    template<typename From, is_strtype_t<From, K> = 0>
    static my_type upper_s(const From& f) {
        return ChangeCase<K>::change_case(f, uni::findFirstLower, uni::upper);
    }

    // Юникодная версия
    template<typename From, is_strtype_t<From, K> = 0>
    static my_type lower_s(const From& f) {
        return ChangeCase<K>::change_case(f, uni::findFirstUpper, uni::lower);
    }

    template<typename From, is_strtype_t<From, K> = 0>
    static my_type replace_s(const From& f, SimpleStr pattern, SimpleStr repl, uint offset = 0, uint maxCount = 0) {
        if (f.isEmpty() || !pattern)
            return my_type{ f };
        uint myLen = f.length();
        if (offset + pattern.length() > myLen)
            return my_type{ f };
        auto findes = f.find_all(pattern, offset, maxCount);
        if (!findes.size())
            return my_type{ f };
        uint newSize = myLen + static_cast<uint>((repl.length() - pattern.length()) * findes.size());
        if (!newSize)
            return my_type{};
        
        my_type res(newSize);
        K* ptr = const_cast<K*>(res.c_str());
        uint from = 0;
        for (const auto& s: findes) {
            uint copyLen = s - from;
            if (copyLen) {
                char_traits<K>::copy(ptr, f.c_str() + from, copyLen);
                ptr += copyLen;
            }
            if (repl.length()) {
                char_traits<K>::copy(ptr, repl.c_str(), repl.len);
                ptr += repl.len;
            }
            from = s + pattern.len;
        }
        myLen -= from;
        if (myLen) {
            char_traits<K>::copy(ptr, f.c_str() + from, myLen);
            ptr += myLen;
        }
        *ptr = 0;
        return res;
    }
};

template<typename K> class sstring;

template<typename K> struct printf_selector {};
template<>
struct printf_selector<u8symbol> {
    template<typename ...T>
    static int snprintf(u8symbol* buffer, size_t count, const u8symbol* format, T&& ... args) {
        return std::snprintf(buffer, count, format, forward<T>(args)...);
    }
    static int vsnprintf(u8symbol* buffer, size_t count, const u8symbol* format, va_list args) {
        return std::vsnprintf(buffer, count, format, args);
    }
};

template<>
struct printf_selector<u16symbol> {
    template<typename ...T>
    static int snprintf(u16symbol* buffer, size_t count, const u16symbol* format, T&& ... args) {
        return std::swprintf(buffer, count, format, forward<T>(args)...);
    }
    static int vsnprintf(u16symbol* buffer, size_t count, const u16symbol* format, va_list args) {
        return std::vswprintf(buffer, count, format, args);
    }
};

template<>
struct printf_selector<u32symbol> {
    template<typename ...T>
    static int snprintf(u32symbol* buffer, size_t count, const u32symbol* format, T&& ... args) {
        return 0;// std::swprintf(buffer, count, format, args...);
    }
    static int vsnprintf(u32symbol* buffer, size_t count, const u32symbol* format, va_list args) {
        return 0;// std::vswprintf(buffer, count, format, args);
    }
};

inline static uint grow2(uint ret, uint current) {
    return ret < current ? ret : ret * 2;
}

/*
* Базовый класс работы с меняющимися inplace строками
* По прежнему ничего не знает о том, где наследник хранит строку и её размер
* Просто вызывает его методы для получения места, и заполняет его при необходимости.
* Для работы класс-наследник должен реализовать методы:
*   uint length() const noexcept    - возвращает длину строки
*   const K* c_str() const          - возвращает указатель на начало строки
*   bool isEmpty() const noexcept   - проверка, не пустая ли строка
*   K* str() noexcept               - Неконстантный указатель на начало строки
*   K* setSize(uint size)           - Изменить размер строки, как больше, так и меньше.
*                                     Содержимое строки нужно оставить.
*   K* reserve(uint size)           - выделить место под строку, старую можно не сохранять
*   uint capacity() const noexcept  - вернуть текущую ёмкость строки, сколько может поместится
*                                     без алокации
* 
* K      - тип символов
* StrRef - тип хранилища куска строки
* Impl   - тип наследника
*/
template<typename K, typename StrRef, typename Impl>
class str_mutable {
    using my_type = Impl;
    Impl& d() {
        return *static_cast<Impl*>(this);
    }
    const Impl& d() const {
        return *static_cast<const Impl*>(this);
    }
    uint _len() const noexcept {
        return d().length();
    }
    const K* _str() const noexcept {
        return d().c_str();
    }
    using SimpleStr = StrRef;
    using symb_type = K;
    using traits = char_traits<K>;
    using uni = unicode_traits<K>;
    using uns_type = make_unsigned_t<K>;

    template<typename Op>
    Impl& make_trim_op(const Op& op) {
        SimpleStr me = static_cast<SimpleStr>(d()), pos = op(me);
        if (me.len != pos.len) {
            if (me.str != pos.str)
                char_traits<K>::move(const_cast<K*>(me.str), pos.str, pos.len);
            d().setSize(pos.len);
        }
        return d();
    }

    template<typename Op>
    Impl& common_change_case(const Op& opConvert) {
        uint len = _len();
        if (len)
            opConvert(_str(), len, str());
        return d();
    }
    template<typename T>
    Impl& _UpperTraits() {
        return common_change_case(unicode_traits<K>::upper);
    }
    template<typename T>
    Impl& _LowerTraits() {
        return common_change_case(unicode_traits<K>::lower);
    }

    template<typename Op>
    Impl& utf8_case_change(const Op& op) {
        // Для utf-8 такая операция может изменить длину строки, поэтому для них делаем разные специализации
        uint len = _len();
        if (len) {
            u8symbol* writePos = str();
            const u8symbol* startData = writePos, * readPos = writePos;
            u8symbol* startWrite = str();
            uint newLen = op(readPos, len, writePos, len);
            if (newLen < len) {
                // Строка просто укоротилась
                d().setSize(newLen);
            } else if (newLen > len) {
                // Строка не влезла в буфер.
                uint readed = static_cast<uint>(readPos - startData);
                uint writed = static_cast<uint>(writePos - startData);
                d().setSize(newLen);
                startData = str(); // при изменении размера могло изменится
                readPos = startData + readed;
                writePos = const_cast<u8symbol*>(startData) + writed;
                op(readPos, len - readed, writePos, newLen - writed);
            }
        }
        return d();
    }
    template<>
    Impl& _UpperTraits<u8symbol>() {
        return utf8_case_change(&unicode_traits<u8symbol>::upper);
    }
    template<>
    Impl& _LowerTraits<u8symbol>() {
        return utf8_case_change(&unicode_traits<u8symbol>::lower);
    }

    template<TrimSides S, bool withSpaces, uint N>
    Impl& _makeTrim(const K(&pattern)[N]) {
        return make_trim_op(trimOperator<S, K, N - 1, withSpaces> {pattern});
    }

    template<TrimSides S, bool withSpaces>
    Impl& _makeTrim(SimpleStr pattern) {
        return make_trim_op(trimOperator<S, K, 0, withSpaces> { pattern});
    }

public:
    using my_type = Impl;

    K* str() noexcept {
        return d().str();
    }
    operator K* () noexcept {
        return str();
    }

    Impl& s_trim() {
        return make_trim_op(SimpleTrim<TrimSides::trimAll, K>{});
    }
    Impl& s_trim_l() {
        return make_trim_op(SimpleTrim<TrimSides::trimLeft, K>{});
    }
    Impl& s_trim_r() {
        return make_trim_op(SimpleTrim<TrimSides::trimRight, K>{});
    }

    template<uint N, is_const_pattern_t<N> = 0>
    Impl& s_trim(const K(&pattern)[N]) {
        return _makeTrim<TrimSides::trimAll, false>(pattern);
    }

    template<uint N, is_const_pattern_t<N> = 0>
    Impl& s_trim_l(const K(&pattern)[N]) {
        return _makeTrim<TrimSides::trimLeft, false>(pattern);
    }

    template<uint N, is_const_pattern_t<N> = 0>
    Impl& s_trim_r(const K(&pattern)[N]) {
        return _makeTrim<TrimSides::trimRight, false>(pattern);
    }

    template<uint N, is_const_pattern_t<N> = 0>
    Impl& s_trim_s(const K(&pattern)[N]) {
        return _makeTrim<TrimSides::trimAll, true>(pattern);
    }

    template<uint N, is_const_pattern_t<N> = 0>
    Impl& s_trim_sl(const K(&pattern)[N]) {
        return _makeTrim<TrimSides::trimLeft, true>(pattern);
    }

    template<uint N, is_const_pattern_t<N> = 0>
    Impl& s_trim_sr(const K(&pattern)[N]) {
        return _makeTrim<TrimSides::trimRight, true>(pattern);
    }

    Impl& s_trim(SimpleStr pattern) {
        return pattern.length() ? _makeTrim<TrimSides::trimAll, false>(pattern) : d();
    }
    Impl& s_trim_l(SimpleStr pattern) {
        return pattern.length() ? _makeTrim<TrimSides::trimLeft, false>(pattern) : d();
    }
    Impl& s_trim_r(SimpleStr pattern) {
        return pattern.length() ? _makeTrim<TrimSides::trimRight, false>(pattern) : d();
    }
    Impl& s_trim_s(SimpleStr pattern) {
        return _makeTrim<TrimSides::trimAll, true>(pattern);
    }
    Impl& s_trim_sl(SimpleStr pattern) {
        return _makeTrim<TrimSides::trimLeft, true>(pattern);
    }
    Impl& s_trim_sr(SimpleStr pattern) {
        return _makeTrim<TrimSides::trimRight, true>(pattern);
    }

    Impl& s_uppera() {
        K *ptr = str();
        for (uint i = 0, l = _len(); i < l; i++, ptr++) {
            K s = *ptr;
            if (isAsciiLower(s))
                *ptr = s & ~0x20;
        }
        return d();
    }
    Impl& s_lowera() {
        K* ptr = str();
        for (uint i = 0, l = _len(); i < l; i++, ptr++) {
            K s = *ptr;
            if (isAsciiUpper(s))
                *ptr = s | 0x20;
        }
        return d();
    }
    
    Impl& s_upper() {
        // Для utf-8 такая операция может изменить длину строки, поэтому для них делаем разные специализации
        return _UpperTraits<K>();
    }
    Impl& s_lower() {
        // Для utf-8 такая операция может изменить длину строки, поэтому для них делаем разные специализации
        return _LowerTraits<K>();
    }
    Impl& s_append(SimpleStr o) {
        if (!o.isEmpty()) {
            uint size = _len();
            K* ptr = d().setSize(size + o.length());
            char_traits<K>::copy(ptr + size, o.c_str(), o.length());
        }
        return d();
    }
    template<typename A>
    Impl& s_append(const strexpr<A>& expr) {
        uint len = expr.length();
        if (len) {
            uint size = _len();
            expr.place(d().setSize(size + len) + size);
        }
        return d();
    }
    Impl& s_append_from(uint pos, SimpleStr o) {
        if (pos > _len())
            pos = _len();
        if (!o.isEmpty()) {
            K* ptr = d().setSize(pos + o.length());
            char_traits<K>::copy(ptr + pos, o.c_str(), o.length());
        } else
            d().setSize(pos);
        return d();
    }
    template<typename A>
    Impl& s_append_from(uint pos, const strexpr<A>& expr) {
        if (pos > _len())
            pos = _len();
        uint len = expr.length();
        if (len)
            expr.place(d().setSize(pos + len) + pos);
        else
            d().setSize(pos);
        return d();
    }
    Impl& s_insert(uint to, SimpleStr o) {
        if (!o.isEmpty()) {
            uint myLen = _len();
            if (to > myLen)
                to = myLen;
            K* ptr = d().setSize(myLen + o.length());
            uint tailLen = myLen - to;
            if (tailLen)
                char_traits<K>::move(ptr + to + o.length(), ptr + to, tailLen);
            char_traits<K>::copy(ptr + to, o.c_str(), o.length());
        }
        return d();
    }
    template<typename A>
    Impl& s_insert(uint to, const strexpr<A>& expr) {
        uint exprLen = expr.length();
        if (exprLen) {
            uint myLen = _len();
            if (to > myLen)
                to = myLen;
            K* ptr = d().setSize(myLen + exprLen);
            uint tailLen = myLen - to;
            if (tailLen)
                char_traits<K>::move(ptr + to + exprLen, ptr + to, tailLen);
            expr.place(ptr + to);
        }
        return d();
    }

    Impl& s_prepend(SimpleStr o) {
        if (!o.isEmpty()) {
            uint size = _len();
            K* ptr = d().setSize(size + o.length());
            char_traits<K>::move(ptr + o.length(), ptr, size);
            char_traits<K>::copy(ptr, o.c_str(), o.length());
        }
        return d();
    }
    template<typename A>
    Impl& s_prepend(const strexpr<A>& expr) {
        uint len = expr.length();
        if (len) {
            uint size = _len();
            K* ptr = d().setSize(size + len);
            char_traits<K>::move(ptr + len, ptr, size);
            expr.place(ptr);
        }
        return d();
    }

    Impl& operator += (SimpleStr o) {
        return s_append(o);
    }
    template<typename A>
    Impl& operator += (const strexpr<A>& expr) {
        return s_append(expr);
    }

    Impl& s_replace(SimpleStr pattern, SimpleStr repl, uint offset = 0, uint maxCount = 0) {
        if (d().isEmpty() || !pattern || offset + pattern.length() > _len())
            return d();
        if (!maxCount)
            maxCount--;
        if (pattern.length() >= repl.length()) {
            // Заменяем на такой же или более короткий кусок, длина текста уменьшится, идём слева направо
            K* ptr = str();
            uint posWrite = offset;
            for (uint i = 0; i < maxCount; i++) {
                uint idx = d().find(pattern, offset);
                if (idx == str_pos::badIdx)
                    break;
                uint lenOfPiece = idx - offset;
                if (posWrite < offset && lenOfPiece)
                    char_traits<K>::move(ptr + posWrite, ptr + offset, lenOfPiece);
                posWrite += lenOfPiece;
                if (repl.length()) {
                    char_traits<K>::copy(ptr + posWrite, repl.c_str(), repl.length());
                    posWrite += repl.length();
                }
                offset = idx + pattern.length();
            }
            uint tailLen = _len() - offset;
            if (posWrite < offset && tailLen)
                char_traits<K>::move(ptr + posWrite, ptr + offset, tailLen);
            d().setSize(posWrite + tailLen);
        } else {
            // Заменяем на более длинный кусок, длина текста увеличиться, идём справа налево
            auto finded = d().find_all(pattern, offset, maxCount);
            if (finded.size()) {
                uint delta = repl.length() - pattern.length();
                uint allDelta = uint(delta * finded.size());
                uint endOfPiece = _len();
                K* ptr = d().setSize(endOfPiece + allDelta);
                for (uint i = uint(finded.size()); i--;) {
                    uint pos = finded[i] + pattern.length();
                    uint lenOfPiece = endOfPiece - pos;
                    char_traits<K>::move(ptr + pos + allDelta, ptr + pos, lenOfPiece);
                    char_traits<K>::copy(ptr + pos + allDelta - repl.length(), repl.c_str(), repl.length());
                    allDelta -= delta;
                    endOfPiece = finded[i];
                }
            }
        }
        return d();
    }

    template<typename From, enable_if_t<is_strtype_v<From, K>, int> = 0>
    static Impl& replaceTo_s(Impl& obj, const From& f, SimpleStr pattern, SimpleStr repl, uint offset = 0, uint maxCount = 0) {
        obj.Impl::~Impl();
        return *new (&obj) Impl(Impl::replace_s(f, pattern, repl, offset, maxCount));
    }
    // Реализация заполнения данными с проверкой на длину и перевыделением буфера в случае недостаточной длины.
    template<typename Op>
    my_type& func_fill(uint from, const Op& fillFunction) {
        uint size = _len();
        if (from > size)
            from = size;
        uint capacity = d().capacity();
        K* ptr = str();
        capacity -= from;
        for (;;) {
            uint needSize = fillFunction(ptr + from, capacity);
            if (capacity >= needSize) {
                d().setSize(from + needSize);
                break;
            }
            ptr = from == 0 ? d().reserve(needSize) : d().setSize(from + needSize);
            capacity = needSize;
        }
        return d();
    }
    // Реализация заполнения данными с проверкой на длину и перевыделением буфера в случае недостаточной длины.
    template<typename Op, enable_if_t<is_invocable_v<Op, K*, uint>, int> = 0>
    my_type& operator << (const Op& fillFunction) {
        return func_fill(0, fillFunction);
    }
    // Реализация добавления данных с проверкой на длину и перевыделением буфера в случае недостаточной длины.
    template<typename Op, enable_if_t<is_invocable_v<Op, K*, uint>, int> = 0>
    my_type& operator <<= (const Op& fillFunction) {
        return func_fill(size, fillFunction);
    }
    template<typename Op, enable_if_t<is_invocable_v<Op, my_type&>, int> = 0>
    my_type& operator << (const Op& fillFunction) {
        fillFunction(d());
        return d();
    }

    template<typename... T>
    my_type& format_to(uint from, const K* format, T&& ... args) {
        uint size = _len();
        if (from > size)
            from = size;
        uint capacity = d().capacity();
        K* ptr = str();
        capacity -= from;
        
        int result;
        // Тут грязный хак для u8symbol и wide_char. u8symbol версия snprintf сразу возвращает размер нужного буфера, если он мал
        // а swprintf - возвращает -1. Поэтому для широкой версии надо тупо увеличивать буфер наугад, пока не подойдет
        if constexpr (sizeof(K) == 1) {
            result = printf_selector<K>::snprintf(ptr + from, capacity + 1, format, forward<T>(args)...);
            if (result > (int) capacity) {
                ptr = from == 0 ? d().reserve(result) : d().setSize(from + result);
                result = printf_selector<K>::snprintf(ptr + from, result + 1, format, forward<T>(args)...);
            }
        } else {
            for (uint i = 0; ; i++) {
                result = printf_selector<K>::snprintf(ptr + from, capacity + 1, format, forward<T>(args)...);
                if (result < 0 && i < 4) {
                    // Не хватило буфера или ошибка конвертации.
                    // Попробуем увеличить буфер в два раза, но не более чем всего в 16 раз
                    capacity *= 2;
                    ptr = from == 0 ? d().reserve(capacity) : d().setSize(from + capacity);
                } else
                    break;
            }
        }
        if (result < 0)
            d().setSize(static_cast<uint>(traits::length(_str())));
        else
            d().setSize(from + result);
        return d();
    }
    template<typename... T>
    my_type& s_format(const K* format, T&& ... args) {
        return format_to(0, format, forward<T>(args)...);
    }
    template<typename... T>
    my_type& s_append_f(const K* format, T&& ... args) {
        return format_to(_len(), format, forward<T>(args)...);
    }
    template<typename Op, typename ...Args>
    my_type& with(const Op& fillFunction, Args&& ... args) {
        fillFunction(d(), forward<Args>(args)...);
        return d();
    }
    my_type& s_remove(uint from, uint count) {
        if (from < _len()) {
            uint last = from + count;
            if (last > _len())
                last = _len();
            uint tailLen = _len() - last;
            if (tailLen)
                traits::move(str() + from, _str() + last, tailLen);
            d().setSize(from + tailLen);
        }
        return d();
    }
    template<typename T>
    my_type& s_join(const T& strings, SimpleStr delimeter, bool tail = false) {
        uint commonLen = 0;
        for (auto it = strings.begin(), e = strings.end(); it != e;) {
            commonLen += it->length();
            ++it;
            if (it != e || tail)
                commonLen += delimeter.length();
        }
        K* ptr = d().setSize(commonLen);
        if (commonLen) {
            for (auto it = strings.begin(), e = strings.end(); it != e;) {
                uint copyLen = it->length();
                if (copyLen) {
                    traits::copy(ptr, it->c_str(), copyLen);
                    ptr += copyLen;
                }
                ++it;
                if (delimeter.length() && (it != e || tail)) {
                    traits::copy(ptr, delimeter.c_str(), delimeter.length());
                    ptr += delimeter.length();
                }
            }
        }
        *ptr = 0;
        return d();
    }
};

struct SharedStringData {
    SharedStringData() = default;
    SharedStringData(uint s) : size(s) {}
    atomic<uint> counter{ 1 };	// Счетчик ссылок
    uint size{ 0 };		        // Количество символов
    void incr() { counter++; }// counter.fetch_add(1, memory_order_relaxed); }
    void decr() { if (!--counter) core_as_free(this); }
};

template<typename K, uint N, bool forShared = false> struct expr_lstr;

/*
* Локальная строка. Хранит в себе длину строки, а за ней либо сами данные до N символов + нуль,
* либо если данные длиннее N, то размер выделенного буфера и указатель на данные.
* При этом, если планируется потом результат переместить в sstring, то для динамического буфера
* выделяется +8 байтов, чтобы потом не двигать данные.
* Так как у класса несколько базовых классов, ms компилятор не применяет автоматом empty base optimization,
* и без явного указания - вставит в начало класса пустые байты, сдвинув поле size на 4 байте.
* Укажем ему явно
*/
template<typename K, uint N, bool forShared = false>
class empty_bases lstring :
    public str_algs<K, SimpleStr<K>, lstring<K, N, forShared>, expr_lstr<K, N, forShared>>,
    public str_storeable<K, lstring<K, N, forShared>>,
    public str_mutable<K, SimpleStr<K>, lstring<K, N, forShared>>,
    public from_utf_convertable<K, lstring<K, N, forShared>>
{
    constexpr static uint extra = forShared ? sizeof(SharedStringData) : 0;
    using base_algs = str_algs<K, SimpleStr<K>, lstring<K, N, forShared>, expr_lstr<K, N, forShared>>;
    using base_store = str_storeable<K, lstring<K, N, forShared>>;
    using base_mutable = str_mutable<K, SimpleStr<K>, lstring<K, N, forShared>>;
    using base_utf = from_utf_convertable<K, lstring<K, N, forShared>>;
    using traits = char_traits<K>;

    friend base_store;
    friend base_mutable;
    friend base_utf;
    friend class sstring<K>;

    // Данные
    uint size;
    union Buffer {
        K local[N + 1] ;
        struct {
            uint bufSize;
            K* data;
        } big;
    } buffer;

    void createEmpty() {
        size = 0;
        buffer.big.bufSize = 0;
    }
    K* init(uint s) {
        size = s;
        if (size > N) {
            buffer.big.data = fromRealAddress(core_as_malloc((s + 1) * sizeof(K) + extra));
            buffer.big.bufSize = s;
        }
        return str();
    }
    // Методы для себя
    bool isBig() const noexcept {
        return size > N;
    }
    void dealloc() {
        if (isBig())
            core_as_free(toRealAddress(buffer.big.data));
    }

    static K* toRealAddress(void* ptr) {
        return reinterpret_cast<K*>(reinterpret_cast<u8symbol*>(ptr) - extra);
    }
    static K* fromRealAddress(void* ptr) {
        return reinterpret_cast<K*>(reinterpret_cast<u8symbol*>(ptr) + extra);
    }

    uint capacity() const noexcept {
        return isBig() ? buffer.big.bufSize : N;
    }

public:
    using my_type = lstring<K, N, forShared>;
    using base_store::base_store;
    using base_utf::base_utf;
    using symb_type = K;
    
    lstring() = default;

    ~lstring() {
        dealloc();
    }
    // Копирование из другой строки с таким же размером буфера
    lstring(const my_type& other) {
        if (other.size)
            traits::copy(reserve(other.size), other.c_str(), other.size + 1);
    }
    // Перемещение из другой строки с таким же размером буфера
    lstring(my_type&& other) {
        if (other.size) {
            if (other.size > N) {
                size = other.size;
                buffer.big = other.buffer.big;
                other.size = 0;
            } else
                traits::copy(reserve(other.size), other.c_str(), other.size + 1);
        }
    }
    // Копирование из строки другого размера
    template<uint L, bool S, enable_if_t<L != N, int> = 0>
    lstring(const lstring<K, L, S>& other) {
        uint len = other.length();
        if (len)
            traits::copy(reserve(len), other.c_str(), len + 1);
    }

    template<typename Op, enable_if_t<is_invocable_v<Op, K*, uint>, int> = 0>
    lstring(const Op& op) {
        this->operator<<(op);
    }
    template<typename Op, enable_if_t<is_invocable_v<Op, my_type&>, int> = 0>
    lstring(const Op& op) {
        this->operator<<(op);
    }

    // Присвоение с параметрами конструктора
    template<typename... T>
    my_type& assign(T&&... args) {
        this->~lstring();
        init_public<my_type>(this, forward<T>(args)...);
        return *this;
    }
    my_type& assign(const my_type& other) {
        if (&other != this) {
            this->~lstring();
            new (this) my_type(other);
        }
        return *this;
    }
    my_type& operator = (my_type&& other) {
        return assign(move(other));
    }

    my_type& operator = (const SimpleStr<K>& other) {
        return assign(other);
    }
    template<typename A>
    my_type& operator = (const strexpr<A>& expr) {
        return assign(expr);
    }

    uint length() const noexcept {
        return size;
    }
    const K* c_str() const noexcept {
        return isBig() ? buffer.big.data : buffer.local;
    }
    K* str() noexcept {
        return isBig() ? buffer.big.data : buffer.local;
    }
    bool isEmpty() const noexcept {
        return size == 0;
    }

    // Выделить буфер, достаточный для размещения newSize символов плюс завершающий ноль
    // Содержимое буфера неопределено, и не гарантируется сохранение старого содержимого
    K* reserve(uint newSize) {
        if (newSize != size) {
            if (isBig()) {
                // У нас есть динамический буфер
                if (newSize <= N) // и он теперь не нужен
                    dealloc();
                else if (newSize > buffer.big.bufSize) {
                    // он недостаточен
                    buffer.big.data = fromRealAddress(core_as_realloc(toRealAddress(buffer.big.data), (newSize + 1) * sizeof(K) + extra));
                    buffer.big.bufSize = newSize;
                }
            } else {
                // У нас локальный буфер
                if (newSize > N) {
                    // А нужен динамический
                    buffer.big.data = fromRealAddress(core_as_malloc((newSize + 1) * sizeof(K) + extra));
                    buffer.big.bufSize = newSize;
                }
            }
            size = newSize;
        }
        return str();
    }
    // Устанавливает размер текущей строки. При необходмости перемещает данные в другой буфер
    // Содержимое сохраняется
    K* setSize(uint newSize) {
        if (newSize != size) {
            if (isBig()) {
                // У нас есть динамический буфер
                if (newSize <= N) {
                    // А нужен локальный. Скопируем нужное количество данных и освободим память
                    K* dynBuffer = buffer.big.data; // Копирование перезатрет данные
                    if (newSize)
                        traits::copy(buffer.local, dynBuffer, newSize);
                    core_as_free(toRealAddress(dynBuffer));
                } else if (newSize > buffer.big.bufSize) {
                    // динамический буфер недостаточный по длине. Расширим его. При этом для строго динамических строк с запасом в полтора раза
                    buffer.big.data = fromRealAddress(core_as_realloc(toRealAddress(buffer.big.data), (newSize + 1 + (N == 0 ? newSize / 2 : 0)) * sizeof(K) + extra));
                    buffer.big.bufSize = newSize + (N == 0 ? newSize / 2 : 0);
                }
            } else {
                // У нас локальный буфер
                if (newSize > N) {
                    // А нужен динамический
                    K* dynBuffer = fromRealAddress(core_as_malloc((newSize + 1) * sizeof(K) + extra));
                    if (size)
                        traits::copy(dynBuffer, buffer.local, size + 1);
                    buffer.big.data = dynBuffer;
                    buffer.big.bufSize = newSize;
                }
            }
            size = newSize;
        }
        K* res = str();
        res[newSize] = 0;
        return res;
    }
};

template<uint N> using lstringa = lstring<u8symbol, N>;
template<uint N> using lstringw = lstring<u16symbol, N>;
template<uint N> using lstringsa = lstring<u8symbol, N, true>;
template<uint N> using lstringsw = lstring<u16symbol, N, true>;

template<typename K, uint N, bool S>
struct expr_lstr {
    using symb_type = K;
    const lstring<K, N, S>& a;
    // Работа с strexpr
    constexpr uint length() const noexcept {
        return a.length();
    }
    constexpr symb_type* place(symb_type* ptr) const noexcept {
        return a.place(ptr);
    }
};

template<typename A, uint N, bool S>
constexpr static auto operator & (const strexpr<A>& a, const lstring<typename A::symb_type, N, S>& s) {
    using J = strexprjoin<A, expr_lstr<typename A::symb_type, N, S>>;
    return strexpr<J>{J{ a.a, s }};
}

template<typename K> struct expr_sstring;

// Реализация разделямых строк.
// Объект "строка" состоит из двух "частей" - сам объект sstring, в котором просто
// лежит указатель (по сути просто RAII обертка вокруг указателя) и разделяемый буфер строки,
// на который он указывает. На один буфер могут указывать несколько объектов sstring.
// В буфере лежат сами символы, длина строки и счетчик ссылок.
// Сам буфер строки абсолютно потокобезопасный - так как сами байты строки не модифицируются,
// а подсчет ссылок делается атомарными операциями.
// Размер самого объекта равен размеру обычного указателя на строку.
// Указывает непосредственно на байты строки, поэтому никаких накладных расходов
// выполнения по сравнению с обычным указателем - нет. Непосредственно перед байтами
// строки лежат - длина и счетчик ссылок, оба unsigned int. Это все дополнительные
// расходы по хранению по сравнению с голым указателем. Буфер строки может быть
// разделяем между несколькими объектами и потоками.
// Сама строка (байты) после присвоения immutable - неизменяема. Все строковые операции
// возвращают новый объект, оставляя саму строку неизменной. Сделано это для уменьшения
// накладных расходов по синхронизации использования буфера в разных потоках.
// Строка в буфере null-терминирована, терминатор в длину строки не входит.
// Однако методы объекта работают со строкой, ориентируясь на ее длину, а не на символ 0,
// поэтому строка может содержать и нулевые символы.

template<typename K>
class empty_bases sstring :
    public str_algs<K, SimpleStr<K>, sstring<K>, expr_sstring<K>>,
    public str_storeable<K, sstring<K>>,
    public from_utf_convertable<K, sstring<K>> 
{
protected:
    using base_algs = str_algs<K, SimpleStr<K>, sstring<K>, expr_sstring<K>>;
    using base_store = str_storeable<K, sstring<K>>;
    using base_utf = from_utf_convertable<K, sstring<K>>;

    friend base_store;
    friend base_utf;
    friend class AsStringW;

    void createEmpty() {
        str = getEmptyString();
    }
    K* init(uint s) {
        str = reinterpret_cast<K*>(new (core_as_malloc((s + 1) * sizeof(K) + sizeof(SharedStringData))) SharedStringData(s) + 1);
        return str;
    }
    K* setSize(uint len) {
        // вызывается при создании строки при необходимости изменить размер
        // других ссылок на shared bufer нет
        if (len) {
            if (len > length() || (len > 32 && len < length() * 3 / 4))
                str = reinterpret_cast<K*>(reinterpret_cast<SharedStringData*>(core_as_realloc(getData(), (len + 1) * sizeof(K) + sizeof(SharedStringData))) + 1);
            getData()->size = len;
            str[len] = 0;
        } else
            str = getEmptyString();
        return str;
    }

    // Вот, собственно, и все данные в объекте
    K* str;	// Адрес строки байтов. Data лежит перед ними.

    SharedStringData* getData() const { return reinterpret_cast<SharedStringData*>(str) - 1; }
    /*
    * Это один единый экземпляр "пустой строки". Все объекты строки нулевой длины указывают
    * на этот экземпляр. Это позволяет достичь того, что поле "str" всегда не нулевое и
    * его не надо проверять на nullptr перед любой строковой операцией.
    * Позволяет избежать лишних атомарных операций при копировании/уничтожении для пустых строк
    * Также так как в аллоцированных строках адрес строки всегда кратен 8, а здесь мы возвращаем
    * адрес 12го элемента (кратно 8 + 4), то это позволит сразу по адресу узнать, пустая строка или нет.
    */
    static K* getEmptyString() {
        alignas(16) static const u8symbol emptyString[16] = {};
        return (K*)(emptyString + 12);
    }

    using traits = char_traits<K>;
    using uni = unicode_traits<K>;

public:
    using base_store::base_store;
    using base_utf::base_utf;

    using symb_type = K;
    using uns_type = make_unsigned_t<K>;
    using my_type = sstring<K>;

    sstring() = default;
    ~sstring() {
        if (!isEmpty())
            getData()->decr();
    }
    sstring(const my_type& other) noexcept: str(other.str) {
        if (!isEmpty())
            getData()->incr();
    }
    sstring(my_type&& other) noexcept: str(other.str) {
        other.str = getEmptyString();
    }
    
    sstring(K pad, uint count) {
        if (count) {
            traits::assign(init(count), count, pad);
            str[count] = 0;
        } else
            str = getEmptyString();
    }

    // Конструктор перемещения из локальной строки
    template<uint N>
    sstring(lstring<K, N, true>&& src) {
        if (src.size) {
            if (src.size > N) {
                // Там динамический буфер, выделенный с запасом для SharedStringData. Просто присвоим его себе.
                str = src.str();
                new (getData()) SharedStringData(src.size);
            } else {
                // Копируем из локального буфера
                traits::copy(init(src.size), src.c_str(), src.size);
                str[src.size] = 0;
            }
            src.size = 0;
        } else
            str = getEmptyString();
    }

    static COREAS_API const my_type strnull;

    template<typename... T>
    my_type& assign(T&&... args) {
        /*
        * Если делать так:
        *    cstring<u8symbol> t("text");
        *    t.assign(t.c_str(), t.length())
        * то будет падать
        * Можно добавить временный объект для лока буфера:
        *    my_type t(*this);
        * но это добавит две атомарные операции.
        * Поэтому из соображений производительности так делать не будем, просто не пишите такой код
        */
        this->~sstring();
        init_public<my_type>(this, forward<T>(args)...);
        return *this;
    }
    my_type& assign(const my_type& other) {
        if (&other != this) {
            this->~sstring();
            new (this) my_type(other);
        }
        return *this;
    }

    // Операции по переприсваиванию строки. Переприсваивая строку важно осознавать, что
    // это делать небезопасно, если в других потоках имеется ссылка на конкретно этот
    // объект. Другие объекты, ссылающиеся на тот же буфер строки, что и этот - опасности не
    // создают и этой оперцией затронуты не будут.
    my_type& operator = (const my_type& other) noexcept {
        return assign(other);
    }
    my_type& operator = (my_type&& other) noexcept {
        return assign(move(other));
    }
    my_type& operator = (const SimpleStr<K>& other) {
        return assign(other);
    }
    template<typename A>
    my_type& operator = (const strexpr<A>& expr) {
        return assign(expr);
    }
    my_type& empty() {
        this->sstring::~sstring();
        new (this) my_type();
        return *this;
    }

    operator const K*() const noexcept {
        return str;
    }
    const K* c_str() const noexcept {
        return str;
    }
    uint length() const noexcept {
        return getData()->size;
    }
    bool isEmpty() const noexcept {
        return (reinterpret_cast<size_t>(str) & 7) != 0;
    }

    // Форматирование строки.
    template<typename ...T>
    static my_type format(const K* format, T&& ... args) {
        return my_type(move(lstring<K, 200, true>().s_format(format, forward<T>(args)...)));
    }
};

template<typename K>
struct expr_sstring {
    using symb_type = K;
    const sstring<K>& a;
    constexpr uint length() const noexcept { return a.length(); }
    constexpr symb_type* place(symb_type* p) const noexcept { return a.place(p); }
};

template<typename A>
constexpr static auto operator & (const strexpr<A>& a, const sstring<typename A::symb_type>& s) {
    using J = strexprjoin<A, expr_sstring<typename A::symb_type>>;
    return strexpr<J>{ J{ a.a, s } };
}

template<typename K, uint N> K getLiteralType(const K(&)[N]) { return {}; };

#define SU(par) ([]() -> const sstring<decltype(getLiteralType(par))>& {\
    static sstring<decltype(getLiteralType(par))> s(par);return s;}())

/*
* Для создания строковых конкатенаций с десятичными целыми числами
* Например
* 
*     stringw a = +L"res1="_ss & resVal1 & L" res2=" & resVal2;
* 
*/

template<uint I>
struct digits_selector {
    using wider_type = uint16_t;
};

template<>
struct digits_selector<2> {
    using wider_type = uint32_t;
};

template<>
struct digits_selector<4> {
    using wider_type = uint64_t;
};

template<typename K, bool I, typename T>
struct need_sign {
    bool sign;
    need_sign(T& t) : sign(t < 0) { if (sign) t = -t; }
    void after(K*& ptr) { if (sign) *--ptr = '-'; }
};

template<typename K, typename T>
struct need_sign<K, false, T> {
    need_sign(T&) {}
    void after(K*&) {}
};

template<typename K, typename T>
constexpr uint fromInt(K* bufEnd, T val) {
    using wider_t = typename digits_selector<sizeof(K)>::wider_type;
    if (val) {
        need_sign<K, T(-1) < 0, T> sign(val);
        K* itr = bufEnd;
        for (;;) {
            if (val >= static_cast<T>(10)) {
                itr -= 2;
                T temp_v = val / 100, rmd = val % 100;
                wider_t t = static_cast<wider_t>('0' + rmd / 10) | (static_cast<wider_t>('0' + rmd % 10) << sizeof(K) * 8);
                *(wider_t*) itr = t;
                val = temp_v;
            } else {
                if (val)
                    *(--itr) = static_cast<K>('0' + val);
                break;
            }
        }
        sign.after(itr);
        return uint(bufEnd - itr);
    }
    bufEnd[-1] = '0';
    return 1;
}

template<typename K, typename T>
struct expr_num {
    using symb_type = K;
    using my_type = expr_num<K, T>;
    
    enum {bufSize = 25 };
    T value;
    mutable K buf[bufSize];
    mutable uint l;
    
    uint length() const noexcept {
        l = fromInt(buf + bufSize, value);
        return l;
    }
    K* place(K* ptr) const noexcept {
        char_traits<K>::copy(ptr, buf + bufSize - l, l);
        return ptr + l;
    }
};

template<typename A, typename T>
constexpr static auto operator & (const strexpr<A>& a, const expr_num<typename A::symb_type, T>& s) {
    using J = strexprjoin<A, expr_num<typename A::symb_type, T>>;
    return strexpr<J>{ J{ a.a, s } };
}

template <typename T>
inline constexpr bool is_number_v = is_one_of_type<remove_cv_t<T>, int, short, long, long long, uint, unsigned short, unsigned long, unsigned long long>::value;

template<typename A, typename T, enable_if_t<is_number_v<T>, int> = 0>
constexpr static auto operator & (const strexpr<A>& a, T s) {
    using K = typename A::symb_type;
    using J = strexprjoin<A, expr_num<K, T>>;
    return strexpr<J>{ J{ a.a, expr_num<K, T>{s} } };
}

/*
* Для создания строковых конкатенаций с векторами и списками, сджойненными константным разделителем
* K - тип символов строки
* T - тип контейнера строк (vector, list)
* I - длина разделителя в символах
* tail - добавлять разделитель после последнего элемента контейнера.
*        Если контейнер пустой, разделитель в любом случае не добавляется
* Например:
*   vector<stringw> vec = source.split<vector<stringw>>(CS(L" "));
*   stringw res =
*       +L"res size "_ss & vec.size() & L":" &
*       e_choice(
*           vec.size() > 0,
*           +L"\n"_ss & e_ls(vec, L", ") & L"\n",
*           +L" "_ss
*       ) &
*       L"<done>";
*/

template<typename K, typename T, uint I, bool tail>
struct expr_lst {
    using symb_type = K;
    using my_type = expr_lst<K, T, I, tail>;
    
    const T& s;
    const K* delim;
    
    constexpr uint length() const noexcept {
        uint l = 0;
        for (const auto& t : s)
            l += t.length() + I;
        return l - (l != 0 && !tail ? I : 0);
    }
    constexpr K* place(K* ptr) const noexcept {
        for (auto t = s.begin(), e = s.end(); ;) {
            uint copyLen = t->length();
            if (copyLen) {
                char_traits<K>::copy(ptr, t->c_str(), copyLen);
                ptr += copyLen;
            }
            ++t;
            if (t == e) {
                // Последний элемент контейнера
                if constexpr (I > 0 && tail) {
                    char_traits<K>::copy(ptr, delim, I);
                    ptr += I;
                }
                break;
            }
            if constexpr (I > 0) {
                char_traits<K>::copy(ptr, delim, I);
                ptr += I;
            }
        }
        return ptr;
    }
};

template<typename A, typename T, size_t I, bool t>
constexpr static auto operator & (const strexpr<A>& a, const expr_lst<typename A::symb_type, T, I, t>& s) {
    using J = strexprjoin<A, expr_lst<typename A::symb_type, T, I, t>>;
    return strexpr<J>{J{ a.a, expr_lst<typename A::symb_type, T, I, t>{s} }};
}

template<bool t = false, uint I, typename K, typename T>
constexpr static auto e_ls(const T& s, const K(&d)[I]) {
    return expr_lst<K, T, I - 1, t>{ s, d };
}

template<typename K, uint N, uint L>
struct expr_replaces {
    using symb_type = K;
    using my_type = expr_replaces<K, N, L>;
    SimpleStr<K> what;
    const K* pattern;
    const K* repl;
    mutable vector<uint> positions;

    constexpr expr_replaces(SimpleStr<K> w, const K* p, const K* r) : what(w), pattern(p), repl(r) {}

    constexpr uint length() const {
        positions = what.find_all(SimpleStr<K>{pattern, N});
        return what.length() + static_cast<int>(L - N) * static_cast<uint>(positions.size());
    }
    constexpr K* place(K* ptr) const noexcept {
        uint from = 0;
        for (uint k : positions) {
            uint copyLen = k - from;
            if (copyLen) {
                char_traits<K>::copy(ptr, what.c_str() + from, copyLen);
                ptr += copyLen;
            }
            if constexpr (L > 0) {
                char_traits<K>::copy(ptr, repl, L);
                ptr += L;
            }
            if constexpr (N > 0)
                from = k + N;
        }
        uint tailLen = what.length() - from;
        if (tailLen) {
            char_traits<K>::copy(ptr, what.c_str() + from, tailLen);
            ptr += tailLen;
        }
        return ptr;
    }
};

template<typename A, uint N, uint L>
constexpr static auto operator & (const strexpr<A>& a, const expr_replaces<typename A::symb_type, N, L>& s) {
    using J = strexprjoin<A, expr_replaces<typename A::symb_type, N, L>>;
    return strexpr<J>{J{ a.a, s }};
}

template<typename K, uint N, uint L, enable_if_t<(N > 1), int> = 0>
constexpr static auto e_repl(SimpleStr<K> w, const K(&p)[N], const K(&r)[L]) {
    return expr_replaces<K, N - 1, L - 1>{w, p, r};
}

template<typename A, typename B>
struct expr_choice {
    using symb_type = typename A::symb_type;
    using my_type = expr_choice<A, B>;
    A a;
    B b;
    bool choice;

    constexpr uint length() const noexcept {
        return choice ? a.length() : b.length();
    }
    constexpr symb_type* place(symb_type* ptr) const noexcept {
        return choice ? a.place(ptr) : b.place(ptr);
    }
    constexpr strexpr<my_type> operator + () const noexcept {
        return strexpr<my_type>(*this);
    }
};

template<typename A, typename B, typename C>
constexpr static auto operator & (const strexpr<A>& a, const expr_choice<B, C>& s) {
    using J = strexprjoin<A, expr_choice<B, C>>;
    return strexpr<J>{J{ a.a, s }};
}

template<typename A, typename B>
constexpr static auto e_choice(bool c, const A& a, const B& b) {
    return expr_choice<A, B> {a, b, c};
}

template<size_t ... Is>
constexpr auto indexSequenceReverse(index_sequence<Is...> const&) -> decltype(std::index_sequence<sizeof...(Is)-1U-Is...>{});
template <size_t N>
using makeIndexSequenceReverse = decltype(indexSequenceReverse(make_index_sequence<N>{}));


template<typename K>
static size_t fnv_hash(const K* ptr, uint l) {
    size_t h = _FNV_offset_basis;
    while (l--)
        h = h ^ *ptr++ * _FNV_prime;
    return h;
}

template<typename K>
constexpr size_t fnv_hash() {
    return _FNV_offset_basis;
};

template<typename K, typename... Chars>
constexpr size_t fnv_hash(K symb, Chars... chrs) {
    return (fnv_hash<K>(chrs...) ^ symb) * _FNV_prime;
};

template <typename K, size_t N, size_t... Indexes>
constexpr auto HashFactory(const K(&value)[N], std::index_sequence<Indexes...> dummy) {
    return fnv_hash<K>(value[Indexes]...);
}

template<typename K, uint N>
constexpr static auto get_hash(const K(&value)[N]) {
    return HashFactory(value, makeIndexSequenceReverse<N - 1> {});
}

template<typename K>
constexpr size_t fnv_hash_ia() {
    return _FNV_offset_basis;
};

template<typename K, typename... Chars>
constexpr size_t fnv_hash_ia(K symb, Chars... chrs) {
    return (fnv_hash_ia<K>(chrs...) ^ (symb >='A' && symb <= 'Z' ? symb | 0x20 : symb)) * _FNV_prime;
};

template <typename K, size_t N, size_t... Indexes>
constexpr auto HashFactory_ia(const K(&value)[N], std::index_sequence<Indexes...> dummy) {
    return fnv_hash_ia<K>(value[Indexes]...);
}

template<typename K, uint N>
constexpr static auto get_hash_ia(const K(&value)[N]) {
    return HashFactory_ia(value, makeIndexSequenceReverse<N - 1> {});
}

template<typename K>
struct StoreType {
    using list_t = list<sstring<K>>;
    using str_it_node = typename list_t::iterator;

    SimpleStr<K> str;
    size_t hash;
    char node[sizeof(str_it_node)];

    const SimpleStrNt<K>& to_nt() const noexcept {
        return static_cast<const SimpleStrNt<K>&>(str);
    }
    const sstring<K>& to_str() const noexcept {
        return **reinterpret_cast<const str_it_node*>(node);
    }
};

using HashKeyA = StoreType<u8symbol>;
using HashKeyW = StoreType<u16symbol>;
using HashKeyU = StoreType<u32symbol>;

// Путем эксперементов выяснилось, что для константных строковых литералов до такой длины
// компилятор может вычислить хеш на этапе компиляции и сразу подставить константу,
// для более длинных строк - нет, а разворачивает в вызов функции, а потом инлайнит всю рекурсию.
// Получается нехорошо. Поэтому для более длинных строк будем задавать более простое вычисление хеша
// в рантайме. Не используйте столь длинные строки как прекомпиленные ключи поиска.
constexpr static const size_t MaxStaticHashLen = 31;

template<typename K, uint N, enable_if_t<(N > MaxStaticHashLen + 1), int> = 0>
constexpr static auto strh(const K(&value)[N]) {
    return StoreType<K> { { value, N - 1 }, fnv_hash(value, N - 1)};
}

template<typename K, uint N, enable_if_t<(N > MaxStaticHashLen + 1), int> = 0>
constexpr static auto strhia(const K(&value)[N]) {
    return StoreType<K> { { value, N - 1 }, unicode_traits<K>::hashia(value, N - 1)};
}

template<typename K, uint N, enable_if_t<N <= MaxStaticHashLen + 1, int> = 0>
constexpr static auto strh(const K(&value)[N]) {
    return StoreType<K> { {value, N - 1 }, get_hash(value)};
}

template<typename K, uint N, enable_if_t<N <= MaxStaticHashLen + 1, int> = 0>
constexpr static auto strhia(const K(&value)[N]) {
    return StoreType<K> { {value, N - 1 }, get_hash_ia(value)};
}

/*
* Контейнер для более эффективного поиска по строковым ключам.
* Как unordered_map, но чуть лучше. В качестве ключей хранит SimpleStr вместе с посчитанным хешем.
* Чтобы SimpleStr было на что ссылатся, строковые значения ключей кладёт в список,
* с ключом запоминает позицию в списке. При удалении ключа удаляет и из списка.
* Позволяет для поиска использовать строковые литералы, не создавая для них объекта sstring.
* Начиная с С++20 в unordered_map появилась возможность для поиска по ключу с типом, отличным от типа
* хранящегося ключа, но у нас С++17. Да и там скорее всего хэш тоже не хранит, каждый раз вычисляя заново.
*/
template<typename K, typename T, typename H = hash<StoreType<K>>, typename E = equal_to<StoreType<K>>>
class hashStrMap {
protected:
    using InStore = StoreType<K>;
    using list_t = typename InStore::list_t;
    unordered_map<InStore, T, H, E> hashStore;
    list_t strings;
public:
    using hasher = H;
    using hash_t = unordered_map<InStore, T, H, E>;
    // При входе хэш должен быть уже посчитан
    template<typename...ValArgs>
    auto emplace(const StoreType<K>& key, ValArgs&&... args) {
        auto it = hashStore.try_emplace(key, forward<ValArgs>(args)...);
        if (it.second) {
            InStore& stored = const_cast<InStore&>(it.first->first);
            stored.str.str = strings.emplace_back(key.str).c_str();
            new (stored.node) typename InStore::str_it_node(--strings.end());
        }
        return it;
    }
    template<typename...ValArgs>
    auto emplace(const SimpleStr<K>& key, ValArgs&&... args) {
        return emplace(StoreType<K>{ key, H()(key) }, forward<ValArgs>(args)...);
    }
    template<typename...ValArgs>
    auto emplace_or_assign(const StoreType<K>& key, ValArgs&&... args) {
        auto it = emplace(key, forward<ValArgs>(args)...);
        if (!it.second) {
            it.first->second.T::~T();
            new (&it.first->second) T(forward<ValArgs>(args)...);
        }
        return it;
    }
    template<typename...ValArgs>
    auto emplace_or_assign(const SimpleStr<K>& key, ValArgs&&... args) {
        return emplace_or_assign(StoreType<K>{ key, H()(key) }, forward<ValArgs>(args)...);
    }
    auto find(const StoreType<K>& key) const {
        return hashStore.find(key);
    }
    auto find(const SimpleStr<K>& key) const {
        return find(StoreType<K>{ key, H()(key) });
    }
    auto find(const StoreType<K>& key) {
        return hashStore.find(key);
    }
    auto find(const SimpleStr<K>& key) {
        return find(StoreType<K>{ key, H()(key) });
    }
    auto erase(const StoreType<K>& key) {
        auto it = hashStore.find(key);
        if (it != hashStore.end()) {
            strings.erase(*(typename InStore::str_it_node*)it->first.node);
            hashStore.erase(it);
            return 1;
        }
        return 0;
    }
    auto erase(const SimpleStr<K>& key) {
        return erase(StoreType<K> { key, H()(key) });
    }
    auto begin() const {
        return hashStore.begin();
    }
    auto end() const {
        return hashStore.end();
    }
    bool lookup(const K* txt, T& val) const {
        auto it = find(e_s(txt));
        if (it != hashStore.end()) {
            val = it->second;
            return true;
        }
        return false;
    }
    bool lookup(SimpleStr<K> txt, T& val) const {
        auto it = find(txt);
        if (it != hashStore.end()) {
            val = it->second;
            return true;
        }
        return false;
    }
    size_t size() const {
        return hashStore.size();
    }
    void clear() {
        hashStore.clear();
        strings.clear();
    }
};

template<typename K>
struct streql {
    bool operator()(const StoreType<K>& _Left, const StoreType<K>& _Right) const {
        return _Left.hash == _Right.hash && _Left.str == _Right.str;
    }
};

template<typename K>
struct strhash {	// hash functor for basic_string
    size_t operator()(const SimpleStr<K>& _Keyval) const {
        return fnv_hash(_Keyval.c_str(), _Keyval.length());
    }
    size_t operator()(const StoreType<K>& _Keyval) const {
        return _Keyval.hash;
    }
};

template<typename K>
struct streqlia {
    bool operator()(const StoreType<K>& _Left, const StoreType<K>& _Right) const {
        return _Left.hash == _Right.hash && _Left.str.isEqualia(_Right.str);
    }
};

template<typename K>
struct strhashia {
    size_t operator()(const SimpleStr<K>& _Keyval) const {
        return unicode_traits<K>::hashia(_Keyval.c_str(), _Keyval.length());
    }
    size_t operator()(const StoreType<K>& _Keyval) const {
        return _Keyval.hash;
    }
};

template<typename K>
struct streqliu {
    bool operator()(const StoreType<K>& _Left, const StoreType<K>& _Right) const {
        return _Left.hash == _Right.hash && _Left.str.isEqualiu(_Right.str);
    }
};

template<typename K>
struct strhashiu {
    size_t operator()(const SimpleStr<K>& _Keyval) const {
        return unicode_traits<K>::hashiu(_Keyval.c_str(), _Keyval.length());
    }
    size_t operator()(const StoreType<K>& _Keyval) const {
        return _Keyval.hash;
    }
};

using stringu = sstring<u32symbol>;
using stringw = sstring<u16symbol>;
using stringa = sstring<u8symbol>;

template<typename T>
using hashStrMapA = hashStrMap<u8symbol, T, strhash<u8symbol>, streql<u8symbol>>;
template<typename T>
using hashStrMapAIA = hashStrMap<u8symbol, T, strhashia<u8symbol>, streqlia<u8symbol>>;
template<typename T>
using hashStrMapAIU = hashStrMap<u8symbol, T, strhashiu<u8symbol>, streqliu<u8symbol>>;

template<typename T>
using hashStrMapW = hashStrMap<u16symbol, T, strhash<u16symbol>, streql<u16symbol>>;
template<typename T>
using hashStrMapWIA = hashStrMap<u16symbol, T, strhashia<u16symbol>, streqlia<u16symbol>>;
template<typename T>
using hashStrMapWIU = hashStrMap<u16symbol, T, strhashiu<u16symbol>, streqliu<u16symbol>>;

template<typename T>
using hashStrMapU = hashStrMap<u32symbol, T, strhash<u32symbol>, streql<u32symbol>>;
template<typename T>
using hashStrMapUIA = hashStrMap<u32symbol, T, strhashia<u32symbol>, streqlia<u32symbol>>;
template<typename T>
using hashStrMapUIU = hashStrMap<u32symbol, T, strhashiu<u32symbol>, streqliu<u32symbol>>;

static_assert(sizeof(sstring<u8symbol>) == sizeof(u8symbol*));

constexpr const SimpleStr<u8symbol> utf8_bom{ "\xEF\xBB\xBF", 3};
