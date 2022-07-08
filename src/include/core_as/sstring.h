/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Классы для работы со строками
*/
#pragma once
#include "core_as_base.h"
#include "strexpr.h"
namespace coreas_str {

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
using is_const_pattern_t = std::enable_if_t<(N > 1) && N <= 17, int>;

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
    using uns = typename std::make_unsigned<K>::type;
    constexpr static const uint width = sizeof(size_t) / sizeof(uns);
    constexpr static const size_t value = _ascii_mask<uns, width>::value;
};

template<bool, typename O, typename K>
constexpr bool is_strtype_obj_v = std::is_same_v<typename O::symb_type, K>;

template<typename O, typename K>
constexpr bool is_strtype_obj_v<false, O, K> = false;

template<typename O, typename K>
constexpr bool is_strtype_v = is_strtype_obj_v<std::is_class_v<O>, O, K>;

template<typename O, typename K>
using is_strtype_t = std::enable_if_t<is_strtype_v<O, K>, int>;

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

template<typename K>
constexpr static K makeAsciiUpper(K k) {
    return isAsciiLower(k) ? k & ~0x20 : k;
}

enum TrimSides { trimLeft = 1, trimRight = 2, trimAll = 3 };
template<TrimSides S, typename K, uint N, bool withSpaces = false>
struct trimOperator;

template<typename K, uint N, uint L> struct expr_replaces;

template <typename T>
inline constexpr bool is_number_v = is_one_of_type<std::remove_cv_t<T>, int, short, long, long long, uint, unsigned short, unsigned long, unsigned long long>::value;

template<typename K, bool I, typename T>
struct need_sign {
    bool sign;
    need_sign(T& t) : sign(t < 0) { if (sign) t = -t; }
    void after(K*& ptr) { if (sign) *--ptr = '-'; }
};

template <typename K, typename... _Args>
using FmtString = std::_Basic_format_string<K, std::type_identity_t<_Args>...>;


template<typename K, typename T>
struct need_sign<K, false, T> {
    need_sign(T&) {}
    void after(K*&) {}
};

template<bool I, typename T>
struct negate_sign {
    bool canNegate() const { return false; }
    void set_negate() {}
    T ret(T& t) { return t; }
};

template<typename T>
struct negate_sign<true, T> {
    bool canNegate() const { return true; }
    bool n{ false };
    void set_negate() { n = true; }
    T ret(T& t) { return n ? -t : t; }
};

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
*/

