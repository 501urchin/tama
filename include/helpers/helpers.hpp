#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>
#include <algorithm>



namespace helpers {
    double simdSum(std::span<const double> elms);
    
// TOOD: the buf is way to slo for my liking. particularly at the min and mac functions
// an apprach could be to keep track of inserted values. as soon as a val is inserted we compare it to min/max vars in the class and if g or s swap
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
        
        void insert(const std::span<const T> vals) {
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

        T max() const {
            return *(std::max_element(buf.begin(), buf.end()));
        }
        
        T min() const {
            return *(std::min_element(buf.begin(), buf.end()));
        }
        
        bool empty() const {
            return buf.empty();
        }
    };


} // namespace helpers
