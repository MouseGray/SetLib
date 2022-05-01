#ifndef SETLIB_H
#define SETLIB_H

#include <array>
#include <sstream>

namespace ltl {

template <int S = 20>
class set
{
public:
    set() { _data.fill(0); }

    void insert(int val) noexcept
    {
        _data[val >> 3] |= 1 << (val & 0x7);
    }

    void insert(int sval, int eval) noexcept
    {
        auto scell = sval >> 3;
        auto ecell = eval >> 3;
        if (scell == ecell) {
            _data[scell] |= ((0xFF << (sval & 0x7)) & (~(0xFF << (eval & 0x7))));
            return;
        }
        _data[scell] |= 0xFF << (sval & 0x7);
        for (auto it = scell + 1; it < ecell; ++it) _data[it] |= 0xFF;
        _data[ecell] |= ~(0xFF << (eval & 0x7));
    }

    void erase(int val) noexcept
    {
        _data[val >> 3] &= ~(1 << (val & 0x7));
    }

    void erase(int sval, int eval) noexcept
    {
        auto scell = sval >> 3;
        auto ecell = eval >> 3;
        _data[scell] &= ~(0xFF << (sval & 0x7));
        for (auto it = scell + 1; it < ecell; ++it) _data[it] &= 0x00;
        _data[ecell] &= 0xFF << (eval & 0x7);
    }

    bool contain(int val) const noexcept
    {
        return _data[val >> 3] & (1 << (val & 0x7));
    }

    bool contain(const set<S>& val) const noexcept
    {
        for (auto it = 0; it < S; ++it) {
            if ((_data[it] & val._data[it]) != val._data[it])
                return false;
        }
        return true;
    }

    void merge(const set<S>& val) noexcept
    {
        for (auto it = 0; it < S; ++it) {
            _data[it] |= val._data[it];
        }
    }

    int get() const noexcept {
        for (auto cit = 0; cit < S; ++cit) {
            for (auto it = 0; it < 0x8; ++it)
                if (_data[cit] & (1 << it)) return cit*0x8 + it;
        }
        return -1;
    }

    int size() const noexcept {
        auto count = 0;
        for (auto cit = 0; cit < S; ++cit) {
            for (auto it = 0; it < 0x8; ++it)
                if (_data[cit] & (1 << it)) ++count;
        }
        return count;
    }

    template <int P>
    class iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = int;
        using pointer           = int*;
        using reference         = int&;

        int _cell = 0;
        int _offset = 0;
        const set<P>& _data;

    public:
        explicit iterator(const set<S>& data, int val = 0) : _cell(val >> 3), _offset(val & 0x7), _data(data)
        { next(); }
        iterator& operator++() {
            ++_offset;
            return next();
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        bool operator==(iterator& other) const { return _cell == other._cell && _offset == other._offset; }
        bool operator!=(iterator& other) const { return !(*this == other); }
        value_type operator*() const { return _cell*0x8 + _offset; }
    private:
        iterator& next() {
            for (; _cell < P; ++_cell) {
                for (; _offset < 0x8; ++_offset)
                    if (_data._data[_cell] & (1 << _offset)) return *this;
                _offset = 0;
            }
            return *this;
        }
    };

    iterator<S> begin() const { return iterator<S>(*this); }
    iterator<S> end() const { return iterator<S>(*this, S << 3); }

    void serialize(std::stringstream& stream) const
    {
        stream.write(reinterpret_cast<const char*>(_data.data()), S);
    }
    void deserialize(std::stringstream& stream)
    {
        stream.read(reinterpret_cast<char*>(_data.data()), S);
    }
private:
    std::array<unsigned char, S> _data;
};

using set_t = set<>;

}
#endif // SETLIB_H