template<typename K, typename StrRef, typename Impl>
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
    using StrPiece = StrRef;
    using traits = std::char_traits<K>;
    using uni = unicode_traits<K>;
    using uns_type = std::make_unsigned_t<K>;
    using my_type = Impl;
    // Пустой конструктор
    str_algs() = default;

    constexpr K* place(K* ptr) const noexcept {
        uint myLen = _len();
        if (myLen) {
            traits::copy(ptr, _str(), myLen);
            return ptr + myLen;
        }
        return ptr;
    }

    void copy_to(K* buffer, uint bufSize) {
        uint tlen = min(_len(), bufSize - 1);
        if (tlen)
            traits::copy(buffer, _str(), tlen);
        buffer[tlen] = 0;
    }

    // Чтобы быть источником строкового объекта
    constexpr operator StrPiece() const noexcept {
        return StrPiece{ _str(), _len() };
    }
    StrPiece to_str() const noexcept {
        return StrPiece{ _str(), _len() };
    }

    constexpr StrPiece operator () (int from, int len = 0) const noexcept {
        uint myLen = _len(), idxStart = from >= 0 ? from : myLen + from,
            idxEnd = (len > 0 ? from : myLen) + len;
        if (idxEnd > myLen)
            idxEnd = myLen;
        if (idxStart > idxEnd)
            idxStart = idxEnd;
        return StrPiece { _str() + idxStart, idxEnd - idxStart };
    }
    constexpr StrPiece mid(uint from, int len = -1) const noexcept {
        uint myLen = _len(), idxStart = from,
            idxEnd = len >= 0 ? from + len : myLen;
        if (idxEnd > myLen)
            idxEnd = myLen;
        if (idxStart > idxEnd)
            idxStart = idxEnd;
        return StrPiece{ _str() + idxStart, idxEnd - idxStart };
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
    int compare(StrPiece o) const {
        if (o.isEmpty())
            return _isEmpty() ? 0 : 1;
        uint myLen = _len(), checkLen = min(myLen, o.length());
        int cmp = checkLen ? traits::compare(_str(), o.c_str(), checkLen) : 0;
        return cmp == 0 ? (myLen > o.length() ? 1 : myLen == o.length() ? 0 : -1) : cmp;
    }

    uint find(StrPiece pattern, uint offset = 0) const noexcept {
        uint lenText = _len(), lenPattern = pattern.length();
        // Образец, не вмещающийся в строку и пустой образец не находим
        if (!lenPattern || offset + lenPattern > lenText)
            return str_pos::badIdx;
        lenPattern--;
        const K* text = _str(), * last = text + lenText - lenPattern, first = pattern.c_str()[0], *tail = pattern.c_str() + 1;
        for (const K* fnd = text + offset; ; ++fnd) {
            fnd = traits::find(fnd, last - fnd, first);
            if (!fnd)
                return str_pos::badIdx;
            if (!lenPattern || traits::compare(fnd + 1, tail, lenPattern) == 0)
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

    std::vector<uint> find_all(StrPiece pattern, uint offset = 0, uint maxCount = 0) const {
        if (!maxCount)
            maxCount--;
        std::vector<uint> result;
        while (maxCount-- > 0) {
            uint fnd = find(pattern, offset);
            if (fnd == str_pos::badIdx)
                break;
            result.push_back(fnd);
            offset = fnd + pattern.length();
        }
        return result;
    }

    uint find_reverse(K s) const noexcept {
        uint len = _len();
        while (len > 0) {
            if (_str()[--len] == s)
                return len;
        }
        return str_pos::badIdx;
    }

    bool isEqual(StrPiece other) const noexcept {
        uint myLen = _len();
        return other.length() == myLen && (myLen == 0 || traits::compare(_str(), other.c_str(), myLen) == 0);
    }

    bool operator == (StrPiece other) const noexcept {
        return isEqual(other);
    }

    bool operator != (StrPiece other) const noexcept {
        return !isEqual(other);
    }

    bool operator < (StrPiece other) const noexcept {
        return compare(other) < 0;
    }

    template<uint N>
    bool operator == (const K(&other)[N]) const noexcept {
        uint myLen = _len();
        return N - 1 == myLen && (myLen == 0 || traits::compare(_str(), other, N - 1) == 0);
    }

    template<uint N>
    bool operator != (const K(&other)[N]) const noexcept {
        uint myLen = _len();
        return !(N - 1 == myLen && (myLen == 0 || traits::compare(_str(), other, N - 1) == 0));
    }

    template<uint N>
    bool operator < (const K(&other)[N]) const noexcept {
        return compare({other}) < 0;
    }

    // Сравнение ascii строк без учёта регистра
    int compareia(StrPiece text) const noexcept {
        uint otherLen = text.length();
        if (!otherLen)
            return _isEmpty() ? 0 : 1;
        uint myLen = _len(), checkLen = min(myLen, otherLen);
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
        return myLen == otherLen ? 0 : myLen > otherLen ? 1 : -1;
    }

    bool isEqualia(StrPiece text) const noexcept {
        return text.length() == _len() && compareia(text) == 0;
    }

    bool isLessia(StrPiece text) const noexcept {
        return compareia(text) < 0;
    }

    int compareiu(StrPiece text) const noexcept {
        if (!text.length())
            return _isEmpty() ? 0 : 1;
        return uni::compareiu(_str(), _len(), text.c_str(), text.length());
    }

    bool isEqualiu(StrPiece text) const noexcept {
        return text.length() == _len() && compareiu(text) == 0;
    }

    bool isLessiu(StrPiece text) const noexcept {
        return compareiu(text) < 0;
    }

    my_type substr(int from, int len = 0) const { // индексация в code units
        return my_type{ d()(from, len) };
    }
    my_type str_mid(uint from, int len = -1) const { // индексация в code units
        return my_type{ d().mid(from, len) };
    }

    template<typename T, typename = std::enable_if_t<is_number_v<T> || is_one_of_type<T, int8_t, uint8_t>::value>>
    T toInt() const {
        const K* ptr = _str(), * end = ptr + _len();
        T res = 0;
        int state = 0;
        negate_sign<T(-1) < 0, T> negate;
        while (ptr < end) {
            K s = *ptr++;
            switch (state) {
            case 0:
                if (negate.canNegate() && s == '-') {
                    negate.set_negate();
                    state = 1;
                } else if (s == '+') {
                    state = 1;
                } else if (s == '0') {
                    state = 2;
                } else if (s > '0' && s <= '9') {
                    res = s - '0';
                    state = 3;
                } else if (s > ' ')
                    return 0;
                break;
            case 1: // Прочитан знак, теперь только цифры
                if (s == '0') {
                    state = 2;
                } else if (s > '0' && s <= '9') {
                    res = s - '0';
                    state = 3;
                } else if (s > ' ')
                    return 0;
                break;
            case 2: // Прочитан 0, теперь или x, или цифры до 8
                if (s == 'x' || s == 'X') {
                    state = 4;
                } else if (s >= '0' && s < '8') {
                    res = s - '0';
                    state = 5;
                } else
                    return 0;
                break;
            case 3: // Прочитана десятичная цифра, продолжаем считывать
                if (s >= '0' && s <= '9') {
                    res = res * 10 + s - '0';
                } else
                    return negate.ret(res);
                break;
            case 4: // Прочитан 0x
                if (s >= '0' && s <= '9') {
                    res = res * 16 + s - '0';
                } else if (s >= 'a' && s <= 'f') {
                    res = res * 16 + s - 'a' + 10;
                } else if (s >= 'A' && s <= 'F') {
                    res = res * 16 + s - 'A' + 10;
                } else
                    return negate.ret(res);
                break;
            case 5:
                if (s >= '0' && s < '8') {
                    res = res * 8 + s - '0';
                } else
                    return negate.ret(res);
                break;
            }
        }
        return negate.ret(res);
    }

    double toDouble() const {
        double result = 0.0;
        uint len = _len();
        if constexpr (sizeof(K) == 1) {
            if (len) {
                const K* ptr = _str();
                std::from_chars(ptr, ptr + len, result);
            }
        } else {
            const int copyLen = 255;
            K buf[copyLen + 1];
            const K* ptr;
            uint len = _len();
            if (len) {
                ptr = _str();
                if (ptr[len] != 0) {
                    while (len && *ptr <= ' ') {
                        len--;
                        *ptr++;
                    }
                    if (len) {
                        len = min(copyLen, len);
                        traits::copy(buf, ptr, len);
                        buf[len] = 0;
                        ptr = buf;
                    }
                }
                if (len) {
                    static _locale_t lc = _wcreate_locale(LC_NUMERIC, L"C");
                    result = _wcstod_l(ptr, nullptr, lc);
                }
            }
        }
        return result;
    }

    template<typename T, typename Op>
    T splitf(StrPiece delimeter, const Op& beforeFunc, uint offset = 0) const {
        uint mylen = _len(), lenDelimeter = delimeter.length();
        T results;
        StrPiece me{ _str(), _len() };
        for (;;) {
            uint beginOfDelim = find(delimeter, offset);
            if (beginOfDelim == str_pos::badIdx) {
                StrPiece last = beforeFunc({ me.c_str() + offset, me.length() - offset });
                if (last.isSame(me)) {
                    // Пробуем положить весь объект
                    results.emplace_back(d());
                } else
                    results.emplace_back(last);
                break;
            }
            results.emplace_back(beforeFunc({ me.c_str() + offset, beginOfDelim - offset }));
            offset = beginOfDelim + lenDelimeter;
        }
        return results;
    }

    // Разбиение строки на части
    template<typename T>
    T split(StrPiece delimeter, uint offset = 0) const {
        return splitf<T>(delimeter, [](StrPiece o) {return o; }, offset);
    }

    // Начинается ли эта строка с указанной подстроки
    bool isPrefixed(StrPiece prefix) const noexcept {
        if (!prefix.length() || _len() < prefix.length())
            return false;
        return 0 == prefix.compare(StrPiece{ _str(), prefix.length() });
    }
    // Начинается ли эта строка с указанной подстроки без учета ascii регистра
    bool isPrefixedia(StrPiece prefix) const noexcept {
        if (!prefix.length() || _len() < prefix.length())
            return false;
        return 0 == prefix.compareia(StrPiece{ _str(), prefix.length() });
    }
    // Начинается ли эта строка с указанной подстроки без учета unicode регистра
    bool isPrefixediu(StrPiece prefix) const noexcept {
        if (!prefix.length() || _len() < prefix.length())
            return false;
        return 0 == uni::compareiu(_str(), prefix.length(), prefix.c_str(), prefix.length());
    }

    // Является ли эта строка началом указанной строки
    bool isPrefixIn(StrPiece text) const noexcept {
        uint myLen = _len();
        if (myLen > text.length())
            return false;
        return !myLen || 0 == traits::compare(text.c_str(), _str(), myLen);
    }
    // Заканчивается ли строка указанной подстрокой
    bool isSuffixed(StrPiece suffix) {
        return suffix.length() <= _len() && (suffix.length() == 0 || (*this)(-int(suffix.length())).isEqual(suffix));
    }
    // Заканчивается ли строка указанной подстрокой без учета регистра ASCII
    bool isSuffixedia(StrPiece suffix) {
        return suffix.length() <= _len() && (suffix.length() == 0 || (*this)(-int(suffix.length())).isEqualia(suffix));
    }
    // Заканчивается ли строка указанной подстрокой без учета регистра UNICODE
    bool isSuffixediu(StrPiece suffix) {
        return suffix.length() <= _len() && (suffix.length() == 0 || (*this)(-int(suffix.length())).isEqualiu(suffix));
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
    R replace(StrPiece pattern, StrPiece repl, uint offset = 0, uint maxCount = 0) const {
        return R::replace_s(d(), pattern, repl, offset, maxCount);
    }

    template<uint N, uint L>
    expr_replaces<K, N - 1, L - 1> replace_init(const K(&pattern)[N], const K(&repl)[L]) {
        return expr_replaces<K, N - 1, L - 1>{d(), pattern, repl};
    }

    template<typename R = my_type>
    R& replaceTo(R& obj, StrPiece pattern, StrPiece repl, uint offset = 0, uint maxCount = 0) const {
        return R::replaceTo_s(obj, d(), pattern, repl, offset, maxCount);
    }

    template<typename From, is_strtype_t<From, K> = 0, typename Op>
    static my_type make_trim_op(const From& from, const Op& opTrim) {
        StrPiece sfrom = from, newPos = opTrim(sfrom);
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
    static my_type trim_static(const From& from, StrPiece pattern) {
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
    R trim(StrPiece pattern) const {
        return R::trim_static<TrimSides::trimAll, false>(d(), pattern);
    }
    template<typename R = my_type>
    R trim_l(StrPiece pattern) const {
        return R::trim_static<TrimSides::trimLeft, false>(d(), pattern);
    }
    template<typename R = my_type>
    R trim_r(StrPiece pattern) const {
        return R::trim_static<TrimSides::trimRight, false>(d(), pattern);
    }
    // Триминг по символам в литерале и пробелам
    template<typename R = my_type>
    R trim_s(StrPiece pattern) const {
        return R::trim_static<TrimSides::trimAll, true>(d(), pattern);
    }
    template<typename R = my_type>
    R trim_sl(StrPiece pattern) const {
        return R::trim_static<TrimSides::trimLeft, true>(d(), pattern);
    }
    template<typename R = my_type>
    R trim_sr(StrPiece pattern) const {
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
    constexpr SimpleStr(const K(&v)[N]) noexcept : str(v), len(N - 1) {}
    
    constexpr SimpleStr(const K* p, uint l) noexcept : str(p), len(l) {}

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
* Класс, заявляющий, что ссылается на нуль-терминированную строку.
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
    static my_type from_pointer(const K* p) noexcept {
        uint l = p ? static_cast<uint>(base::traits::length(p)) : 0;
        return my_type{ l ? p : empty_string, l };
    }
    constexpr static my_type null() noexcept { return my_type{ empty_string, 0 }; }
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

template<typename K>
static auto e_s(const K* ptr) {
    return SimpleStrNt<K>::from_pointer(ptr);
}

// Когда нужно вернуть из класса объект своего класса, инициализированный шаблонным параметром,
// но применять только публичные конструкторы
template<typename T, typename ...Args>
T return_public(Args&& ...other) {
    return T{ std::forward<Args>(other)... };
}

// Чтобы через ::assign нельзя было вызвать приватные конструкторы класса
// вызываем конструктор через эту прокладку
template<typename T, typename ...Args>
void init_public(T* pThis, Args&& ...other) {
    new (pThis) T(std::forward<Args>(other)...);
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
        return symbols.len != 0 && SimpleStr<K>::traits::find(symbols.str, symbols.len, s) != nullptr;
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
    constexpr CheckConstSymbolsTrim(const K(&value)[N + 1], std::index_sequence<Indexes...> dummy) :
        CheckConstSymbolsTrim(value[Indexes]...) {
    }

    constexpr CheckConstSymbolsTrim(const K(&s)[N + 1]) : CheckConstSymbolsTrim(s, typename std::make_index_sequence<N>{}){
    }

    template<uint Idx, std::enable_if_t<Idx == N, int> = 0>
    constexpr bool isInSymbols(K s) const noexcept {
        return false;
    }

    template<uint Idx, std::enable_if_t<Idx != N, int> = 0>
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
using SimpleTrim = trimOperator<S, K, -1, true>;
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
    template<typename O, typename = std::enable_if_t<!std::is_same_v<O, K>>>
    from_utf_convertable(SimpleStr<O> init) {
        using worker = utf_convert_selector<O, K>;
        Impl* d = static_cast<Impl*>(this);
        uint len = init.length();
        if (!len)
            d->createEmpty();
        else {
            uint maxSpace = worker::maxSpace(len);
            if (maxSpace <= 64) {
                K buf[64];
                maxSpace = worker::convert(init.c_str(), len, buf);
                K* ptr = d->init(maxSpace);
                std::char_traits<K>::copy(ptr, buf, maxSpace);
                ptr[maxSpace] = 0;
            } else
                d->setSize(worker::convert(init.c_str(), len, d->init(maxSpace)));
        }
    }
    template<typename O, typename I, typename = std::enable_if_t<!std::is_same_v<O, K>>>
    from_utf_convertable(const str_algs<O, SimpleStr<O>, I>& init) : from_utf_convertable(init.to_str()) {}
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
* K     - тип символов
* Impl  - тип наследника
*/
template<typename K, typename Impl>
class str_storeable {

    using my_type = Impl;
    using traits = std::char_traits<K>;
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
                    my_type res{ len };
                    K* pWrite = const_cast<K*>(res.c_str());
                    if (l) {
                        traits::copy(pWrite, f.c_str(), l);
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
                    my_type res{ len };
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

                    my_type res{ len };

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
            traits::copy(ptr, other.c_str(), other.length());
            ptr[other.length()] = 0;
        } else
            d().createEmpty();
    }
    // Конструктор повторения
    constexpr str_storeable(SimpleStr pattern, uint repeat) {
        uint l = pattern.length(), allLen = l * repeat;
        if (allLen) {
            K* ptr = d().init(allLen);
            for (uint i = 0; i < repeat; i++) {
                traits::copy(ptr, pattern.c_str(), l);
                ptr += l;
            }
            *ptr = 0;
        } else
            d().createEmpty();
    }

    // Конструктор из строкового литерала
    template<uint N>
    constexpr str_storeable(const K(&value)[N]) {
        if (N > 1) {
            K* ptr = d().init(N - 1);
            traits::copy(ptr, value, N - 1);
            ptr[N - 1] = 0;
        } else
            d().createEmpty();
    }
    
    // Конструктор из строкового выражения
    template<typename A, typename = std::enable_if_t<is_strexpr<A, K>>>
    constexpr str_storeable(const A& expr) {
        uint len = expr.length();
        if (len)
            *expr.place(d().init(len)) = 0;
        else
            d().createEmpty();
    }

    SimpleStrNt to_nts(uint from = 0) const {
        uint len = d().length();
        return from >= len ? SimpleStrNt::null() : SimpleStrNt{ d().c_str() + from, len - from };
    }

    operator SimpleStrNt() {
        return to_nts();
    }

    my_type& operator = (const SimpleStr& other) {
        return d().assign(other);
    }
    // Слияние контейнера строк
    template<typename T>
    static my_type join(const T& strings, SimpleStr delimeter, bool tail = false) {
        if (!strings.size())
            return my_type{};
        if (strings.size() == 1 && (!delimeter.length() || !tail))
            return my_type{ strings[0] };
        uint commonLen = 0;
        for (auto it = strings.begin(), e = strings.end(); it != e;) {
            commonLen += it->length();
            ++it;
            if (it != e || tail)
                commonLen += delimeter.length();
        }
        if (!commonLen)
            return my_type{};

        my_type res{ commonLen };	// выделяется память под все строки
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
    // ascii версия upper
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
        uint newSize = myLen + static_cast<int>((repl.length() - pattern.length()) * findes.size());
        if (!newSize)
            return my_type{};
        
        my_type res{ newSize };
        K* ptr = const_cast<K*>(res.c_str());
        uint from = 0;
        for (const auto& s: findes) {
            uint copyLen = s - from;
            if (copyLen) {
                traits::copy(ptr, f.c_str() + from, copyLen);
                ptr += copyLen;
            }
            if (repl.length()) {
                traits::copy(ptr, repl.c_str(), repl.len);
                ptr += repl.len;
            }
            from = s + pattern.len;
        }
        myLen -= from;
        if (myLen) {
            traits::copy(ptr, f.c_str() + from, myLen);
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
        return std::snprintf(buffer, count, format, std::forward<T>(args)...);
    }
    static int vsnprintf(u8symbol* buffer, size_t count, const u8symbol* format, va_list args) {
        return std::vsnprintf(buffer, count, format, args);
    }
};

template<>
struct printf_selector<u16symbol> {
    template<typename ...T>
    static int snprintf(u16symbol* buffer, size_t count, const u16symbol* format, T&& ... args) {
        return std::swprintf(buffer, count, format, std::forward<T>(args)...);
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
    using traits = std::char_traits<K>;
    using uni = unicode_traits<K>;
    using uns_type = std::make_unsigned_t<K>;

    template<typename Op>
    Impl& make_trim_op(const Op& op) {
        SimpleStr me = static_cast<SimpleStr>(d()), pos = op(me);
        if (me.length() != pos.length()) {
            if (me.c_str()!= pos.c_str())
                std::char_traits<K>::move(const_cast<K*>(me.c_str()), pos.c_str(), pos.length());
            d().setSize(pos.length());
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
            std::char_traits<K>::copy(ptr + size, o.c_str(), o.length());
        }
        return d();
    }
    template<typename A, typename = std::enable_if_t<is_strexpr<A, K>>>
    Impl& s_append(const A& expr) {
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
    template<typename A, typename = std::enable_if_t<is_strexpr<A, K>>>
    Impl& s_append_from(uint pos, const A& expr) {
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
    template<typename A, typename = std::enable_if_t<is_strexpr<A, K>>>
    Impl& s_insert(uint to, const A& expr) {
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
            if (size)
                std::char_traits<K>::move(ptr + o.length(), ptr, size);
            std::char_traits<K>::copy(ptr, o.c_str(), o.length());
        }
        return d();
    }
    template<typename A, typename = std::enable_if_t<is_strexpr<A, K>>>
    Impl& s_prepend(const A& expr) {
        uint len = expr.length();
        if (len) {
            uint size = _len();
            K* ptr = d().setSize(size + len);
            if (size)
                std::char_traits<K>::move(ptr + len, ptr, size);
            expr.place(ptr);
        }
        return d();
    }

    Impl& operator += (SimpleStr o) {
        return s_append(o);
    }
    template<typename A, typename = std::enable_if_t<is_strexpr<A, K>>>
    Impl& operator += (const A& expr) {
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
                    std::char_traits<K>::move(ptr + posWrite, ptr + offset, lenOfPiece);
                posWrite += lenOfPiece;
                if (repl.length()) {
                    std::char_traits<K>::copy(ptr + posWrite, repl.c_str(), repl.length());
                    posWrite += repl.length();
                }
                offset = idx + pattern.length();
            }
            uint tailLen = _len() - offset;
            if (posWrite < offset && tailLen)
                std::char_traits<K>::move(ptr + posWrite, ptr + offset, tailLen);
            d().setSize(posWrite + tailLen);
        } else {
            // Заменяем на более длинный кусок, длина текста увеличится, идём справа налево
            auto finded = d().find_all(pattern, offset, maxCount);
            if (finded.size()) {
                uint delta = repl.length() - pattern.length();
                uint allDelta = uint(delta * finded.size());
                uint endOfPiece = _len();
                K* ptr = d().setSize(endOfPiece + allDelta);
                for (uint i = uint(finded.size()); i--;) {
                    uint pos = finded[i] + pattern.length();
                    uint lenOfPiece = endOfPiece - pos;
                    std::char_traits<K>::move(ptr + pos + allDelta, ptr + pos, lenOfPiece);
                    std::char_traits<K>::copy(ptr + pos + allDelta - repl.length(), repl.c_str(), repl.length());
                    allDelta -= delta;
                    endOfPiece = finded[i];
                }
            }
        }
        return d();
    }

    template<typename From, typename = std::enable_if_t<is_strtype_v<From, K>>>
    static Impl& replaceTo_s(Impl& obj, const From& f, SimpleStr pattern, SimpleStr repl, uint offset = 0, uint maxCount = 0) {
        obj.~Impl();
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
    template<typename Op, std::enable_if_t<std::is_invocable_v<Op, K*, uint>, int> = 0>
    my_type& operator << (const Op& fillFunction) {
        return func_fill(0, fillFunction);
    }
    // Реализация добавления данных с проверкой на длину и перевыделением буфера в случае недостаточной длины.
    template<typename Op, std::enable_if_t<std::is_invocable_v<Op, K*, uint>, int> = 0>
    my_type& operator <<= (const Op& fillFunction) {
        return func_fill(_len(), fillFunction);
    }
    template<typename Op, std::enable_if_t<std::is_invocable_v<Op, my_type&>, int> = 0>
    my_type& operator << (const Op& fillFunction) {
        fillFunction(d());
        return d();
    }
    template<typename... T>
    my_type& s_format_to(uint from, const K* format, T&& ... args) {
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
            result = printf_selector<K>::snprintf(ptr + from, capacity + 1, format, std::forward<T>(args)...);
            if (result > (int)capacity) {
                ptr = from == 0 ? d().reserve(result) : d().setSize(from + result);
                result = printf_selector<K>::snprintf(ptr + from, result + 1, format, std::forward<T>(args)...);
            }
        } else {
            for (uint i = 0; ; i++) {
                result = printf_selector<K>::snprintf(ptr + from, capacity + 1, format, std::forward<T>(args)...);
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
        return s_format_to(0, format, std::forward<T>(args)...);
    }
    template<typename... T>
    my_type& s_append_f(const K* format, T&& ... args) {
        return s_format_to(_len(), format, std::forward<T>(args)...);
    }
    template<typename... T>
    my_type& s_tformat_to(uint from, const FmtString<K, T...>& format, T&& ... args) {
        uint size = _len();
        if (from > size)
            from = size;
        uint capacity = d().capacity();
        K* ptr = str();
        capacity -= from;

        auto result = std::format_to_n(ptr + from, capacity, format, std::forward<T>(args)...);
        if (result.size > (int)capacity) {
            ptr = from == 0 ? d().reserve((uint)result.size) : d().setSize(from + (uint)result.size);
            result = std::format_to_n(ptr + from, result.size, format, std::forward<T>(args)...);
        }
        d().setSize(from + (uint)result.size);
        return d();
    }
    template<typename... T>
    my_type& s_tformat(const FmtString<K, T...>& format, T&& ... args) {
        return s_tformat_to(0, format, std::forward<T>(args)...);
    }
    template<typename... T>
    my_type& s_append_tf(const FmtString<K, T...>& format, T&& ... args) {
        return s_tformat_to(_len(), format, std::forward<T>(args)...);
    }

    template<typename Op, typename ...Args>
    my_type& with(const Op& fillFunction, Args&& ... args) {
        fillFunction(d(), std::forward<Args>(args)...);
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

template<typename K>
struct SharedStringData {
    std::atomic_uint ref;	// Счетчик ссылок

    SharedStringData() {
        ref.store(1, std::memory_order_release);
    }
    K* str() const { return (K*)(this + 1); }
    void incr() { ref.fetch_add(1, std::memory_order_acq_rel); }
    void decr() {
        uint old = ref.fetch_sub(1, std::memory_order_acq_rel);
        if (old == 1)
            core_as_free(this);
    }
    static SharedStringData<K>* create(uint l) {
        return new (core_as_malloc(sizeof(SharedStringData<K>) + (l + 1) * sizeof(K))) SharedStringData;
    }
    static SharedStringData<K>* from_str(const K* p) {
        return (SharedStringData<K>*)p - 1;
    }
    K* place(K* p, uint len) {
        std::char_traits<K>::copy(p, str(), len);
        return p + len;
    }
};

/*
* Локальная строка. Хранит в себе длину строки, а за ней либо сами данные до N символов + нуль,
* либо если данные длиннее N, то размер выделенного буфера и указатель на данные.
* При этом, если планируется потом результат переместить в sstring, то для динамического буфера
* выделяется +n байтов, чтобы потом не двигать данные.
* Так как у класса несколько базовых классов, ms компилятор не применяет автоматом empty base optimization,
* и без явного указания - вставит в начало класса пустые байты, сдвинув поле size на 4 байта.
* Укажем ему явно
*/
template<typename K, uint N, bool forShared = false>
class empty_bases lstring :
    public str_algs<K, SimpleStr<K>, lstring<K, N, forShared>>,
    public str_storeable<K, lstring<K, N, forShared>>,
    public str_mutable<K, SimpleStr<K>, lstring<K, N, forShared>>,
    public from_utf_convertable<K, lstring<K, N, forShared>>
{
    constexpr static uint extra = forShared ? sizeof(SharedStringData<K>) : 0;
    using base_algs = str_algs<K, SimpleStr<K>, lstring<K, N, forShared>>;
    using base_store = str_storeable<K, lstring<K, N, forShared>>;
    using base_mutable = str_mutable<K, SimpleStr<K>, lstring<K, N, forShared>>;
    using base_utf = from_utf_convertable<K, lstring<K, N, forShared>>;
    using traits = std::char_traits<K>;

    friend base_store;
    friend base_mutable;
    friend base_utf;
    friend class sstring<K>;
    friend class sstring<K>;

    // Данные
    uint size;
    union {
        K local[N + 1];
        struct {
            uint bufSize;
            K* data;
        };
    };

    void createEmpty() {
        size = 0;
        bufSize = 0;
    }
    K* init(uint s) {
        size = s;
        if (size > N) {
            data = fromRealAddress(core_as_malloc((s + 1) * sizeof(K) + extra));
            bufSize = s;
        }
        return str();
    }
    // Методы для себя
    bool isBig() const noexcept {
        return size > N;
    }
    void dealloc() {
        if (isBig())
            core_as_free(toRealAddress(data));
    }

    static K* toRealAddress(void* ptr) {
        return reinterpret_cast<K*>(reinterpret_cast<u8symbol*>(ptr) - extra);
    }
    static K* fromRealAddress(void* ptr) {
        return reinterpret_cast<K*>(reinterpret_cast<u8symbol*>(ptr) + extra);
    }

public:
    using my_type = lstring<K, N, forShared>;
    using base_store::base_store;
    using base_utf::base_utf;
    using symb_type = K;

    constexpr static uint forSize(uint I) { return I - 1 - offsetof(my_type, local); }
    
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
                data = other.data;
                bufSize = other.bufSize;
            } else
                traits::copy(reserve(other.size), other.c_str(), other.size + 1);
            other.size = 0;
        }
    }
    // Копирование из строки другого размера
    template<uint L, bool S, typename = std::enable_if_t<L != N>>
    lstring(const lstring<K, L, S>& other) {
        uint len = other.length();
        if (len)
            traits::copy(reserve(len), other.c_str(), len + 1);
    }

    template<typename Op, std::enable_if_t<std::is_invocable_v<Op, K*, uint> || std::is_invocable_v<Op, my_type&>, int> = 0>
    lstring(const Op& op) : size(0) {
        this->operator<<(op);
    }

    // Присвоение с параметрами конструктора
    template<typename... T>
    my_type& assign(T&&... args) {
        this->~lstring();
        init_public<my_type>(this, std::forward<T>(args)...);
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
        return assign(std::move(other));
    }

    my_type& operator = (const SimpleStr<K>& other) {
        return assign(other);
    }
    template<typename A, std::enable_if_t<is_strexpr<A, K>, int> = 0>
    my_type& operator = (const A& expr) {
        return assign(expr);
    }

    uint length() const noexcept {
        return size;
    }
    const K* c_str() const noexcept {
        return isBig() ? data : local;
    }
    K* str() noexcept {
        return isBig() ? data : local;
    }
    bool isEmpty() const noexcept {
        return size == 0;
    }

    uint capacity() const noexcept {
        return isBig() ? bufSize : N;
    }

    // Выделить буфер, достаточный для размещения newSize символов плюс завершающий ноль
    // Содержимое буфера неопределено, и не гарантируется сохранение старого содержимого
    K* reserve(uint newSize) {
        if (newSize != size) {
            if (isBig()) {
                // У нас есть динамический буфер
                if (newSize <= N) // и он теперь не нужен
                    dealloc();
                else if (newSize > bufSize) {
                    // он недостаточен
                    data = fromRealAddress(core_as_realloc(toRealAddress(data), (newSize + 1) * sizeof(K) + extra));
                    bufSize = newSize;
                }
            } else {
                // У нас локальный буфер
                if (newSize > N) {
                    // А нужен динамический
                    data = fromRealAddress(core_as_malloc((newSize + 1) * sizeof(K) + extra));
                    bufSize = newSize;
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
                    K* dynBuffer = data; // Копирование перезатрет данные
                    if (newSize)
                        traits::copy(local, dynBuffer, newSize);
                    core_as_free(toRealAddress(dynBuffer));
                } else if (newSize > bufSize) {
                    // динамический буфер недостаточный по длине. Расширим его. При этом для строго динамических строк с запасом в полтора раза
                    data = fromRealAddress(core_as_realloc(toRealAddress(data), (newSize + 1 + (N == 0 ? newSize / 2 : 0)) * sizeof(K) + extra));
                    bufSize = newSize + (N == 0 ? newSize / 2 : 0);
                }
            } else {
                // У нас локальный буфер
                if (newSize > N) {
                    // А нужен динамический
                    K* dynBuffer = fromRealAddress(core_as_malloc((newSize + 1) * sizeof(K) + extra));
                    if (size)
                        traits::copy(dynBuffer, local, size + 1);
                    data = dynBuffer;
                    bufSize = newSize;
                }
            }
            size = newSize;
        }
        K* res = str();
        res[newSize] = 0;
        return res;
    }
    void defineSize() {
        K* start = str();
        for (uint i = 0, ie = capacity(); i < ie; i++) {
            if (start[i] == 0) {
                size = i;
                return;
            }
        }
        size = capacity();
        start[size] = 0;
    }
};

template<uint N> using lstringa = lstring<u8symbol, N>;
template<uint N> using lstringw = lstring<u16symbol, N>;
template<uint N> using lstringsa = lstring<u8symbol, N, true>;
template<uint N> using lstringsw = lstring<u16symbol, N, true>;

template<typename K, uint N> K getLiteralType(const K(&)[N]) { return {}; };

template<uint L>
constexpr const uint _local_count = 7;
template<>
constexpr const uint _local_count<1> = 23;
template<>
constexpr const uint _local_count<2> = 15;

template<typename T>
constexpr const uint local_count = _local_count<sizeof(T)>;

/*
* Класс с small string optimization плюс разделяемый буфер строки. Иммутабельный.
* Размеры:
* для u8symbol  - 24 байта, хранит строки до 23 символов + 0
* для u16symbol - 32 байта, хранит строки до 15 символов + 0
* для u32symbol - 32 байта, хранит строки до 7 символов + 0
*/
template<typename K>
class empty_bases sstring :
    public str_algs<K, SimpleStr<K>, sstring<K>>,
    public str_storeable<K, sstring<K>>,
    public from_utf_convertable<K, sstring<K>> {
public:
    using symb_type = K;
    using uns_type = std::make_unsigned_t<K>;
    using my_type = sstring<K>;
    static COREAS_API const my_type strnull;
protected:
    using base_algs = str_algs<K, SimpleStr<K>, my_type>;
    using base_store = str_storeable<K, my_type>;
    using base_utf = from_utf_convertable<K, my_type>;

    friend base_store;
    friend base_utf;
    friend sstring<K>;

    enum { LocalCount = local_count<K> };
    enum Types { tLocal, tConstant, tShared };

    union {
        struct {
            K buf[LocalCount];        // Локальный буфер строки
            uns_type localRemain : sizeof(uns_type) * 8 - 2;
            uns_type type : 2;
        };
        struct {
            union {
                const K* cstr; // Указатель на конcтантную строку
                const K* sstr; // Указатель на строку, перед которой лежит SharedStringData
            };
            uint bigLen;   // Длина не локальной строки.
        };
    };

    void createEmpty() {
        type = tLocal;
        localRemain = LocalCount;
        buf[0] = 0;
    }
    K* init(uint s) {
        if (s > LocalCount) {
            type = tShared;
            localRemain = 0;
            bigLen = s;
            sstr = SharedStringData<K>::create(s)->str();
            return (K*)sstr;
        } else {
            type = tLocal;
            localRemain = LocalCount - s;
            return buf;
        }
    }

    K* setSize(uint newSize) {
        // вызывается при создании строки при необходимости изменить размер
        // других ссылок на shared bufer нет
        uint size = length();
        if (newSize != size) {
            if (type == tConstant) {
                bigLen = newSize;
            } else {
                if (newSize <= LocalCount) {
                    if (type == tShared) {
                        SharedStringData<K>* str_buf = SharedStringData<K>::from_str(sstr);
                        traits::copy(buf, sstr, newSize);
                        str_buf->decr();
                    }
                    type = tLocal;
                    localRemain = LocalCount - newSize;
                } else {
                    if (type == tShared) {
                        if (newSize > size || (newSize > 32 && newSize < size * 3 / 4)) // строка сильно изменилась
                            sstr = reinterpret_cast<SharedStringData<K>*>(core_as_realloc(SharedStringData<K>::from_str(sstr), (newSize + 1) * sizeof(K) + sizeof(SharedStringData<K>)))->str();
                    } else if (type == tLocal) {
                        K* dynBuffer = SharedStringData<K>::create(newSize)->str();
                        if (size)
                            traits::copy(dynBuffer, buf, size);
                        sstr = dynBuffer;
                        type = tShared;
                        localRemain = 0;
                    }
                    bigLen = newSize;
                }
            }
        }
        K* str = type == tLocal ? buf : (K*)sstr;
        str[newSize] = 0;
        return str;
    }
    using traits = std::char_traits<K>;
    using uni = unicode_traits<K>;

public:
    using base_store::base_store;
    using base_utf::base_utf;

    sstring() = default;
    ~sstring() {
        if (type == tShared)
            SharedStringData<K>::from_str(sstr)->decr();
    }
    sstring(const my_type& other) noexcept {
        memcpy(this, &other, sizeof(other));
        if (type == tShared)
            SharedStringData<K>::from_str(sstr)->incr();
    }
    sstring(my_type&& other) noexcept {
        memcpy(this, &other, sizeof(other));
        other.createEmpty();
    }
    sstring(K pad, uint count) {
        if (count) {
            K* str = init(count);
            traits::assign(str, count, pad);
            str[count] = 0;
        } else
            createEmpty();
    }
    // Конструктор перемещения из локальной строки
    template<uint N>
    sstring(lstring<K, N, true>&& src) {
        uint size = src.length();
        if (size) {
            if (size > N) {
                // Там динамический буфер, выделенный с запасом для SharedStringData.
                K* str = src.str();
                if (size > LocalCount) {
                    // Просто присвоим его себе.
                    sstr = str;
                    bigLen = size;
                    type = tShared;
                    localRemain = 0;
                    new (SharedStringData<K>::from_str(str)) SharedStringData<K>;
                } else {
                    // Скопируем локально
                    type = tLocal;
                    localRemain = LocalCount - size;
                    traits::copy(buf, str, size + 1);
                    // Освободим тот буфер, у локальной строки буфер не делится с другими
                    core_as_free(SharedStringData<K>::from_str(str));
                }
            } else {
                // Копируем из локального буфера
                K* str = init(src.size);
                traits::copy(str, src.c_str(), size);
                str[size] = 0;
            }
            src.size = 0;
        } else
            createEmpty();
    }
    // Инициализация из строкового литерала
    template<uint N>
    sstring(const K(&s)[N]) {
        if constexpr (N == 1) {
            createEmpty();
        } else {
            type = tConstant;
            localRemain = 0;
            cstr = s;
            bigLen = N - 1;
        }
    }

    my_type& operator = (const my_type& other) noexcept {
        if (&other != this) {
            this->~sstring();
            new (this) my_type(other);
        }
        return *this;
    }
    my_type& operator = (my_type&& other) noexcept {
        if (&other != this) {
            this->~sstring();
            new (this) my_type(std::move(other));
        }
        return *this;
    }
    template<uint N>
    my_type& operator = (lstring<K, N, true>&& other) {
        this->~sstring();
        new (this) my_type(std::move(other));
        return *this;
    }
    my_type& operator = (SimpleStr<K> other) {
        this->~sstring();
        new (this) my_type(other);
        return *this;
    }

    template<uint N>
    my_type& operator = (const K(&value)[N]) {
        this->~sstring();
        new(this)my_type(value);
        return *this;
    }

    template<typename A, std::enable_if_t<is_strexpr<A, K>, int> = 0>
    my_type& operator = (const A& expr) {
        this->~sstring();
        new(this)my_type(expr);
        return *this;
    }

    my_type& makeEmpty() {
        this->~sstring();
        createEmpty();
        return *this;
    }
    operator const K* () const noexcept {
        return c_str();
    }
    const K* c_str() const noexcept {
        return type == tLocal ? buf : cstr;
    }
    uint length() const noexcept {
        return type == tLocal ? LocalCount - localRemain : bigLen;
    }
    bool isEmpty() const noexcept {
        return length() == 0;
    }
    // Форматирование строки.
    template<typename ...T>
    static my_type tformat(const FmtString<K, T...>& fmtString, T&& ... args) {
        return my_type{ std::move(lstring<K, 300, true>{}.s_tformat(fmtString, std::forward<T>(args)...)) };
    }
};

template<typename K>
struct std::formatter<SimpleStr<K>, K> : std::formatter<std::basic_string_view<K>, K> {
    // Define format() by calling the base class implementation with the wrapped value
    template<typename FormatContext>
    auto format(coreas_str::SimpleStr<K> t, FormatContext& fc) const {
        return std::formatter<std::basic_string_view<K>, K>::format({t.str, t.len}, fc);
    }
};

#define SU(par) ([]() -> const sstring<decltype(getLiteralType(par))>& {\
    static sstring<decltype(getLiteralType(par))> s(par);return s;}())

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

template<typename K, typename T>
constexpr uint fromInt(K* bufEnd, T val) {
    using wider_t = typename digits_selector<sizeof(K)>::wider_type;
    if (val) {
        need_sign<K, T(-1) < 0, T> sign(val);
        K* itr = bufEnd;
        while (val) {
            *--itr = static_cast<K>('0' + val % 10);
            val /= 10;
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
    
    enum {bufSize = 24 };
    mutable T value;
    mutable K buf[bufSize];

    expr_num(T t) : value(t) {}
    expr_num(expr_num<K, T>&& t) : value(t.value) {}
    
    uint length() const noexcept {
        value = (uint)fromInt(buf + bufSize, value);
        return (uint)value;
    }
    K* place(K* ptr) const noexcept {
        std::char_traits<K>::copy(ptr, buf + bufSize - (uint)value, (uint)value);
        return ptr + (uint)value;
    }
};

template<typename A, typename T, std::enable_if_t<is_strexpr_v<A> && is_number_v<T>, int> = 0>
constexpr static auto operator & (const A& a, T s) {
    return strexprjoin_c<A, expr_num<typename A::symb_type, T>>{ a, {s} };
}

template<typename K, typename T>
constexpr static auto e_num(T t) {
    return expr_num<K, T>{t};
}

template<typename K>
SimpleStrNt<K> select_str(SimpleStrNt<u8symbol> s8, SimpleStrNt<u16symbol> s16, SimpleStrNt<u32symbol> s32) {
    if constexpr (sizeof(K) == 1)
        return s8;
    else if constexpr (sizeof(K) == 2)
        return s16;
    else
        return s32;
}

#define uni_string(K, p) select_str<K>(p, L##p, U##p)

template<typename K>
struct expr_real {
    using symb_type = K;
    mutable K buf[40];
    mutable uint l;
    double v;
    expr_real(double d) : v(d) {}
    expr_real(float d) : v(d) {}

    uint length() const noexcept {
        printf_selector<K>::snprintf(buf, 40, uni_string(K, "%.17g"), v);
        l = (uint)std::char_traits<K>::length(buf);
        return l;
    }
    K* place(K* ptr) const noexcept {
        std::char_traits<K>::copy(ptr, buf, l);
        return ptr + l;
    }
};

template<typename A, std::enable_if_t<is_strexpr_v<A>, int> = 0>
constexpr static auto operator & (const A& a, double s) {
    return strexprjoin_c<A, expr_real<typename A::symb_type>>{ a, {s} };
}

template<typename A, std::enable_if_t<is_strexpr_v<A>, int> = 0>
constexpr static auto operator & (const A& a, float s) {
    return strexprjoin_c<A, expr_real<typename A::symb_type>>{ a, {s} };
}

template<typename K>
constexpr static auto e_real(double t) {
    return expr_real<K>{t};
}


/*
* Для создания строковых конкатенаций с векторами и списками, сджойненными константным разделителем
* K - тип символов строки
* T - тип контейнера строк (vector, list)
* I - длина разделителя в символах
* tail - добавлять разделитель после последнего элемента контейнера.
*        Если контейнер пустой, разделитель в любом случае не добавляется
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
        if (!s.empty()) {
            for (auto t = s.begin(), e = s.end(); ;) {
                uint copyLen = t->length();
                if (copyLen) {
                    std::char_traits<K>::copy(ptr, t->c_str(), copyLen);
                    ptr += copyLen;
                }
                ++t;
                if (t == e) {
                    // Последний элемент контейнера
                    if constexpr (I > 0 && tail) {
                        std::char_traits<K>::copy(ptr, delim, I);
                        ptr += I;
                    }
                    break;
                }
                if constexpr (I > 0) {
                    std::char_traits<K>::copy(ptr, delim, I);
                    ptr += I;
                }
            }
        }
        return ptr;
    }
};

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
    mutable std::vector<uint> positions;

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
                std::char_traits<K>::copy(ptr, what.c_str() + from, copyLen);
                ptr += copyLen;
            }
            if constexpr (L > 0) {
                std::char_traits<K>::copy(ptr, repl, L);
                ptr += L;
            }
            if constexpr (N > 0)
                from = k + N;
        }
        uint tailLen = what.length() - from;
        if (tailLen) {
            std::char_traits<K>::copy(ptr, what.c_str() + from, tailLen);
            ptr += tailLen;
        }
        return ptr;
    }
};

template<typename K, uint N, uint L, std::enable_if_t<(N > 1), int> = 0>
constexpr static auto e_repl(SimpleStr<K> w, const K(&p)[N], const K(&r)[L]) {
    return expr_replaces<K, N - 1, L - 1>{w, p, r};
}

template<typename A, typename B, std::enable_if_t<is_strexpr_v<A> && is_strexpr_v<B> && std::is_same_v<typename A::symb_type, typename B::symb_type>, int> = 0>
struct expr_choice {
    using symb_type = typename A::symb_type;
    using my_type = expr_choice<A, B>;
    const A& a;
    const B& b;
    bool choice;

    constexpr uint length() const noexcept {
        return choice ? a.length() : b.length();
    }
    constexpr symb_type* place(symb_type* ptr) const noexcept {
        return choice ? a.place(ptr) : b.place(ptr);
    }
};

template<typename A, typename B>
constexpr static auto e_choice(bool c, const A& a, const B& b) {
    return expr_choice<A, B> {a, b, c};
}

template<typename K>
struct StoreType {
    SimpleStr<K> str;
    size_t hash;
    char node[sizeof(sstring<K>)];

    const SimpleStrNt<K>& to_nt() const noexcept {
        return static_cast<const SimpleStrNt<K>&>(str);
    }
    const sstring<K>& to_str() const noexcept {
        return *reinterpret_cast<const sstring<K>*>(node);
    }
};

using HashKeyA = StoreType<u8symbol>;
using HashKeyW = StoreType<u16symbol>;
using HashKeyU = StoreType<u32symbol>;

template<typename K>
constexpr static size_t fnv_hash(const K* ptr, uint l) {
    size_t h = std::_FNV_offset_basis;
    while (l--)
        h = (h ^ *ptr++) * std::_FNV_prime;
    return h;
};

template<typename K>
constexpr static size_t fnv_hash_ia(const K* ptr, uint l) {
    size_t h = std::_FNV_offset_basis;
    while (l--) {
        K s = *ptr++;
        h = (h ^ (s >= 'A' && s <= 'Z' ? s | 0x20 : s)) * std::_FNV_prime;
    }
    return h;
};

/*
* Вычисление хешей строк во время компиляции.
*/

template<typename K, uint N>
constexpr static size_t fnv_hash(const K(&value)[N]) {
    size_t h = std::_FNV_offset_basis;
    for (uint i = 0; i < N - 1; i++)
        h = (h ^ value[i]) * std::_FNV_prime;
    return h;
};

template<typename K, uint N>
constexpr static size_t fnv_hash_ia(const K(&value)[N]) {
    size_t h = std::_FNV_offset_basis;
    for (uint i = 0; i < N - 1; i++)
        h = (h ^ (value[i] >= 'A' && value[i] <= 'Z' ? value[i] | 0x20 : value[i])) * std::_FNV_prime;
    return h;
};

/*
 * Данный шаблон используется для форсирования вычисления хэша именно во время компиляции,
 * так как если просто использовать вызов constexpr функции, то начиная с определенной
 * длинны строки(компилятор VC2019 с 32 символов) вставляет в код заинлайненный вызов
 * функции.
 */

template<size_t I> struct hhh { constexpr static const size_t val = I; };

#define strh(Val) StoreType<decltype(getLiteralType(Val))> {{Val, (sizeof(Val) / sizeof(Val[0])) - 1}, hhh<fnv_hash(Val)>::val}
#define strhia(Val) StoreType<decltype(getLiteralType(Val))> {{Val, (sizeof(Val) / sizeof(Val[0])) - 1}, hhh<fnv_hash_ia(Val)>::val}

/*
* Контейнер для более эффективного поиска по строковым ключам.
* Как unordered_map, но чуть лучше. В качестве ключей хранит SimpleStr вместе с посчитанным хешем.
* Чтобы SimpleStr было на что ссылатся, строковые значения ключей кладёт в список,
* с ключом запоминает позицию в списке. При удалении ключа удаляет и из списка.
* Позволяет использовать для поиска строковые литералы, не создавая для них объекта sstring.
* Начиная с С++20 в unordered_map появилась возможность для гетерогенного поиска по ключу с типом,
* отличным от типа хранящегося ключа. Однако удаление по прежнему только по типу ключа,
* что сводит на нет улучшения.
* Да и хэш тоже не хранит, каждый раз вычисляя заново.
*/
template<typename K, typename T, typename H, typename E>
class hashStrMap : public std::unordered_map<StoreType<K>, T, H, E> {
protected:
    using InStore = StoreType<K>;
public:
    using my_type = hashStrMap<K, T, H, E>;
    using hash_t = std::unordered_map<InStore, T, H, E>;
    using hasher = H;
    ~hashStrMap() {
        for (auto& k : *this)
            ((sstring<K>*)k.first.node)->~sstring();
    }
    hashStrMap() = default;
    hashStrMap(const my_type&) = default;
    hashStrMap(my_type&& o) = default;
    my_type& operator = (const my_type&) = default;
    my_type& operator = (my_type&&) = default;

    hashStrMap(std::initializer_list<std::pair<StoreType<K>, T>> init) {
        for (const auto& e : init)
            emplace(e.first, e.second);
    }
    // При входе хэш должен быть уже посчитан
    template<typename...ValArgs>
    auto emplace(const StoreType<K>& key, ValArgs&&... args) {
        auto it = hash_t::try_emplace(key, std::forward<ValArgs>(args)...);
        if (it.second) {
            InStore& stored = const_cast<InStore&>(it.first->first);
            new (stored.node) sstring<K>(key.str);
            stored.str.str = stored.to_str().c_str();
        }
        return it;
    }
    template<typename...ValArgs>
    auto emplace(const SimpleStr<K>& key, ValArgs&&... args) {
        return emplace(StoreType<K>{ key, H{}(key) }, std::forward<ValArgs>(args)...);
    }
    template<typename...ValArgs>
    auto emplace(const sstring<K>& key, ValArgs&&... args) {
        auto it = hash_t::try_emplace(StoreType<K>{ key.to_str(), H{}(key)}, std::forward<ValArgs>(args)...);
        if (it.second) {
            InStore& stored = const_cast<InStore&>(it.first->first);
            new (stored.node) sstring<K>(key);
            stored.str.str = stored.to_str().c_str();
        }
        return it;
    }

    template<typename...ValArgs>
    auto emplace_or_assign(const StoreType<K>& key, ValArgs&&... args) {
        auto it = emplace(key, std::forward<ValArgs>(args)...);
        if (!it.second) {
            it.first->second.T::~T();
            new (&it.first->second) T(std::forward<ValArgs>(args)...);
        }
        return it;
    }
    template<typename...ValArgs>
    auto emplace_or_assign(const SimpleStr<K>& key, ValArgs&&... args) {
        return emplace_or_assign(StoreType<K>{ key, H{}(key) }, std::forward<ValArgs>(args)...);
    }
    template<typename...ValArgs>
    auto emplace_or_assign(const sstring<K>& key, ValArgs&&... args) {
        auto it = emplace(key, std::forward<ValArgs>(args)...);
        if (!it.second) {
            it.first->second.T::~T();
            new (&it.first->second) T(std::forward<ValArgs>(args)...);
        }
        return it;
    }
    auto find(const StoreType<K>& key) const {
        return hash_t::find(key);
    }
    auto find(const SimpleStr<K>& key) const {
        return find(StoreType<K>{ key, H{}(key) });
    }
    auto find(const StoreType<K>& key) {
        return hash_t::find(key);
    }
    auto find(const SimpleStr<K>& key) {
        return find(StoreType<K>{ key, H{}(key) });
    }
    auto erase(const StoreType<K>& key) {
        auto it = hash_t::find(key);
        if (it != hash_t::end()) {
            ((sstring<K>*)it->first.node)->~sstring();
            hash_t::erase(it);
            return 1;
        }
        return 0;
    }
    auto erase(const SimpleStr<K>& key) {
        return erase(StoreType<K> { key, H{}(key) });
    }
    /*hash_t::iterator begin() {
        return hash_t::begin();
    }
    hash_t::iterator end() {
        return hash_t::end();
    }
    auto begin() const {
        return hash_t::begin();
    }
    auto end() const {
        return hash_t::end();
    }*/
    bool lookup(const K* txt, T& val) const {
        auto it = find(e_s(txt));
        if (it != hash_t::end()) {
            val = it->second;
            return true;
        }
        return false;
    }
    bool lookup(SimpleStr<K> txt, T& val) const {
        auto it = find(txt);
        if (it != hash_t::end()) {
            val = it->second;
            return true;
        }
        return false;
    }
    /*size_t size() const {
        return hash_t::size();
    }*/
    void clear() {
        for (auto& k : *this)
            ((sstring<K>*)k.first.node)->~sstring();
        hash_t::clear();
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
        return fnv_hash_ia(_Keyval.c_str(), _Keyval.length());
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

/*
* Для построения длинных динамических строк конкатенацией мелких кусочков.
* Выделяет по мере надобности отдельные блоки заданного размера (или кратного ему для больших вставок),
* чтобы избежать релокации длинных строк.
* После построения можно слить в одну строку
*/
template<typename K>
class chunked_string_concatenator {
    std::vector<std::pair<std::unique_ptr<K>, uint>> chunks;// блоки и длина данных в них
    K* write{ nullptr }; // Текущая позиция записи
    uint len{ 0 };// Общая длина
    uint remain{ 0 }; // Сколько осталось места в текущем блоке
    uint align{ 1024 };
public:
    using my_type = chunked_string_concatenator<K>;
    using symb_type = K;
    chunked_string_concatenator() = default;
    chunked_string_concatenator(uint a) : align(a) {};
    chunked_string_concatenator(const my_type&) = delete;
    chunked_string_concatenator(my_type&& other) :
        chunks(std::move(other.chunks)), write(other.write), len(other.len), remain(other.remain), align(other.align)  {
        other.len = other.remain = 0;
        other.write = nullptr;
    }
    my_type& operator = (my_type&& other) {
        if (&other != this) {
            this->~my_type();
            new (this) my_type(std::move(other));
        }
        return *this;
    }
    // Добавление порции данных
    my_type& operator << (SimpleStr<K> data) {
        if (data.len) {
            len += data.len;
            if (data.len <= remain) {
                // Добавляемые данные влезают в выделенный блок, просто скопируем их
                std::char_traits<K>::copy(write, data.str, data.len);
                write += data.len;// Сдвинем позицию  записи
                chunks.back().second += data.len;// Увеличим длину хранимых в блоке данных
                remain -= data.len;// Уменьшим остаток места в блоке
            } else {
                // Не влезают
                if (remain) {
                    // Сначала запишем сколько влезет
                    std::char_traits<K>::copy(write, data.str, remain);
                    data.len -= remain;
                    data.str += remain;
                    chunks.back().second += remain; // Увеличим длину хранимых в блоке данных
                }
                // Выделим новый блок и впишем в него данные
                uint blockSize = (data.len + align - 1) / align * align;  // Рассчитаем размер блока, кратного заданному выравниванию
                chunks.emplace_back(new K[blockSize], data.len);
                write = chunks.back().first.get();
                std::char_traits<K>::copy(write, data.str, data.len);
                write += data.len;
                remain = blockSize - data.len;
            }
        }
        return *this;
    }
    template<typename A, std::enable_if_t<is_strexpr<A, K>, int> = 0>
    my_type& operator << (const A& expr) {
        uint l = expr.length();
        if (l) {
            if (l < remain) {
                write = expr.place(write);
                chunks.back().second += l;
                len += l;
                remain -= l;
            } else {
                std::unique_ptr<K> store(new K[l]);
                expr.place(store.get());
                return operator<<({ store.get(), l });
            }
        }
        return *this;
    }
    constexpr uint length() const noexcept { return len; }
    constexpr K* place(K* p) const noexcept {
        for (const auto& block: chunks) {
            std::char_traits<K>::copy(p, block.first.get(), block.second);
            p += block.second;
        }
        return p;
    }

    template<typename Op>
    void out(Op&& o) const {
        for (const auto& block: chunks)
            o(block.first.get(), block.second);
    }

    bool isContinuous() const {
        if (chunks.size()) {
            const char* ptr = chunks.front().first.get();
            for (const auto& chunk : chunks) {
                if (chunk.first.get() != ptr)
                    return false;
                ptr += chunk.second;
            }
        }
        return true;
    }
    const K* begin() const {
        return chunks.size() ? chunks.front().first.get() : nullptr;
    }

    void clear() {
        uint a = align;
        this->~chunked_string_concatenator<K>();
        new(this)chunked_string_concatenator<K>(a);
    }
    struct portionStore {
        typename decltype(chunks)::const_iterator it, end;
        uint ptr;

        bool isEnd() {
            return it == end;
        }
        uint store(K* buffer, uint size) {
            uint writed = 0;
            while (size && !isEnd()) {
                uint remain = it->second - ptr;
                uint write = min(size, remain);
                std::char_traits<K>::copy(buffer, it->first.get() + ptr, write);
                writed += write;
                remain -= write;
                size -= write;
                if (!remain) {
                    ++it;
                    ptr = 0;
                } else
                    ptr += write;
            }
            return writed;
        }
    };
    portionStore getPortion() const { return { chunks.begin(), chunks.end(), 0 }; }
};

inline static char hexDigit(int t) { return t < 10 ? '0' + t : 'a' + t - 10; }

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

constexpr const SimpleStrNt<u8symbol> utf8_bom{ "\xEF\xBB\xBF", 3};

}// namespace coreas_str {
