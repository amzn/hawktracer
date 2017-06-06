#ifndef HAWKTRACER_VECTOR_H_
#define HAWKTRACER_VECTOR_H_

#include <cassert>
#include <cstring>
#include <utility>
#include <functional>
#include <algorithm>

namespace hawktracer
{

template<typename T>
class Vector
{
public:
    const size_t invalid_size = (size_t)-1;

    Vector(std::size_t capacity = 64)
    {
        assert(capacity >= 8);

        _min_capacity = capacity;
        _capacity = capacity;
        _data = new T[capacity];
    }

    ~Vector()
    {
        delete [] _data;
    }

    template<typename ... ArgsType>
    void emplace_back(ArgsType ... args)
    {
        if (_size == _capacity)
        {
            _resize(_capacity * 2);
        }

        _data[_size++] = T(args...);
    }

    void erase(std::size_t pos)
    {
        assert(pos < _size);

        _size--;
        std::swap(_data[pos], _data[_size]);

        if ((_size * 2) < _capacity && _capacity > _min_capacity)
        {
            _resize(_capacity / 2);
        }
    }

    template<typename KEY, typename COMPARATOR = std::equal_to<KEY>>
    size_t find(const KEY& key, COMPARATOR comparator = COMPARATOR())
    {
        for (size_t i = 0; i < _size; i++)
        {
            if (comparator(key, _data[i]))
            {
                return i;
            }
        }

        return invalid_size;
    }

    T& operator[](size_t pos)
    {
        assert(pos < _size);

        return _data[pos];
    }

    size_t size() const { return _size; }

private:
    T* _data = nullptr;
    size_t _capacity;
    size_t _min_capacity;
    size_t _size = 0;

    void _resize(size_t new_capacity)
    {
        T* new_data = new T[new_capacity];

        memcpy(new_data, _data, std::min(_size, new_capacity) * sizeof(T));
        delete [] _data;

        _data = new_data;
        _capacity = new_capacity;
    }

};

} // namespace hawktracer

#endif // HAWKTRACER_VECTOR_H_
