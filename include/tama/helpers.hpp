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
        size_t headIdx;     
        size_t capacity;
        std::vector<T> buf;

    public:
        RingBuffer(size_t size) : headIdx(0), capacity(size) {
            if (size == 0) {
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
                insert(val);
            }
        }

        void insert(const T& val) {
            if (buf.size() < capacity) {
                buf.push_back(val);
            } else {
                buf[headIdx] = val;

                headIdx++;
                if (headIdx >= capacity) {
                    headIdx = 0;
                }
            }
        }

        T operator[](size_t i) const {
            if (i >= buf.size()) {
                throw std::out_of_range("index out of range");
            }

            size_t idx = headIdx + i;
            if (idx >= capacity) {
                idx -= capacity;
            }

            return buf[idx];
        }

        size_t len() const {
            return buf.size();
        }

        size_t cap() const {
            return capacity;
        }
    };


} // namespace helpers
