#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace helpers {
    double simdSum(std::span<const double> elms);
    
template <typename T>
class RingBuffer {
private:
    int headIdx;
    size_t capacity;
    std::vector<T> buf;

public:
    RingBuffer(int size) : headIdx(0), capacity(size) {
        if (size <= 0) {
            throw std::invalid_argument("invalid size");
        }
        buf.reserve(size);
    }

    T head() const {
        if (buf.empty()) {
            throw std::runtime_error("buffer is empty");
        }

        return buf[headIdx];
    }

    void insert(const std::vector<T>& vals) {
        for (const auto& val : vals) {
            if (buf.size() < capacity) {
                buf.push_back(val);
            } else {
                buf[headIdx] = val;
                headIdx = (headIdx + 1) % capacity;
            }
        }
    }
    
    void insert(const T& val) {
        if (buf.size() < capacity) {
            buf.push_back(val);
        } else {
            buf[headIdx] = val;
            headIdx = (headIdx + 1) % capacity;
        }
    }

    T operator[](int i){
        if (i >= capacity || buf.empty()) {
            throw std::out_of_range("index out of range");
        }
        
        return buf[(headIdx + i) % capacity];
    }

    int len() const {
        return buf.size();
    }

    int cap() const {
        return capacity;
    }
};

} // namespace helpers
