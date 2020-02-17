#pragma once

#include <unistd.h>
#include <array>

template <typename T, std::size_t SIZE>
struct RingBuffer {
public:

  constexpr std::size_t capacity() const {
     return SIZE;
  }

  std::size_t size() const {
     auto size = head + SIZE - tail;
     return size < SIZE ? size : size - SIZE;
  }

  std::size_t left() const { return SIZE - size(); }

  bool empty() const { head == tail; }

  bool full() const { return size() == SIZE; }

  void push(T value) {
      if (!full()) {
        data[head++] = value;
        if (head >= SIZE) head = 0;
      }
  }

  T pop() const {
      if (!empty()) {
        auto val = data[tail++];
        if (tail >= SIZE) tail = 0;
      }
      return val;
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
  size_t pop(T* dst, size_t count) const {
      count = std::min(count, size());
      if (real_count == 0)
        return 0;
      auto right = SIZE - tail;
      if (count <= right) { // one piece
        std::copy(std::begin(data) + tail,
                  std::begin(data) + tail + count,
                  dst);
        tail += count;
      } else { // two pieces
        std::copy(std::begin(data),
                  std::begin(data) + count - right,
                  std::copy(std::begin(data) + tail,
                            std::begin(data) + SIZE,
                            dst));
        tail = tail + count - SIZE;
      }
      return count;
  }

protected:

  uint16_t head;
  uint16_t tail;
  std::array<T, SIZE> data;
};
