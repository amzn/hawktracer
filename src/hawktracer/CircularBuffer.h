#ifndef HAWKTRACER_CIRCULARBUFFER_H
#define HAWKTRACER_CIRCULARBUFFER_H

#include <cassert>
#include <stdexcept>

namespace hawktracer
{

template<typename T, std::size_t SIZE>
class CircularBuffer
{
    static_assert(SIZE > 0, "CircularBuffer size must be greater than 0");
public:
    std::size_t _head;
    std::size_t _tail;
    bool _empty = true;

    T _data[SIZE];

public:
    void push(const T& value)
    {
        if (_empty)
        {
            _head = _tail = 0;
            _empty = false;
        }
        else
        {
            std::size_t next = (_head + 1) % SIZE;

            if (next == _tail)
            {
                _tail = (_tail + 1) % SIZE;
            }

            _head = next;
        }

        _data[_head] = value;
    }

    T pop()
    {
        T value = front();

        if (_head == _tail)
        {
            _empty = true;
        }
        else
        {
            _tail = (_tail + 1) % SIZE;
        }

        return value;
    }

    T front()
    {
        assert(!_empty && "try to get front element from empty queue");

        return _data[_tail];
    }

    bool empty() const
    {
        return _empty;
    }
};

} // namespace hawktracer

#endif // HAWKTRACER_CIRCULARBUFFER_H
