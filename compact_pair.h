#pragma once

#include <algorithm>

template<typename T11, typename T12, typename T21, typename T22>
class compact_pair
{
public:
    enum type { first, second };
    template<typename T>
    static constexpr std::size_t type_no = 11;
    template<>
    static constexpr std::size_t type_no<T11> = 11;
    template<>
    static constexpr std::size_t type_no<T12> = 12;
    template<>
    static constexpr std::size_t type_no<T21> = 21;
    template<>
    static constexpr std::size_t type_no<T22> = 22;
public:
    constexpr compact_pair(T11 val1, T12 val2) {
        new(_data) T11{val1};
        new(_data + _offset<T11>()) T12{val2};
    }
    constexpr compact_pair(T21 val1, T22 val2) {
        new(_data) T21{val1};
        new(_data + _offset<T21>()) T22{val2};
        to_second();
    }
    constexpr type type() const noexcept {
        return is_second() ? second : first;
    }
    template<typename T, std::size_t N = type_no<T>>
    constexpr T get() const noexcept {
        return {};
    }
    template<>
    constexpr T11 get<T11, type_no<T11>>() const noexcept {
        if (is_second()) return {};
        return *reinterpret_cast<const T11*>(_data);
    }
    template<>
    constexpr T12 get<T12, type_no<T12>>() const noexcept {
        if (is_second()) return {};
        return *reinterpret_cast<const T12*>(_data + _offset<T11>());
    }
    template<>
    constexpr T21 get<T21, type_no<T21>>() const noexcept {
        if (!is_second()) return {};
        return *reinterpret_cast<const T21*>(_data);
    }
    template<>
    constexpr T22 get<T22, type_no<T22>>() const noexcept {
        if (!is_second()) return {};
        return *reinterpret_cast<const T22*>(_data + _offset<T21>());
    }
private:
    inline constexpr bool is_second() const noexcept {
        return _data[0] & 0x80;
    }

    constexpr void to_second() noexcept {
        _data[0] &= 0x80;
    }

    template<typename T>
    static constexpr std::size_t _offset() noexcept {
        return sizeof(T);
    }

    static constexpr std::size_t _size() noexcept {
        return std::max(sizeof(T11) + sizeof(T12), sizeof(T21) + sizeof(T22));
    }

    unsigned char _data[_size()];
};
