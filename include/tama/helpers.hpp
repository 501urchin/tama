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
            std::unique_ptr<T[]> buf;
            size_t headIdx;
            size_t size;
        public: 
            RingBuffer(size_t size, const std::vector<T>& initialValues): buf(std::make_unique<T[]>(size)), headIdx(0), size(size) {
                if (initialValues.size() != size) {
                    throw std::invalid_argument("initialValues must match buffer size");
                }

                for (size_t i = 0; i < size; ++i) {
                    buf[i] = initialValues[i];
                }
            }

            T head() {
                return this->buf[this->headIdx];
            };

            void insert(const T& value) {
                this->buf[this->headIdx] = value;
                this->headIdx = (this->headIdx + 1) % this->size;
            };

            T operator[](size_t i) const {
                return buf[(this->headIdx + i) % this->size]; 
            }
    };
} // namespace helpers
