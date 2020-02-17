#pragma once

#include <unistd.h>
#include <array>

template <typename T, std::size_t SIZE>
struct RingBuffer {
public:

  RingBuffer(): _tail(0), _size(0) {}

  constexpr std::size_t capacity() const {
     return SIZE;
  }

  uint16_t tail() const { return _tail; }

  uint16_t head() const {
      auto v = _tail + _size;
      return v < SIZE ? v : v - SIZE;
  }

  std::size_t size() const { return _size; }

  std::size_t left() const { return SIZE - size(); }

  bool empty() const { return size() == 0; }

  bool full() const { return size() == SIZE; }

  void push(T value) {
      if (full()) return;
      _data[head()] = value;
      ++_size;
  }

  T pop() {
      if (empty()) return T(0);
      auto v = _data[_tail++];
      --_size;
      if (_tail == SIZE) _tail = 0;
      return v;
  }

   /* 0 <= head < SIZE
    * 0 <= tail < SIZE
    * count <= (head + SIZE - tail) % SIZE
    * tail == head: no copy
    * SIZE - tail < count:
    *   tail < head: copy data[tail..head) -> dst
    * tail > head:
    * copy data[tail..SIZE) and copy[0..head)
    */
  size_t pop(T* dst, size_t count) {
      count = std::min(count, size());
      if (count == 0) return 0;
      auto right = SIZE - tail();
      if (count <= right) { // one piece
        std::copy(std::begin(_data) + tail(),
                  std::begin(_data) + tail() + count,
                  dst);
        _tail += count;
        if (_tail >= SIZE) _tail -= SIZE;
      } else { // two pieces
        std::copy(std::begin(_data),
                  std::begin(_data) + count - right,
                  std::copy(std::begin(_data) + tail(),
                            std::begin(_data) + SIZE,
                            dst));
        _tail = _tail + count - SIZE;
      }
      _size -= count;
      return count;
  }

protected:

  uint16_t _tail;
  uint16_t _size;
  std::array<T, SIZE> _data;
};

